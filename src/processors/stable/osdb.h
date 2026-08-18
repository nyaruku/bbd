#pragma once
#include <vector>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <zlib.h>
#include "helper.h"
#include "structs.h"

namespace processor::osu::stable::osdb {
    // .osdb (CollectionManager by Piotrekol)
    // File layout (version >= 7, which is all current files):
    //   ULEB128+bytes  outer version string (e.g. "o!dm8")
    //   GZip blob {
    //     ULEB128+bytes  inner version string (same)
    //     double         OA date (days since 1899-12-30)
    //     ULEB128+bytes  editor username
    //     int32          collection count
    //     per collection:
    //       ULEB128+bytes  name
    //       int32          onlineId        (v>=7)
    //       int32          beatmap count
    //       per beatmap:
    //         int32        mapId
    //         int32        mapSetId        (v>=2)
    //         ULEB128+bytes artist          (full only)
    //         ULEB128+bytes title           (full only)
    //         ULEB128+bytes diffName        (full only)
    //         ULEB128+bytes md5
    //         ULEB128+bytes userComment     (v>=4)
    //         byte         playMode        (v>=5 full, or v>=8)
    //         double       starRating      (v>=6 full, or v>=8)
    //       int32          hash-only count (v>=3)
    //       per hash: ULEB128+bytes md5
    //     "By Piotrekol" footer
    //   }

    inline std::vector<uint8_t> gzipDecompress(std::istream& source) {
        std::vector<uint8_t> compressedBytes(
            (std::istreambuf_iterator<char>(source))
            ,std::istreambuf_iterator<char>()
        );

        std::vector<uint8_t> output;
        output.resize(std::max<size_t>(compressedBytes.size() * 4, 65536u));

        z_stream zstream{};
        // 15+16 = GZip mode
        if (inflateInit2(&zstream, 15 + 16) != Z_OK) {
            throw std::runtime_error("zlib inflateInit2 failed");
        }

        zstream.next_in = compressedBytes.data();
        zstream.avail_in = static_cast<uInt>(compressedBytes.size());

        int zlibResult;
        do {
            if (zstream.total_out >= output.size()) {
                output.resize(output.size() * 2);
            }
            zstream.next_out = output.data() + zstream.total_out;
            zstream.avail_out = static_cast<uInt>(output.size() - zstream.total_out);
            zlibResult = inflate(&zstream, Z_NO_FLUSH);
        } while (zlibResult == Z_OK);

        if (zlibResult != Z_STREAM_END) {
            throw std::runtime_error("GZip decompress failed (zlib code " + std::to_string(zlibResult) + ")");
        }

        output.resize(zstream.total_out);
        inflateEnd(&zstream);
        return output;
    }

    inline std::vector<structs::collection> parseOsdb(std::istream& file) {
        static const std::unordered_map<std::string, int> versionMap = {
            {"o!dm", 1}
            ,{"o!dm2", 2}
            ,{"o!dm3", 3}
            ,{"o!dm4", 4}
            ,{"o!dm5", 5}
            ,{"o!dm6", 6}
            ,{"o!dm7", 7}
            ,{"o!dm8", 8}
            ,{"o!dm7min",1007}
            ,{"o!dm8min",1008}
        };

        std::string versionStr = readDotnetString(file);
        if (auto found = versionMap.find(versionStr); found == versionMap.end()) {
            throw std::runtime_error("Unknown osdb version string: " + versionStr);
        }

        int fileVersion = versionMap.at(versionStr);
        bool minimal = (versionStr.size() >= 3 && versionStr.substr(versionStr.size() - 3) == "min");

        std::string decompressedStr;
        std::istream* stream = &file;
        std::istringstream decompressedStream;

        if (fileVersion >= 7) {
            auto decompressedBytes = gzipDecompress(file);
            decompressedStr.assign(reinterpret_cast<char*>(decompressedBytes.data()), decompressedBytes.size());
            decompressedStream = std::istringstream(decompressedStr, std::ios::binary);
            // discard inner version string
            readDotnetString(decompressedStream);
            stream = &decompressedStream;
        }

        read<double>(*stream); // OA date
        readDotnetString(*stream); // editor username
        int32_t collectionCount = read<int32_t>(*stream);

        std::vector<structs::collection> collections;
        collections.reserve(collectionCount);

        for (int32_t i = 0; i < collectionCount; ++i) {
            structs::collection collection;
            collection.name = readDotnetString(*stream);
            if (fileVersion >= 7) {
                read<int32_t>(*stream); // onlineId
            }

            int32_t beatmapCount = read<int32_t>(*stream);
            collection.beatmaps.reserve(beatmapCount);
            collection.hashes.reserve(beatmapCount);

            for (int32_t j = 0; j < beatmapCount; ++j) {
                const auto beatmapStart = stream->tellg();
                structs::beatmap beatmap;
                try {
                    beatmap.beatmapId = read<int32_t>(*stream);
                    if (fileVersion >= 2) {
                        beatmap.beatmapSetId = read<int32_t>(*stream);
                    }

                    if (!minimal) {
                        beatmap.artist = readDotnetString(*stream);
                        beatmap.title = readDotnetString(*stream);
                        beatmap.difficulty = readDotnetString(*stream);
                    }

                    beatmap.md5 = readDotnetString(*stream);
                    if (fileVersion >= 4) {
                        readDotnetString(*stream); // userComment
                    }
                    if (fileVersion >= 8 || (fileVersion >= 5 && !minimal)) {
                        read<uint8_t>(*stream); // playMode
                    }
                    if (fileVersion >= 8 || (fileVersion >= 6 && !minimal)) {
                        beatmap.starRating = read<double>(*stream);
                    }

                    collection.hashes.push_back(beatmap.md5);
                    collection.beatmaps.push_back(std::move(beatmap));
                } catch (const std::runtime_error& error) {
                    if (std::string(error.what()) != "Unexpected end of file" || beatmapStart == std::streampos(-1)) {
                        throw;
                    }
                    stream->clear();
                    stream->seekg(beatmapStart);
                    break;
                }
            }

            if (fileVersion >= 3) {
                int32_t hashCount = read<int32_t>(*stream);
                for (int32_t j = 0; j < hashCount; ++j) {
                    collection.hashes.push_back(readDotnetString(*stream));
                }
            }

            collections.push_back(std::move(collection));
        }
        return collections;
    }

    inline std::vector<structs::collection> parseOsdb(const std::string& path) {
        std::ifstream fileStream(path, std::ios::binary);
        if (!fileStream)
            throw std::runtime_error("Cannot open: " + path);
        return parseOsdb(fileStream);
    }

    // Resolve osu!.db metadata into collections
    inline void resolveBeatmaps(std::vector<structs::collection>& collections, const std::unordered_map<std::string, structs::beatmap>& beatmapMap) {
        for (auto& collection : collections) {
            collection.beatmaps.clear();
            for (const auto& hash : collection.hashes) {
                if (auto found = beatmapMap.find(hash); found != beatmapMap.end()) {
                    collection.beatmaps.push_back(found->second);
                } else {
                    structs::beatmap missing;
                    missing.md5 = hash;
                    collection.beatmaps.push_back(std::move(missing));
                }
            }
        }
    }
}
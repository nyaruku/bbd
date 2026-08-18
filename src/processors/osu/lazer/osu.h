#pragma once

#include <array>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

#include <cpprealm/sdk.hpp>
#include "../structs.h"

struct RealmUser {
    int64_t OnlineID;
    std::string Username;
    std::string CountryCode;
};

struct BeatmapCollection {
    realm::primary_key<realm::uuid> ID;
    std::string Name;
    std::vector<std::string> BeatmapMD5Hashes;
    std::chrono::time_point<std::chrono::system_clock> LastModified;
};

struct BeatmapMetadata {
    std::string Title;
    std::string TitleUnicode;
    std::string Artist;
    std::string ArtistUnicode;
    RealmUser* Author;
};

struct BeatmapSet {
    realm::primary_key<realm::uuid> ID;
    int64_t OnlineID = 0;
};

struct Beatmap {
    realm::primary_key<realm::uuid> ID;
    int64_t OnlineID = 0;
    std::string MD5Hash;
    std::string DifficultyName;
    double StarRating;
    BeatmapMetadata* Metadata;
    BeatmapSet* BeatmapSet;
};

namespace realm {
    REALM_SCHEMA(BeatmapSet, ID, OnlineID)
    REALM_EMBEDDED_SCHEMA(RealmUser, OnlineID, Username, CountryCode)
    REALM_SCHEMA(BeatmapCollection, ID, Name, BeatmapMD5Hashes, LastModified)
    REALM_EMBEDDED_SCHEMA(BeatmapMetadata, Title, TitleUnicode, Artist, ArtistUnicode, Author)
    REALM_SCHEMA(Beatmap, ID, OnlineID, MD5Hash, DifficultyName, StarRating, Metadata, BeatmapSet)
}

namespace processor::osu::lazer {
    inline std::vector<structs::collection> loadCollections(const std::string& path) {
        realm::db_config dbConfig;
        dbConfig.set_path(path);
        dbConfig.set_schema_version(46);
        dbConfig.set_schema_mode(realm::db_config::schema_mode::read_only);
        auto database = realm::open<RealmUser, BeatmapCollection, Beatmap, BeatmapMetadata, BeatmapSet>(dbConfig);

        std::unordered_map<std::string, structs::beatmap> beatmapMap;
        for (const auto& entry : database.objects<Beatmap>()) {
            std::string md5 = entry.MD5Hash.detach();
            if (md5.empty()) {
                continue;
            }

            structs::beatmap beatmap;
            beatmap.md5 = md5;
            beatmap.beatmapId = static_cast<int32_t>(entry.OnlineID);
            if (entry.BeatmapSet) {
                beatmap.beatmapSetId = static_cast<int32_t>(entry.BeatmapSet->OnlineID);
            }
            beatmap.difficulty = entry.DifficultyName.detach();
            beatmap.starRating = entry.StarRating;

            if (entry.Metadata) {
                beatmap.title  = entry.Metadata->Title.detach();
                beatmap.artist = entry.Metadata->Artist.detach();
                if (entry.Metadata->Author) {
                    beatmap.creator = entry.Metadata->Author->Username.detach();
                }
            }

            beatmapMap[md5] = std::move(beatmap);
        }

        std::vector<structs::collection> collections;
        for (const auto& entry : database.objects<BeatmapCollection>()) {
            structs::collection collection;
            collection.name = entry.Name.detach();
            if (collection.name.empty()) {
                collection.name = "(unnamed)";
            }

            for (const auto& hash : entry.BeatmapMD5Hashes.detach()) {
                collection.hashes.push_back(hash);

                if (auto found = beatmapMap.find(hash); found != beatmapMap.end()) {
                    collection.beatmaps.push_back(found->second);
                } else {
                    structs::beatmap missing;
                    missing.md5 = hash;
                    collection.beatmaps.push_back(std::move(missing));
                }
            }
            collections.push_back(std::move(collection));
        }
        return collections;
    }
}
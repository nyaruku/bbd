#pragma once
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <zlib.h>
#include "helper.h"
#include "structs.h"

namespace processor::osu::stable::osu {
    inline void skipOsuString(std::istream& stream) {
        uint8_t indicator = read<uint8_t>(stream);
        if (indicator == 0x00) {
            return;
        }
        if (indicator != 0x0b) {
            throw std::runtime_error("Invalid osu string indicator: " + std::to_string(indicator));
        }
        stream.seekg(readUleb128(stream), std::ios::cur);
    }

    // Skip timing points (17 bytes each: double+double+bool)
    inline void skipTimingPoints(std::istream& stream) {
        uint32_t count = read<uint32_t>(stream);
        stream.seekg(static_cast<std::streamoff>(count) * 17, std::ios::cur);
    }

    inline std::unordered_map<std::string, structs::beatmap> parseOsuDb(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open: " + path);
        }

        uint32_t version = read<uint32_t>(file);
        if (version < 20191106) {
            throw std::runtime_error("osu!.db version too old (" + std::to_string(version) + "), need >= 20191106");
        }

        // folder_count
        read<uint32_t>(file);
        // account_unlocked
        read<uint8_t>(file) != 0;
        // date_unlocked
        read<int64_t>(file);
        // player name
        skipOsuString(file);
        uint32_t beatmapCount = read<uint32_t>(file);

        std::unordered_map<std::string, structs::beatmap> beatmapMap;
        beatmapMap.reserve(beatmapCount);

        for (uint32_t i = 0; i < beatmapCount; ++i) {
            structs::beatmap beatmap;
            beatmap.artist = readOsuString(file); // artist ASCII
            skipOsuString(file); // artist Unicode
            beatmap.title = readOsuString(file); // title ASCII
            skipOsuString(file); // title Unicode
            beatmap.creator = readOsuString(file);
            beatmap.difficulty = readOsuString(file);
            skipOsuString(file); // audio filename
            beatmap.md5 = readOsuString(file);
            skipOsuString(file); // .osu filename

            read<uint8_t>(file); // ranked_status
            read<uint16_t>(file); // circles
            read<uint16_t>(file); // sliders
            read<uint16_t>(file); // spinners
            read<int64_t>(file); // last_modified
            read<float>(file); // AR
            read<float>(file); // CS
            read<float>(file); // HP
            read<float>(file); // OD
            read<double>(file); // slider_velocity

            // 4 star-rating maps (one per game mode).
            // Each entry = 1 byte (type=8/int32) + 4 bytes mods + 1 byte (type=0x0c/float32) + 4 bytes stars = 10 bytes.
            for (int mode = 0; mode < 4; mode++) {
                uint32_t count = read<uint32_t>(file);
                if (mode == 0 && count > 0) {
                    read<uint8_t>(file); // type byte (8 = int32)
                    read<uint32_t>(file); // mods
                    read<uint8_t>(file); // type byte (0x0c = float32)
                    beatmap.starRating = read<float>(file);
                    count--;
                }
                // Skip remaining entries (10 bytes each: 1+4+1+4)
                file.seekg(static_cast<std::streamoff>(count) * 10, std::ios::cur);
            }

            read<uint32_t>(file); // drain_time
            read<uint32_t>(file); // total_time
            read<uint32_t>(file); // preview_offset

            skipTimingPoints(file);

            beatmap.beatmapId = read<int32_t>(file);
            beatmap.beatmapSetId = read<int32_t>(file);
            read<int32_t>(file); // thread_id
            read<uint8_t>(file); // grade_std
            read<uint8_t>(file); // grade_taiko
            read<uint8_t>(file); // grade_ctb
            read<uint8_t>(file); // grade_mania
            read<uint16_t>(file); // local_offset
            read<float>(file); // stack_leniency
            read<uint8_t>(file); // game_mode
            skipOsuString(file); // source
            skipOsuString(file); // tags
            read<uint16_t>(file); // online_offset
            skipOsuString(file); // title font
            read<uint8_t>(file) != 0; // unplayed
            read<int64_t>(file); // last_played
            read<uint8_t>(file) != 0; // is_osz2
            skipOsuString(file); // folder name
            read<int64_t>(file); // last_checked
            read<uint8_t>(file) != 0; // ignore_sounds
            read<uint8_t>(file) != 0; // ignore_skin
            read<uint8_t>(file) != 0; // disable_storyboard
            read<uint8_t>(file) != 0; // disable_video
            read<uint8_t>(file) != 0; // visual_override
            read<uint32_t>(file); // last_modified2
            read<uint8_t>(file); // scroll_speed

            if (!beatmap.md5.empty()) {
                beatmapMap[beatmap.md5] = std::move(beatmap);
            }
        }
        return beatmapMap;
    }
}

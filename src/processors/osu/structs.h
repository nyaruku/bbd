#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace processor::osu::structs {
    struct beatmap {
        std::string md5;
        std::string title;
        std::string artist;
        std::string difficulty;
        std::string creator;
        double starRating = 0.0;
        int32_t beatmapId = 0;
        int32_t beatmapSetId = 0;
    };

    struct collection {
        std::string name;
        std::vector<std::string> hashes;
        std::vector<beatmap> beatmaps;
    };
}
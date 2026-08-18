#pragma once
#include <vector>
#include "helper.h"
#include "../structs.h"

namespace processor::osu::stable::collection {
    inline std::vector<structs::collection> parseCollectionDb(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open: " + path);
        }

        // version
        read<uint32_t>(file);

        std::vector<structs::collection> collections(read<uint32_t>(file));

        for (uint32_t i = 0; i < collections.size(); ++i) {
            auto& collection = collections[i];
            collection.name = readOsuString(file);
            uint32_t hashCount = read<uint32_t>(file);
            collection.hashes.reserve(hashCount);
            for (uint32_t j = 0; j < hashCount; ++j) {
                collection.hashes.push_back(readOsuString(file));
            }
        }
        return collections;
    }
}
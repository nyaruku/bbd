#pragma once
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

namespace processor::stable {
    inline void readBytes(std::istream& stream, void* dst, std::streamsize count) {
        if (!stream.read(static_cast<char*>(dst), count)) {
            throw std::runtime_error("Unexpected end of file");
        }
    }

    template<typename T>
    T read(std::istream& stream) {
        T value{};
        readBytes(stream, &value, sizeof(T));
        return value;
    }

    // ULEB128 used for both osu! string lengths and .NET BinaryWriter string lengths
    inline uint32_t readUleb128(std::istream& stream) {
        uint32_t result = 0;
        int shift = 0;
        uint8_t byte;
        do {
            byte = read<uint8_t>(stream);
            result |= static_cast<uint32_t>(byte & 0x7F) << shift;
            shift += 7;
        } while (byte & 0x80);
        return result;
    }

    // osu! DB string: 0x00 = null/empty, 0x0b = ULEB128 length + UTF-8 bytes
    inline std::string readOsuString(std::istream& stream) {
        uint8_t indicator = read<uint8_t>(stream);
        if (indicator == 0x00) {
            return "";
        }
        if (indicator != 0x0b) {
            throw std::runtime_error("Invalid osu string indicator: " + std::to_string(indicator));
        }
        uint32_t length = readUleb128(stream);
        std::string result(length, '\0');
        if (length) {
            readBytes(stream, result.data(), length);
        }
        return result;
    }

    // .NET BinaryWriter string: ULEB128 length + UTF-8 bytes (no indicator byte)
    inline std::string readDotnetString(std::istream& stream) {
        uint32_t length = readUleb128(stream);
        std::string result(length, '\0');
        if (length) {
            readBytes(stream, result.data(), length);
        }
        return result;
    }
}
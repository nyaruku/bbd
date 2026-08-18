#pragma once

#include <array>
#include <filesystem>

namespace helpers::resolver {
    static std::string exec(const char* cmd) {
        FILE* pipe = popen(cmd, "r");
        if (!pipe)
            return "";
        std::string output = "";
        std::array<char, 256> buffer;
        while (fgets(buffer.data(), buffer.size(), pipe)) {
            output += buffer.data();
        }
        pclose(pipe);
        return output;
    }

    // Parses the osu! stable folder from `osu-wine --info`.
    // String: "osu! folder: '/path/to/osu!/osugame'"
    static std::string osuWineStableDir() {
        std::string output = exec("osu-wine --info 2>/dev/null");
        const std::string marker = "osu! folder: '";

        size_t start = output.find(marker);
        if (start == std::string::npos)
            return "";
        start += marker.size();
        const size_t end = output.find('\'', start);
        if (end == std::string::npos)
            return "";
        return output.substr(start, end - start);
    }

    static std::string stableDir() {
        const std::string path = osuWineStableDir();
        if (!path.empty() && std::filesystem::exists(std::filesystem::path(path) / "osu!.exe"))
            return path;
        return "";
    }

    // The Flatpak data root is always $HOME/.var/app/<app-id>/data (Flatpak XDG sandbox spec).
    static std::string flatpakLazerDir() {
       if (std::getenv("HOME") == nullptr || exec("flatpak info sh.ppy.osu 2>/dev/null").empty())
            return "";
        return std::string(std::getenv("HOME")) + "/.var/app/sh.ppy.osu/data/osu";
    }

    // osu! lazer follows the XDG base directory spec for its data directory.
    // XDG spec: if XDG_DATA_HOME is unset, the default is $HOME/.local/share
    static std::string lazerRealm() {
        const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
        const char* home = std::getenv("HOME");
        const std::string xdgData = xdgDataHome ? xdgDataHome : (home ? std::string(home) + "/.local/share" : "");
        const std::string flatpakDir = flatpakLazerDir();

        for (const auto& path : {
            xdgData.empty() ? "" : xdgData + "/osu/client.realm",
            flatpakDir.empty() ? "" : flatpakDir + "/client.realm",
        }) {
            if (!path.empty() && std::filesystem::exists(path))
                return path;
        }
        return {};
    }
}

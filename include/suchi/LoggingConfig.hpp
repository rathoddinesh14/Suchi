#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "suchi/LogLevel.hpp"

namespace suchi {

struct LoggingConfig {
    LogLevel minimumLevel = LogLevel::Info;
    std::string backend = "console";
    bool includeTimestamp = true;
    bool includeThreadId = false;
    bool includeSourceLocation = false;
    bool colorize = true;
    std::filesystem::path outputFile;
    std::unordered_map<std::string, std::string> backendOptions;

    // Strict key=value loader. Unknown keys and malformed values are reported.
    [[nodiscard]] static LoggingConfig defaults();
    [[nodiscard]] static LoggingConfig fromFile(const std::filesystem::path& path);
};

} // namespace suchi

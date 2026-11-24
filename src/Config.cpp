#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>

#include "suchi/LoggingConfig.hpp"

namespace suchi {
namespace {
std::string trim(std::string value) {
    const auto first = std::find_if_not(value.begin(), value.end(),
                                        [](unsigned char c) { return std::isspace(c) != 0; });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
                          return std::isspace(c) != 0;
                      }).base();
    if (first >= last)
        return {};
    return std::string(first, last);
}
LogLevel parseLevel(const std::string& value) {
    auto v = value;
    std::transform(v.begin(), v.end(), v.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    if (v == "TRACE")
        return LogLevel::Trace;
    if (v == "DEBUG")
        return LogLevel::Debug;
    if (v == "INFO")
        return LogLevel::Info;
    if (v == "WARN" || v == "WARNING")
        return LogLevel::Warning;
    if (v == "ERROR")
        return LogLevel::Error;
    if (v == "CRITICAL")
        return LogLevel::Critical;
    if (v == "OFF")
        return LogLevel::Off;
    throw std::runtime_error("Invalid Suchi log level: " + value);
}
bool parseBool(const std::string& value, const std::string& key) {
    if (value == "true" || value == "1" || value == "yes")
        return true;
    if (value == "false" || value == "0" || value == "no")
        return false;
    throw std::runtime_error("Invalid boolean for '" + key + "': " + value);
}
} // namespace
LoggingConfig LoggingConfig::defaults() {
    return {};
}
LoggingConfig LoggingConfig::fromFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input)
        throw std::runtime_error("Unable to open Suchi configuration file: " + path.string());
    LoggingConfig config;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty() || line.front() == '#' || line.front() == ';')
            continue;
        const auto equals = line.find('=');
        if (equals == std::string::npos)
            throw std::runtime_error("Malformed Suchi config at line " +
                                     std::to_string(lineNumber));
        const auto key = trim(line.substr(0, equals));
        const auto value = trim(line.substr(equals + 1));
        if (key == "level")
            config.minimumLevel = parseLevel(value);
        else if (key == "backend")
            config.backend = value;
        else if (key == "timestamp")
            config.includeTimestamp = parseBool(value, key);
        else if (key == "thread_id")
            config.includeThreadId = parseBool(value, key);
        else if (key == "source_location")
            config.includeSourceLocation = parseBool(value, key);
        else if (key == "color")
            config.colorize = parseBool(value, key);
        else if (key == "output_file")
            config.outputFile = value;
        else if (key.rfind("backend.", 0) == 0)
            config.backendOptions.emplace(key.substr(8), value);
        else
            throw std::runtime_error("Unknown Suchi configuration key at line " +
                                     std::to_string(lineNumber) + ": " + key);
    }
    return config;
}
} // namespace suchi

#pragma once

#include "suchi/LogField.hpp"
#include "suchi/LogLevel.hpp"

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

namespace suchi {

struct SourceLocation {
    std::string file;
    std::string function;
    std::uint_least32_t line = 0;
};

struct LogRecord {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level = LogLevel::Info;
    std::string loggerName;
    std::string message;
    LogFields fields;
    LogFields context;
    SourceLocation source;
    std::thread::id threadId;
};

} // namespace suchi

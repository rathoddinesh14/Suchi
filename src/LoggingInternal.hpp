#pragma once
#include <mutex>
#include <unordered_map>

#include "suchi/Backend.hpp"

namespace suchi::detail {
struct Runtime {
    mutable std::mutex mutex;
    LoggingConfig config;
    std::unique_ptr<ILoggingBackend> backend;
    bool initialized = false;
};
Runtime& runtime();
void dispatch(LogRecord record) noexcept;
bool enabled(LogLevel level) noexcept;
} // namespace suchi::detail

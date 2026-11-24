#include "suchi/Logger.hpp"

#include <utility>

#include "LoggingInternal.hpp"

namespace suchi {
Logger::Logger(std::string name, LogFields context)
    : name_(std::move(name)), context_(std::move(context)) {}

Logger Logger::withContext(LogFields fields) const {
    auto combined = context_;
    combined.insert(combined.end(), fields.begin(), fields.end());
    return Logger{name_, std::move(combined)};
}

void Logger::log(LogLevel level, std::string_view message, LogFields fields,
                 std::source_location location) const noexcept {
    if (!detail::enabled(level))
        return;
    try {
        LogRecord record;
        record.timestamp = std::chrono::system_clock::now();
        record.level = level;
        record.loggerName = name_;
        record.message.assign(message);
        record.fields = std::move(fields);
        record.context = context_;
        record.source = {location.file_name(), location.function_name(), location.line()};
        record.threadId = std::this_thread::get_id();
        detail::dispatch(std::move(record));
    } catch (...) {
        // Logging must not throw into application code. Avoid recursive logging here.
    }
}
void Logger::trace(std::string_view m, LogFields f) const noexcept {
    log(LogLevel::Trace, m, std::move(f));
}
void Logger::debug(std::string_view m, LogFields f) const noexcept {
    log(LogLevel::Debug, m, std::move(f));
}
void Logger::info(std::string_view m, LogFields f) const noexcept {
    log(LogLevel::Info, m, std::move(f));
}
void Logger::warning(std::string_view m, LogFields f) const noexcept {
    log(LogLevel::Warning, m, std::move(f));
}
void Logger::error(std::string_view m, LogFields f) const noexcept {
    log(LogLevel::Error, m, std::move(f));
}
void Logger::critical(std::string_view m, LogFields f) const noexcept {
    log(LogLevel::Critical, m, std::move(f));
}
} // namespace suchi

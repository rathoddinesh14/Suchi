#include "suchi/Backend.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <type_traits>

namespace suchi {
namespace {
std::string valueToString(const FieldValue& value) {
    return std::visit([](const auto& item) {
        using T = std::decay_t<decltype(item)>;
        if constexpr (std::is_same_v<T, bool>) return std::string(item ? "true" : "false");
        else if constexpr (std::is_same_v<T, std::string>) return item;
        else { std::ostringstream out; out << item; return out.str(); }
    }, value);
}
class ConsoleBackend final : public ILoggingBackend {
public:
    explicit ConsoleBackend(LoggingConfig config) : config_(std::move(config)) {}
    BackendDescriptor descriptor() const override { return {"console", "Built-in standard error console backend"}; }
    void write(const LogRecord& record) noexcept override {
        try {
            std::lock_guard lock(mutex_);
            std::ostringstream line;
            if (config_.includeTimestamp) {
                const auto time = std::chrono::system_clock::to_time_t(record.timestamp);
                std::tm tm{};
#if defined(_WIN32)
                localtime_s(&tm, &time);
#else
                localtime_r(&time, &tm);
#endif
                line << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << ' ';
            }
            line << '[' << toString(record.level) << "] [" << record.loggerName << "] " << record.message;
            for (const auto& item : record.context) line << " " << item.key << '=' << valueToString(item.value);
            for (const auto& item : record.fields) line << " " << item.key << '=' << valueToString(item.value);
            if (config_.includeThreadId) line << " [thread=" << record.threadId << ']';
            if (config_.includeSourceLocation && !record.source.file.empty())
                line << " (" << record.source.file << ':' << record.source.line << ')';
            std::cerr << line.str() << '\n';
        } catch (...) { /* Never throw from backend write. */ }
    }
    void flush() noexcept override { try { std::lock_guard lock(mutex_); std::cerr.flush(); } catch (...) {} }
private:
    LoggingConfig config_;
    std::mutex mutex_;
};
}
std::unique_ptr<ILoggingBackend> makeConsoleBackend(const LoggingConfig& config) {
    return std::make_unique<ConsoleBackend>(config);
}
} // namespace suchi

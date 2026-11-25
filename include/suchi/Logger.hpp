#pragma once

#include <source_location>
#include <string>
#include <string_view>

#include "suchi/LogField.hpp"
#include "suchi/LogLevel.hpp"
#include "suchi/LogRecord.hpp"

namespace suchi {

class Logger {
public:
    Logger() = default;
    explicit Logger(std::string name, LogFields context = {});

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] Logger withContext(LogFields fields) const;

    void log(LogLevel level, std::string_view message, LogFields fields = {},
             std::source_location location = std::source_location::current()) const noexcept;
    void trace(std::string_view message, LogFields fields = {}) const noexcept;
    void debug(std::string_view message, LogFields fields = {}) const noexcept;
    void info(std::string_view message, LogFields fields = {}) const noexcept;
    void warning(std::string_view message, LogFields fields = {}) const noexcept;
    void error(std::string_view message, LogFields fields = {}) const noexcept;
    void critical(std::string_view message, LogFields fields = {}) const noexcept;

private:
    std::string name_;
    LogFields context_;
};

} // namespace suchi

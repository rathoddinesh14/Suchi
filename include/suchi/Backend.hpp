#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "suchi/LogRecord.hpp"
#include "suchi/LoggingConfig.hpp"

namespace suchi {

struct BackendDescriptor {
    std::string name;
    std::string description;
};

class ILoggingBackend {
public:
    virtual ~ILoggingBackend() = default;
    [[nodiscard]] virtual BackendDescriptor descriptor() const = 0;
    virtual void write(const LogRecord& record) noexcept = 0;
    virtual void flush() noexcept = 0;
};

using BackendFactory = std::unique_ptr<ILoggingBackend> (*)(const LoggingConfig&);

} // namespace suchi

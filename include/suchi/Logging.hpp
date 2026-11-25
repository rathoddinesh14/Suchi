#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "suchi/Backend.hpp"
#include "suchi/Logger.hpp"
#include "suchi/LoggingConfig.hpp"

namespace suchi {

class Logging {
public:
    // Initializes or replaces the active runtime. Throws std::runtime_error on failure.
    static void initialize(const LoggingConfig& config = LoggingConfig::defaults());
    static void shutdown() noexcept;
    static void flush() noexcept;
    [[nodiscard]] static Logger getLogger(std::string name);
    [[nodiscard]] static bool isInitialized() noexcept;

    // Register a backend factory before initialize(). Duplicate names are rejected.
    static void registerBackend(std::string name, BackendFactory factory);
};

} // namespace suchi

#include "suchi/Logging.hpp"
#include "LoggingInternal.hpp"

#include <stdexcept>
#include <unordered_map>

namespace suchi::detail {
namespace {
std::unordered_map<std::string, BackendFactory>& factories() {
    static std::unordered_map<std::string, BackendFactory> value;
    return value;
}
std::mutex& factoriesMutex() { static std::mutex value; return value; }
}
Runtime& runtime() { static Runtime value; return value; }

void dispatch(LogRecord record) noexcept {
    auto& state = runtime();
    std::lock_guard lock(state.mutex);
    if (!state.initialized || !state.backend || record.level < state.config.minimumLevel || state.config.minimumLevel == LogLevel::Off) return;
    state.backend->write(record);
}
bool enabled(LogLevel level) noexcept {
    auto& state = runtime();
    std::lock_guard lock(state.mutex);
    return state.initialized && state.backend && state.config.minimumLevel != LogLevel::Off && level >= state.config.minimumLevel;
}
}

namespace suchi {
void Logging::registerBackend(std::string name, BackendFactory factory) {
    if (name.empty() || factory == nullptr) throw std::invalid_argument("Backend name and factory must be valid");
    std::lock_guard lock(detail::factoriesMutex());
    auto [it, inserted] = detail::factories().emplace(std::move(name), factory);
    if (!inserted) throw std::invalid_argument("A backend with this name is already registered");
}

void Logging::initialize(const LoggingConfig& config) {
    BackendFactory factory = nullptr;
    {
        std::lock_guard lock(detail::factoriesMutex());
        const auto it = detail::factories().find(config.backend);
        if (it != detail::factories().end()) factory = it->second;
    }
#ifdef SUCHI_HAS_CONSOLE_BACKEND
    if (config.backend == "console" && factory == nullptr) {
        extern std::unique_ptr<ILoggingBackend> makeConsoleBackend(const LoggingConfig&);
        factory = &makeConsoleBackend;
    }
#endif
    if (factory == nullptr) throw std::runtime_error("Logging backend is not registered: " + config.backend);
    auto backend = factory(config);
    if (!backend) throw std::runtime_error("Logging backend factory returned null: " + config.backend);

    auto& state = detail::runtime();
    std::lock_guard lock(state.mutex);
    if (state.backend) state.backend->flush();
    state.backend = std::move(backend);
    state.config = config;
    state.initialized = true;
}

void Logging::shutdown() noexcept {
    auto& state = detail::runtime();
    std::lock_guard lock(state.mutex);
    if (state.backend) state.backend->flush();
    state.backend.reset();
    state.initialized = false;
}
void Logging::flush() noexcept {
    auto& state = detail::runtime();
    std::lock_guard lock(state.mutex);
    if (state.backend) state.backend->flush();
}
Logger Logging::getLogger(std::string name) { return Logger{std::move(name)}; }
bool Logging::isInitialized() noexcept {
    auto& state = detail::runtime();
    std::lock_guard lock(state.mutex);
    return state.initialized;
}
} // namespace suchi

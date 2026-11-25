#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "suchi/Suchi.hpp"

namespace {
int failures = 0;
#define CHECK(condition)                                                 \
    do {                                                                 \
        if (!(condition)) {                                              \
            std::cerr << "FAIL: " #condition " at " << __LINE__ << '\n'; \
            ++failures;                                                  \
        }                                                                \
    } while (false)
std::vector<suchi::LogRecord> records;
class MemoryBackend final : public suchi::ILoggingBackend {
public:
    suchi::BackendDescriptor descriptor() const override { return {"memory", "Test backend"}; }
    void write(const suchi::LogRecord& record) noexcept override { records.push_back(record); }
    void flush() noexcept override {}
};
std::unique_ptr<suchi::ILoggingBackend> makeMemory(const suchi::LoggingConfig&) {
    return std::make_unique<MemoryBackend>();
}
} // namespace

int main() {
    using namespace suchi;
    CHECK(toString(LogLevel::Info) == "INFO");
    CHECK(LoggingConfig::defaults().backend == "console");

    const auto configPath = std::filesystem::temp_directory_path() / "suchi-test.conf";
    {
        std::ofstream out(configPath);
        out << "level=debug\nbackend=memory\ntimestamp=false\nthread_id=true\nbackend.buffer="
               "4096\n";
    }
    const auto parsed = LoggingConfig::fromFile(configPath);
    CHECK(parsed.minimumLevel == LogLevel::Debug);
    CHECK(parsed.backend == "memory");
    CHECK(!parsed.includeTimestamp);
    CHECK(parsed.includeThreadId);
    CHECK(parsed.backendOptions.at("buffer") == "4096");
    std::filesystem::remove(configPath);

    Logging::registerBackend("memory", &makeMemory);
    LoggingConfig config;
    config.backend = "memory";
    config.minimumLevel = LogLevel::Debug;
    Logging::initialize(config);
    auto logger = Logging::getLogger("Suchi.Tests").withContext({field("component", "unit-test")});
    logger.debug("debug event", {field("attempt", 3), field("success", true)});
    logger.info("ready");
    logger.trace("filtered event");
    CHECK(records.size() == 2);
    CHECK(records.at(0).loggerName == "Suchi.Tests");
    CHECK(records.at(0).message == "debug event");
    CHECK(records.at(0).context.size() == 1);
    CHECK(records.at(0).fields.size() == 2);
    CHECK(records.at(0).level == LogLevel::Debug);
    Logging::flush();
    Logging::shutdown();
    CHECK(!Logging::isInitialized());

    try {
        LoggingConfig invalid;
        invalid.backend = "does-not-exist";
        Logging::initialize(invalid);
        CHECK(false);
    } catch (const std::runtime_error&) {
        CHECK(true);
    }

    if (failures != 0)
        return EXIT_FAILURE;
    std::cout << "All Suchi tests passed\n";
    return EXIT_SUCCESS;
}

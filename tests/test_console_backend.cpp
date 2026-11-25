#include <iostream>
#include <sstream>

#include "suchi/Suchi.hpp"
#include "test_harness.hpp"

SUCHI_TEST(console_backend_emits_level_logger_message_and_fields) {
    std::ostringstream captured;
    auto* previous = std::cerr.rdbuf(captured.rdbuf());
    suchi::LoggingConfig c;
    c.includeTimestamp = false;
    c.colorize = false;
    suchi::Logging::initialize(c);
    suchi::Logging::getLogger("console.test")
        .warning("careful", {suchi::field("attempt", 2), suchi::field("ok", true)});
    suchi::Logging::shutdown();
    std::cerr.rdbuf(previous);
    const auto output = captured.str();
    SUCHI_CHECK(output.find("[WARN]") != std::string::npos);
    SUCHI_CHECK(output.find("[console.test]") != std::string::npos);
    SUCHI_CHECK(output.find("careful") != std::string::npos);
    SUCHI_CHECK(output.find("attempt=2") != std::string::npos);
    SUCHI_CHECK(output.find("ok=true") != std::string::npos);
}
SUCHI_TEST(console_backend_respects_timestamp_option) {
    std::ostringstream captured;
    auto* previous = std::cerr.rdbuf(captured.rdbuf());
    suchi::LoggingConfig c;
    c.includeTimestamp = false;
    suchi::Logging::initialize(c);
    suchi::Logging::getLogger("no-time").info("message");
    suchi::Logging::shutdown();
    std::cerr.rdbuf(previous);
    SUCHI_CHECK(captured.str().find("1970-") == std::string::npos);
    SUCHI_CHECK(captured.str().find("[INFO]") == 0);
}

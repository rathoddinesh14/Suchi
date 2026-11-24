#include "test_harness.hpp"
#include "test_support.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace {
std::unique_ptr<suchi::ILoggingBackend> nullFactory(const suchi::LoggingConfig&) { return {}; }
}
SUCHI_TEST(logging_initialization_state_transitions) {
    suchi::Logging::shutdown(); SUCHI_CHECK(!suchi::Logging::isInitialized());
    test_support::initializeMemory(); SUCHI_CHECK(suchi::Logging::isInitialized());
    suchi::Logging::shutdown(); SUCHI_CHECK(!suchi::Logging::isInitialized());
    suchi::Logging::shutdown(); SUCHI_CHECK(!suchi::Logging::isInitialized());
}
SUCHI_TEST(logging_rejects_empty_backend_name_and_null_factory) {
    SUCHI_CHECK_THROWS(suchi::Logging::registerBackend("", &test_support::makeMemoryBackend), std::invalid_argument);
    SUCHI_CHECK_THROWS(suchi::Logging::registerBackend("null-factory", nullptr), std::invalid_argument);
}
SUCHI_TEST(logging_rejects_duplicate_backend_registration) {
    static const std::string uniqueName = "duplicate-registration-test";
    suchi::Logging::registerBackend(uniqueName, &test_support::makeMemoryBackend);
    SUCHI_CHECK_THROWS(suchi::Logging::registerBackend(uniqueName, &test_support::makeMemoryBackend), std::invalid_argument);
}
SUCHI_TEST(logging_rejects_unknown_backend_and_null_backend_instance) {
    suchi::LoggingConfig c; c.backend = "no-such-backend";
    SUCHI_CHECK_THROWS(suchi::Logging::initialize(c), std::runtime_error);
    suchi::Logging::registerBackend("null-backend-test", &nullFactory);
    c.backend = "null-backend-test";
    SUCHI_CHECK_THROWS(suchi::Logging::initialize(c), std::runtime_error);
    SUCHI_CHECK(!suchi::Logging::isInitialized());
}
SUCHI_TEST(logging_flushes_on_explicit_flush_shutdown_and_reinitialize) {
    auto recorder = test_support::resetRecorder(); test_support::initializeMemory();
    suchi::Logging::flush(); SUCHI_CHECK_EQ(recorder->flushCount.load(), 1);
    suchi::Logging::shutdown(); SUCHI_CHECK_EQ(recorder->flushCount.load(), 2);
    test_support::initializeMemory();
    suchi::LoggingConfig c; c.backend = "test-memory"; suchi::Logging::initialize(c);
    SUCHI_CHECK_EQ(recorder->flushCount.load(), 3); // flush of prior active backend during replacement
    suchi::Logging::shutdown();
}

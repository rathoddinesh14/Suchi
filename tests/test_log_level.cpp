#include "suchi/LogLevel.hpp"
#include "test_harness.hpp"

SUCHI_TEST(log_level_names_are_stable) {
    SUCHI_CHECK_EQ(suchi::toString(suchi::LogLevel::Trace), std::string_view{"TRACE"});
    SUCHI_CHECK_EQ(suchi::toString(suchi::LogLevel::Debug), std::string_view{"DEBUG"});
    SUCHI_CHECK_EQ(suchi::toString(suchi::LogLevel::Info), std::string_view{"INFO"});
    SUCHI_CHECK_EQ(suchi::toString(suchi::LogLevel::Warning), std::string_view{"WARN"});
    SUCHI_CHECK_EQ(suchi::toString(suchi::LogLevel::Error), std::string_view{"ERROR"});
    SUCHI_CHECK_EQ(suchi::toString(suchi::LogLevel::Critical), std::string_view{"CRITICAL"});
    SUCHI_CHECK_EQ(suchi::toString(suchi::LogLevel::Off), std::string_view{"OFF"});
}
SUCHI_TEST(invalid_log_level_has_fallback_name) {
    SUCHI_CHECK_EQ(suchi::toString(static_cast<suchi::LogLevel>(255)), std::string_view{"UNKNOWN"});
}

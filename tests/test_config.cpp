#include "test_harness.hpp"
#include "suchi/LoggingConfig.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace {
struct TempConfig {
    std::filesystem::path path = std::filesystem::temp_directory_path() / "suchi-config-tests.conf";
    ~TempConfig() { std::error_code ec; std::filesystem::remove(path, ec); }
    void write(const std::string& contents) { std::ofstream out(path); out << contents; }
};
}
SUCHI_TEST(config_defaults_are_documented) {
    const auto c = suchi::LoggingConfig::defaults();
    SUCHI_CHECK_EQ(c.minimumLevel, suchi::LogLevel::Info);
    SUCHI_CHECK_EQ(c.backend, std::string{"console"});
    SUCHI_CHECK(c.includeTimestamp);
    SUCHI_CHECK(!c.includeThreadId);
    SUCHI_CHECK(!c.includeSourceLocation);
}
SUCHI_TEST(config_parses_every_supported_scalar_setting) {
    TempConfig f; f.write("# comment\n; another comment\nlevel=debug\nbackend=memory\ntimestamp=no\nthread_id=1\nsource_location=yes\ncolor=0\noutput_file=/tmp/suchi.log\nbackend.buffer_size=8192\n");
    const auto c = suchi::LoggingConfig::fromFile(f.path);
    SUCHI_CHECK_EQ(c.minimumLevel, suchi::LogLevel::Debug);
    SUCHI_CHECK_EQ(c.backend, std::string{"memory"});
    SUCHI_CHECK(!c.includeTimestamp); SUCHI_CHECK(c.includeThreadId);
    SUCHI_CHECK(c.includeSourceLocation); SUCHI_CHECK(!c.colorize);
    SUCHI_CHECK_EQ(c.outputFile.string(), std::string{"/tmp/suchi.log"});
    SUCHI_CHECK_EQ(c.backendOptions.at("buffer_size"), std::string{"8192"});
}
SUCHI_TEST(config_accepts_all_severity_spellings_case_insensitively) {
    TempConfig f;
    const std::pair<const char*, suchi::LogLevel> cases[] = {
        {"trace", suchi::LogLevel::Trace}, {"DEBUG", suchi::LogLevel::Debug},
        {"Info", suchi::LogLevel::Info}, {"warn", suchi::LogLevel::Warning},
        {"WARNING", suchi::LogLevel::Warning}, {"error", suchi::LogLevel::Error},
        {"critical", suchi::LogLevel::Critical}, {"off", suchi::LogLevel::Off}};
    for (const auto& item : cases) { f.write(std::string{"level="} + item.first); SUCHI_CHECK_EQ(suchi::LoggingConfig::fromFile(f.path).minimumLevel, item.second); }
}
SUCHI_TEST(config_rejects_missing_file) {
    SUCHI_CHECK_THROWS(suchi::LoggingConfig::fromFile("/definitely/not/a/suchi/config"), std::runtime_error);
}
SUCHI_TEST(config_rejects_malformed_line) {
    TempConfig f; f.write("level=info\nthis-is-not-a-key-value-pair\n");
    SUCHI_CHECK_THROWS(suchi::LoggingConfig::fromFile(f.path), std::runtime_error);
}
SUCHI_TEST(config_rejects_unknown_key) {
    TempConfig f; f.write("mystery=true\n"); SUCHI_CHECK_THROWS(suchi::LoggingConfig::fromFile(f.path), std::runtime_error);
}
SUCHI_TEST(config_rejects_invalid_level_and_boolean) {
    TempConfig f; f.write("level=verbose\n"); SUCHI_CHECK_THROWS(suchi::LoggingConfig::fromFile(f.path), std::runtime_error);
    f.write("timestamp=perhaps\n"); SUCHI_CHECK_THROWS(suchi::LoggingConfig::fromFile(f.path), std::runtime_error);
}
SUCHI_TEST(config_trims_whitespace_and_empty_values) {
    TempConfig f; f.write("  backend = console  \nbackend.custom =   \n");
    const auto c = suchi::LoggingConfig::fromFile(f.path);
    SUCHI_CHECK_EQ(c.backend, std::string{"console"});
    SUCHI_CHECK_EQ(c.backendOptions.at("custom"), std::string{});
}

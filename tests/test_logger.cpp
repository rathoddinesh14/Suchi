#include <algorithm>
#include <thread>
#include <vector>

#include "test_harness.hpp"
#include "test_support.hpp"

SUCHI_TEST(logger_preserves_name_message_severity_fields_and_source) {
    auto recorder = test_support::resetRecorder();
    test_support::initializeMemory();
    auto logger = suchi::Logging::getLogger("unit.logger");
    logger.info("hello", {suchi::field("count", 7)});
    {
        std::lock_guard<std::mutex> lock(recorder->mutex);
        SUCHI_CHECK_EQ(recorder->records.size(), std::size_t{1});
        const auto& r = recorder->records.front();
        SUCHI_CHECK_EQ(r.loggerName, std::string{"unit.logger"});
        SUCHI_CHECK_EQ(r.message, std::string{"hello"});
        SUCHI_CHECK_EQ(r.level, suchi::LogLevel::Info);
        SUCHI_CHECK_EQ(r.fields.size(), std::size_t{1});
        SUCHI_CHECK(!r.source.file.empty());
        SUCHI_CHECK(r.source.line > 0);
    }
    suchi::Logging::shutdown();
}
SUCHI_TEST(logger_context_is_immutable_and_composes) {
    auto recorder = test_support::resetRecorder();
    test_support::initializeMemory();
    auto base = suchi::Logging::getLogger("ctx").withContext({suchi::field("request", "r1")});
    auto child = base.withContext({suchi::field("component", "reader")});
    base.info("base");
    child.info("child");
    {
        std::lock_guard<std::mutex> lock(recorder->mutex);
        SUCHI_CHECK_EQ(recorder->records.size(), std::size_t{2});
        SUCHI_CHECK_EQ(recorder->records[0].context.size(), std::size_t{1});
        SUCHI_CHECK_EQ(recorder->records[1].context.size(), std::size_t{2});
        SUCHI_CHECK_EQ(recorder->records[1].context[0].key, std::string{"request"});
        SUCHI_CHECK_EQ(recorder->records[1].context[1].key, std::string{"component"});
    }
    suchi::Logging::shutdown();
}
SUCHI_TEST(logger_filters_below_threshold_and_off_filters_everything) {
    auto recorder = test_support::resetRecorder();
    test_support::initializeMemory(suchi::LogLevel::Warning);
    auto logger = suchi::Logging::getLogger("filter");
    logger.debug("no");
    logger.info("no");
    logger.warning("yes");
    logger.error("yes");
    suchi::Logging::shutdown();
    SUCHI_CHECK_EQ(recorder->records.size(), std::size_t{2});
    SUCHI_CHECK_EQ(recorder->records[0].level, suchi::LogLevel::Warning);
    recorder = test_support::resetRecorder();
    test_support::initializeMemory(suchi::LogLevel::Off);
    suchi::Logging::getLogger("off").critical("hidden");
    suchi::Logging::shutdown();
    SUCHI_CHECK(recorder->records.empty());
}
SUCHI_TEST(logger_calls_before_initialization_and_after_shutdown_are_safe) {
    suchi::Logging::shutdown();
    auto logger = suchi::Logging::getLogger("inactive");
    logger.info("ignored before init");
    test_support::initializeMemory();
    suchi::Logging::shutdown();
    logger.error("ignored after shutdown");
}
SUCHI_TEST(logger_handles_empty_names_messages_and_fields) {
    auto recorder = test_support::resetRecorder();
    test_support::initializeMemory();
    suchi::Logging::getLogger("").info("");
    suchi::Logging::shutdown();
    SUCHI_CHECK_EQ(recorder->records.size(), std::size_t{1});
    SUCHI_CHECK(recorder->records.front().loggerName.empty());
    SUCHI_CHECK(recorder->records.front().message.empty());
    SUCHI_CHECK(recorder->records.front().fields.empty());
}
SUCHI_TEST(logger_concurrent_writes_are_not_lost) {
    auto recorder = test_support::resetRecorder();
    test_support::initializeMemory();
    constexpr int threadCount = 6;
    constexpr int perThread = 150;
    std::vector<std::thread> workers;
    for (int t = 0; t < threadCount; ++t)
        workers.emplace_back([t] {
            auto logger = suchi::Logging::getLogger("concurrent");
            for (int i = 0; i < perThread; ++i)
                logger.info("event", {suchi::field("thread", t), suchi::field("index", i)});
        });
    for (auto& worker : workers)
        worker.join();
    suchi::Logging::shutdown();
    SUCHI_CHECK_EQ(recorder->records.size(), static_cast<std::size_t>(threadCount * perThread));
}

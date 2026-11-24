#pragma once

#include "suchi/Suchi.hpp"

#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace test_support {

struct Recorder {
    std::mutex mutex;
    std::vector<suchi::LogRecord> records;
    std::atomic<int> flushCount{0};
};

inline std::shared_ptr<Recorder>& recorderSlot() {
    static std::shared_ptr<Recorder> recorder = std::make_shared<Recorder>();
    return recorder;
}

class MemoryBackend final : public suchi::ILoggingBackend {
public:
    explicit MemoryBackend(std::shared_ptr<Recorder> recorder) : recorder_(std::move(recorder)) {}
    suchi::BackendDescriptor descriptor() const override { return {"test-memory", "In-memory test sink"}; }
    void write(const suchi::LogRecord& record) noexcept override {
        try {
            std::lock_guard<std::mutex> lock(recorder_->mutex);
            recorder_->records.push_back(record);
        } catch (...) {
            // The production contract is noexcept; the test records no allocation failure.
        }
    }
    void flush() noexcept override { ++recorder_->flushCount; }
private:
    std::shared_ptr<Recorder> recorder_;
};

inline std::unique_ptr<suchi::ILoggingBackend> makeMemoryBackend(const suchi::LoggingConfig&) {
    return std::make_unique<MemoryBackend>(recorderSlot());
}

inline void registerMemoryBackend() {
    static const bool registered = [] {
        suchi::Logging::registerBackend("test-memory", &makeMemoryBackend);
        return true;
    }();
    (void)registered;
}

inline std::shared_ptr<Recorder> resetRecorder() {
    auto recorder = std::make_shared<Recorder>();
    recorderSlot() = recorder;
    return recorder;
}

inline void initializeMemory(suchi::LogLevel level = suchi::LogLevel::Trace) {
    registerMemoryBackend();
    suchi::LoggingConfig config;
    config.backend = "test-memory";
    config.minimumLevel = level;
    suchi::Logging::initialize(config);
}

} // namespace test_support

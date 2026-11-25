#include <iostream>

#include "suchi/Suchi.hpp"

int main() {
    try {
        auto config = suchi::LoggingConfig::defaults();
        config.minimumLevel = suchi::LogLevel::Debug;
        config.includeSourceLocation = true;
        suchi::Logging::initialize(config);

        auto logger = suchi::Logging::getLogger("Example.Application");
        logger.info("Application started", {suchi::field("version", "0.1.0")});
        auto operation = logger.withContext({suchi::field("operation", "load-volume")});
        operation.debug("Loading volume", {suchi::field("dimensions", "512x512x180")});
        operation.warning("Using default configuration");
        suchi::Logging::flush();
        suchi::Logging::shutdown();
        return 0;
    } catch (const std::exception& error) {
        // Emergency path: do not report logging initialization failures through Suchi.
        std::cerr << "Application startup failed: " << error.what() << '\n';
        return 1;
    }
}

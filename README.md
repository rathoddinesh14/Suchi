# Suchi

**Suchi** is a backend-independent C++20 logging library. Its core uses only the C++ standard library and exposes a stable application-facing API. Backend implementations are registered behind a Suchi-owned interface; third-party adapters and a versioned C plugin ABI are planned as optional modules.

## Current status

The initial implementation provides:

- C++20 core with no third-party dependencies.
- Named loggers, severity levels, structured fields, and contextual fields.
- Built-in console backend.
- Runtime registration and selection of compiled backend factories.
- Strict `key=value` configuration file loading with defaults and validation.
- Safe logging calls that do not propagate exceptions into application code.
- CMake install/export package and consumer target `Suchi::Core`.
- Unit tests, example, and cross-platform GitHub Actions workflow.

Dynamic plugin loading, async logging, file rotation, and third-party adapters are separate follow-up milestones and are not claimed as implemented in this release.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Basic usage

```cpp
#include <suchi/Suchi.hpp>

int main() {
    suchi::LoggingConfig config;
    config.minimumLevel = suchi::LogLevel::Debug;
    suchi::Logging::initialize(config);

    auto logger = suchi::Logging::getLogger("CTMRI.Renderer");
    logger.info("Renderer initialized", {
        suchi::field("api", "OpenGL"),
        suchi::field("major_version", 4)
    });

    auto registration = logger.withContext({
        suchi::field("operation", "image-registration")
    });
    registration.warning("Using fallback interpolation");

    suchi::Logging::shutdown();
}
```

## Configuration file

Suchi accepts a deliberately small, dependency-free `key=value` format:

```ini
# suchi.conf
level=info
backend=console
timestamp=true
thread_id=false
source_location=false
color=true
# backend-specific values are preserved for the selected adapter
backend.buffer_size=8192
```

Load it with `LoggingConfig::fromFile("suchi.conf")`, then pass the result to `Logging::initialize(config)`. Unknown keys, malformed lines, invalid booleans, and unsupported levels are rejected with an exception. Applications should catch initialization errors and use an independent emergency reporting path.

## Backend registration

A compiled backend implements `suchi::ILoggingBackend` and supplies a factory:

```cpp
std::unique_ptr<suchi::ILoggingBackend>
makeMyBackend(const suchi::LoggingConfig& config);

suchi::Logging::registerBackend("my-backend", &makeMyBackend);
auto config = suchi::LoggingConfig::defaults();
config.backend = "my-backend";
suchi::Logging::initialize(config);
```

Register factories before initializing the runtime. The first release intentionally keeps the backend interface in-process; independently loaded plugins will use a versioned C ABI rather than crossing a shared-library boundary with C++ virtual objects.

## CMake integration

```cmake
FetchContent_Declare(
    Suchi
    GIT_REPOSITORY https://github.com/<owner>/Suchi.git
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(Suchi)
target_link_libraries(my_app PRIVATE Suchi::Core)
```

For installation, build and run `cmake --install build --prefix <prefix>`, then use `find_package(Suchi CONFIG REQUIRED)` and link `Suchi::Core`.

## Privacy guidance

Suchi does not inspect application objects or automatically capture patient data. Applications must not pass patient identifiers, DICOM metadata, image content, credentials, or other sensitive values as log fields. Redaction policies and domain-specific privacy helpers will be considered in a later milestone.

## Roadmap

1. Core API and console backend — initial implementation.
2. Backend capability model and first third-party adapter (spdlog).
3. Configuration extensions and explicit fallback policy.
4. Versioned C ABI and dynamic plugin loader.
5. Async dispatch, file sinks/rotation, redaction, and performance testing.
6. Integration into CTMRI-Workstation, Rupa, and TerraForge.

## License

MIT. See [LICENSE](LICENSE).

## Test-driven development

Tests are organized by behavior under `tests/`, implemented with Catch2, and discovered by CTest. When tests are enabled, CMake uses an installed Catch2 3 package or fetches Catch2 v3.8.1. New behavior and bug fixes must follow the Red → Green → Refactor cycle: add a failing test first, implement the smallest passing change, refactor, then run the complete suite.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DSUCHI_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For a GCC/Clang coverage build (requires `gcovr`):

```bash
cmake -S . -B build-coverage \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSUCHI_BUILD_TESTS=ON \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage --parallel
ctest --test-dir build-coverage --output-on-failure
gcovr --root . --filter 'src/.*' --filter 'include/.*' \
  --exclude 'tests/.*' --print-summary
```

The test suite uses an in-memory backend for deterministic assertions and does not require external logging libraries. Keep test names behavior-oriented and add regression cases for every defect found.

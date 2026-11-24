cmake_minimum_required(VERSION 3.20)
project(Suchi VERSION 0.1.0 LANGUAGES CXX)

find_package(Threads REQUIRED)

option(SUCHI_BUILD_TESTS "Build Suchi tests" ON)
option(SUCHI_BUILD_EXAMPLES "Build Suchi examples" ON)
option(SUCHI_BUILD_CONSOLE "Build the built-in console backend" ON)

add_library(suchi_core
    src/Logger.cpp
    src/Logging.cpp
    src/Config.cpp
)
add_library(Suchi::Core ALIAS suchi_core)
target_compile_features(suchi_core PUBLIC cxx_std_20)
target_include_directories(suchi_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
set_target_properties(suchi_core PROPERTIES EXPORT_NAME Core)

if(SUCHI_BUILD_CONSOLE)
    target_sources(suchi_core PRIVATE src/ConsoleBackend.cpp)
    target_compile_definitions(suchi_core PRIVATE SUCHI_HAS_CONSOLE_BACKEND=1)
endif()

if(MSVC)
    target_compile_options(suchi_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(suchi_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(SUCHI_BUILD_EXAMPLES)
    add_executable(suchi_basic examples/basic.cpp)
    target_link_libraries(suchi_basic PRIVATE Suchi::Core)
endif()

if(SUCHI_BUILD_TESTS)
    enable_testing()
    find_package(Catch2 3 QUIET)
    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.8.1
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(Catch2)
    endif()

    add_executable(suchi_tests
        tests/test_log_level.cpp
        tests/test_fields.cpp
        tests/test_config.cpp
        tests/test_logger.cpp
        tests/test_backend_lifecycle.cpp
        tests/test_console_backend.cpp
    )
    target_include_directories(suchi_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    target_link_libraries(suchi_tests PRIVATE Suchi::Core Catch2::Catch2WithMain Threads::Threads)
    include(Catch)
    catch_discover_tests(suchi_tests)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS suchi_core EXPORT SuchiTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT SuchiTargets FILE SuchiTargets.cmake NAMESPACE Suchi:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Suchi)
configure_package_config_file(cmake/SuchiConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuchiConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Suchi
)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/SuchiConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SuchiConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuchiConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Suchi
)

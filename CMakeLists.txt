cmake_minimum_required(VERSION 3.20)
project(Suchi VERSION 0.1.0 LANGUAGES CXX)

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
    add_executable(suchi_tests tests/test_suchi.cpp)
    target_link_libraries(suchi_tests PRIVATE Suchi::Core)
    add_test(NAME suchi.unit COMMAND suchi_tests)
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

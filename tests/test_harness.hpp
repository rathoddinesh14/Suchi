#pragma once
#include <catch2/catch_test_macros.hpp>

#define SUCHI_TEST(name) TEST_CASE(#name)
#define SUCHI_CHECK(expression) CHECK(expression)
#define SUCHI_CHECK_EQ(actual, expected) CHECK((actual) == (expected))
#define SUCHI_CHECK_THROWS(expression, exception_type) CHECK_THROWS_AS(expression, exception_type)

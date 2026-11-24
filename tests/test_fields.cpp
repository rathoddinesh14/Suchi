#include "test_harness.hpp"
#include "suchi/LogField.hpp"
#include <cstdint>
#include <type_traits>

SUCHI_TEST(field_factory_preserves_scalar_types) {
    using suchi::FieldValue;
    SUCHI_CHECK(std::holds_alternative<bool>(suchi::field("enabled", true).value));
    SUCHI_CHECK(std::holds_alternative<std::int64_t>(suchi::field("count", -3).value));
    SUCHI_CHECK(std::holds_alternative<std::uint64_t>(suchi::field("size", 3u).value));
    SUCHI_CHECK(std::holds_alternative<double>(suchi::field("ratio", 0.25).value));
    SUCHI_CHECK(std::holds_alternative<std::string>(suchi::field("name", "Suchi").value));
}
SUCHI_TEST(field_factory_moves_string_values) {
    std::string value = "payload";
    auto result = suchi::field("message", std::move(value));
    SUCHI_CHECK_EQ(std::get<std::string>(result.value), std::string{"payload"});
    SUCHI_CHECK(value.empty());
}

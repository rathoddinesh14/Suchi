#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <utility>
#include <vector>

namespace suchi {

using FieldValue = std::variant<bool, std::int64_t, std::uint64_t, double, std::string>;

struct LogField {
    std::string key;
    FieldValue value;
};

using LogFields = std::vector<LogField>;

inline LogField field(std::string key, std::string value) {
    return {std::move(key), FieldValue{std::move(value)}};
}
inline LogField field(std::string key, const char* value) {
    return field(std::move(key), std::string{value});
}
inline LogField field(std::string key, bool value) {
    return {std::move(key), FieldValue{value}};
}
inline LogField field(std::string key, int value) {
    return {std::move(key), FieldValue{static_cast<std::int64_t>(value)}};
}
inline LogField field(std::string key, unsigned value) {
    return {std::move(key), FieldValue{static_cast<std::uint64_t>(value)}};
}
inline LogField field(std::string key, std::int64_t value) {
    return {std::move(key), FieldValue{value}};
}
inline LogField field(std::string key, std::uint64_t value) {
    return {std::move(key), FieldValue{value}};
}
inline LogField field(std::string key, double value) {
    return {std::move(key), FieldValue{value}};
}

} // namespace suchi

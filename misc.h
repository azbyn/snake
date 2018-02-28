#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>

#define CONCATENATE(s1, s2) s1##s2

#ifdef __COUNTER__
#define ANONYMUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
#define ANONYMUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

#define LEN(x) (sizeof(x) / sizeof(*x))

namespace azbyn {
template <class... Args>
std::string string_format(const std::string& format, Args... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
template <class V, class... T>
constexpr auto array_of(T&&... t) -> std::array<V, sizeof...(T)> {
    return {{std::forward<T>(t)...}};
}
using Callback = void (*)(void);

} // namespace azbyn


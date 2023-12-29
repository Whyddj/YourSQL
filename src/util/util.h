// util.h
#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cstring>

namespace util {

template <typename T>
void serializeIntegral(std::vector<char>& buffer, T value) {
    const char* bytes = reinterpret_cast<const char*>(&value);
    buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
}

template <typename T>
T deserializeIntegral(const std::vector<char>& buffer, size_t& pos) {
    T value;
    std::memcpy(&value, buffer.data() + pos, sizeof(T));
    pos += sizeof(T);
    return value;
}

// 用于打印调试信息
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string RESET = "\033[0m";

enum LogLevel {
    DEBUG,
    INFO,
    ERROR
};

// 用于终止递归的函数
void logInternal(std::ostream& os) {
    os << RESET << std::endl;
}

template <typename T, typename... Args>
void logInternal(std::ostream& os, const T& value, const Args&... args) {
    os << value;
    logInternal(os, args...);
}

template <typename... Args>
void log(LogLevel level, Args... args) {
    std::ostream& os = (level == ERROR) ? std::cerr : std::cout;

    switch (level) {
        case DEBUG:
            os << GREEN << "[DEBUG]: ";
            break;
        case INFO:
            os << YELLOW << "[INFO]: ";
            break;
        case ERROR:
            os << RED << "[ERROR]: ";
            break;
    }

    logInternal(os, args...);
}

} // namespace util

#endif // UTIL_H
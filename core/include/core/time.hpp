#include <iostream>
#include <chrono>


namespace core::time {

class Time {
public:
    static inline std::chrono::_V2::system_clock::time_point now() {
        return std::chrono::system_clock::now();
    }

    static inline uint32_t now_second() {
        return std::chrono::duration_cast<std::chrono::seconds>(Time::now().time_since_epoch()).count();
    }

    static inline uint32_t now_millisecond() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Time::now().time_since_epoch()).count();
    }

    static inline uint32_t now_nanosecond() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(Time::now().time_since_epoch()).count();
    }

    template<int second>
    constexpr static uint32_t second_2_nanosecond() {
        return static_cast<uint32_t>(second * 1'000'000'000);
    }

};

} // namespace core::time

#include <iostream>
#include <ctime>
#include <chrono>
/**
 * Document of chrono: https://en.cppreference.com/w/cpp/chrono
 * g++ 11.4.0 cannot support utc_clock (2024年1月27日)
*/


namespace core::time {

class Time {
public:
    Time() {
        _time_point = std::chrono::system_clock::now();
        _time = _time_point.time_since_epoch();
        std::time_t tt = std::chrono::system_clock::to_time_t(_time_point);
        _time_member = std::localtime(&tt);
    }

    Time(int year, int month, int day, int hour, int minute, int second) {
        _time_member = new std::tm;
        _time_member->tm_year = year - 1900;
        _time_member->tm_mon = month - 1;
        _time_member->tm_mday = day;
        _time_member->tm_hour = hour;
        _time_member->tm_min = minute;
        _time_member->tm_sec = second;

        std::time_t _t = std::mktime(_time_member);
        _time_point = std::chrono::system_clock::from_time_t(_t);
        _time = _time_point.time_since_epoch();
    }

public:
    Time set_timezoom(int offset) {
        _offset = offset;
        return *this;
    }
    int timezoom() { return _offset; }

public:
    uint64_t year() { return _time_member->tm_year + 1900; }
    uint64_t month() { return _time_member->tm_mon + 1; }
    uint64_t day() { return _time_member->tm_mday; }
    uint64_t hour() { return _time_member->tm_hour; }    
    uint64_t minute() { return _time_member->tm_min; }
    uint64_t second() { return _time_member->tm_sec; }

    uint64_t to_second() { return std::chrono::duration_cast<std::chrono::seconds>(_time).count(); }
    uint64_t to_milliseconds() { return std::chrono::duration_cast<std::chrono::milliseconds>(_time).count(); }
    uint64_t to_nanoseconds() { return _time.count(); }

    std::string to_string() {
        char time_format[25];
        if (_offset == 0) {
            strftime(time_format, sizeof(time_format), "%Y-%m-%dT%H:%M:%SZ", _time_member);
        } else {
            uint off = 0;
            if (_offset >= 0) {
                strftime(time_format, sizeof(time_format), "%Y-%m-%dT%H:%M:%S+", _time_member);
                off = _offset;
            } else {
                strftime(time_format, sizeof(time_format), "%Y-%m-%dT%H:%M:%S-", _time_member);
                off = _offset * -1;
            }

            if (off > 10) { time_format[20] = '1'; }
            else { time_format[20] = '0'; }

            time_format[21] = '0' + (off % 10);
            time_format[22] = '0';
            time_format[23] = '0';
            time_format[24] = '\0';
        }
        return std::string(time_format);
    }

private:
    std::chrono::_V2::system_clock::time_point _time_point;
    std::chrono::_V2::system_clock::duration _time;
    std::tm *_time_member = nullptr;
    int _offset = 0;
};

} // namespace core::time

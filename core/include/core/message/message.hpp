#ifndef CORE_MESSAGE
#define CORE_MESSAGE

#include "mmap.hpp"
#include <typeinfo>
#include "core/datas.hpp"


#define LOGHEAD "[message::" + std::string(__func__) + "]"

constexpr int KB = 1024;
constexpr int MB = 1024 * KB;
constexpr int GB = 1024 * MB;


namespace {
using namespace core::message::sharememory;

struct alignas(64) Header {
    size_t data_size = 0;
    // front address of data
    uintptr_t data_front_address = 0;
    // tail address of data
    uintptr_t data_tail_address = 0;
    // the earliest data's address
    uintptr_t data_earliest_addresss = 0;
    // the lastest data's address
    uintptr_t data_lastest_address = 0;
    // for trade; check if the data can be remove
    uintptr_t data_cursor_address = 0;
    // the end of the lastest data
    uintptr_t data_next_address = 0;
};

struct alignas(64) MarketDataHeader {
    core::datas::MarketType type;
    size_t data_size = 0;
};

constexpr size_t HeaderSize = static_cast<size_t>(sizeof(Header));
constexpr size_t MarketDataHeaderSize = static_cast<size_t>(sizeof(MarketDataHeader));

class Channel {
public:
    Channel(const std::string& path, size_t size, bool is_writting) : path(path), size(size), is_writting(is_writting) {
        address = load(std::move(path), size, is_writting);

        if (address < 0) {
            throw std::runtime_error(LOGHEAD + " unable to load buffer!");
        }

        header = reinterpret_cast<Header *>(address);
        header->data_size = size - HeaderSize;
        header->data_front_address = address + HeaderSize;
        header->data_tail_address = address + size;
        header->data_earliest_addresss = header->data_front_address;
        header->data_lastest_address = header->data_front_address;
        header->data_cursor_address = header->data_front_address;
        header->data_next_address = header->data_front_address;
    }

    virtual ~Channel() {
        release(address, size);
    }

protected:
    const std::string& path;
    size_t size;
    bool is_writting;

    uintptr_t address = -1;
    Header* header;
};
}


namespace core::message::message {

class MarketChannel: protected Channel {
public:
    MarketChannel(const std::string& path, size_t size, bool is_writting) : Channel(path, size, is_writting) {

    }
    virtual ~MarketChannel() = default;

public:
    template<typename T>
    bool write(T &value) {
        return insert(value);
    }

    core::datas::Market_base* read(uintptr_t target__address) {
        MarketDataHeader *_header = reinterpret_cast<MarketDataHeader*>(target__address);
        if (_header == nullptr) { return nullptr; }

        if (_header->type == core::datas::MarketType::Bbo && _header->data_size == sizeof(core::datas::Market_bbo)) {
            return reinterpret_cast<core::datas::Market_bbo*>(target__address + MarketDataHeaderSize);
        } else if (_header->type == core::datas::MarketType::Depth && _header->data_size == sizeof(core::datas::Market_depth)) {
            return reinterpret_cast<core::datas::Market_depth*>(target__address + MarketDataHeaderSize);
        } else if (_header->type == core::datas::MarketType::Kline && _header->data_size == sizeof(core::datas::Market_kline)) {
            return reinterpret_cast<core::datas::Market_kline*>(target__address + MarketDataHeaderSize);
        }

        return nullptr;
    }

    core::datas::Market_base* read_next(uintptr_t target__address) {
        auto result = read(target__address);
        if (result) { return result; }

        return read(header->data_lastest_address);
    }

private:
    template<typename T>
    bool insert(T &value) {
        // lock

        int _market_size = 0;
        core::datas::MarketType _type = core::datas::MarketType::Unknow;
        if (std::is_same<T, core::datas::Market_bbo>::value) {
            _market_size = sizeof(core::datas::Market_bbo);
            _type = core::datas::MarketType::Bbo;
        } else if (std::is_same<T, core::datas::Market_depth>::value) {
            _market_size = sizeof(core::datas::Market_depth);
            _type = core::datas::MarketType::Depth;
        } else if (std::is_same<T, core::datas::Market_kline>::value) {
            _market_size = sizeof(core::datas::Market_kline);
            _type = core::datas::MarketType::Kline;
        } else {
            spdlog::error("{} unsupport type", LOGHEAD);
            return false;
        }

        int length = _market_size + MarketDataHeaderSize;
        if (!apply(length)) {
            // cover
            header->data_next_address = address + HeaderSize;

            if (!apply(length)) { return false; }
        }

        // data_header
        header->data_lastest_address = header->data_next_address;
        MarketDataHeader* _header = reinterpret_cast<MarketDataHeader*>(header->data_lastest_address);
        _header->data_size = _market_size;
        _header->type = _type;

        // date_body
        T* data = reinterpret_cast<T*>(header->data_lastest_address + MarketDataHeaderSize);
        std::memcpy(data, &value, _market_size);

        header->data_next_address = header->data_next_address + MarketDataHeaderSize + _market_size;
        return true;
    }

    bool apply(int length) {
        return (((header->data_next_address + length) - address) <= size);
    }

};


class OrderChannel: protected Channel {
public:
    OrderChannel(const std::string& path, size_t size) : Channel(path, size, true) {
    }
    virtual ~OrderChannel() = default;

private:
};

} // core::message::message
#undef LOGHEAD
#endif
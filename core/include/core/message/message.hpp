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
    // front displacement of data
    uint64_t data_front_displacement = 0;
    // tail displacement of data
    uint64_t data_tail_displacement = 0;
    // the earliest data's displacement
    uint64_t data_earliest_displacement = 0;
    // the lastest data's displacement
    uint64_t data_lastest_displacement = 0;
    // for trade; check if the data can be remove
    uint64_t data_cursor_displacement = 0;
    // the end of the lastest data
    uint64_t data_next_displacement = 0;
};

struct alignas(64) MarketDataHeader {
    core::datas::MarketType type;
    size_t data_size = 0;
};

struct alignas(64) CommandDataHeader {
    core::datas::CommandType type;
    size_t data_size = 0;
    core::datas::CommandStatus status = core::datas::CommandStatus::INVALID;
};

constexpr size_t HeaderSize = static_cast<size_t>(sizeof(Header));
constexpr size_t MarketDataHeaderSize = static_cast<size_t>(sizeof(MarketDataHeader));
constexpr size_t CommandDataHeaderSize = static_cast<size_t>(sizeof(CommandDataHeader));

class Channel {
public:
    Channel(const std::string& path, size_t size, bool is_writting) : path(path), size(size), is_writting(is_writting) {
        address = load(std::move(path), size, is_writting);

        if (address < 0) {
            throw std::runtime_error(LOGHEAD + " unable to load buffer!");
        }

        header = reinterpret_cast<Header *>(address);
        if (is_writting && header->data_front_displacement == 0) {
            header->data_front_displacement = HeaderSize;
            header->data_tail_displacement = size;
            header->data_earliest_displacement = HeaderSize;
            header->data_lastest_displacement = HeaderSize;
            header->data_cursor_displacement = HeaderSize;
            header->data_next_displacement = HeaderSize;
        }
    }

    virtual ~Channel() {
        release(address, size);
    }

    virtual uint64_t earliest_displacement() {
        return header ? header->data_earliest_displacement : 0;
    }

    virtual inline uintptr_t front_address() const { return address + header->data_front_displacement; }
    virtual inline uintptr_t tail_address() const { return address + header->data_tail_displacement; }
    virtual inline uintptr_t earliest_address() const { return address + header->data_earliest_displacement; }
    virtual inline uintptr_t lastest_address() const { return address + header->data_lastest_displacement; }
    virtual inline uintptr_t cursor_address() const { return address + header->data_cursor_displacement; }
    virtual inline uintptr_t next_address() const { return address + header->data_next_displacement; }
    virtual inline uintptr_t target_address(uint64_t target_displacement) const { return address + target_displacement; }

protected:
    const std::string& path;
    size_t size;
    bool is_writting;

    uintptr_t address = -1;
    Header* header;
};
}


namespace core::message::message {

class MarketChannel: public Channel {
public:
    MarketChannel(const std::string& filename, size_t size, bool is_writting=false) : Channel("/tmp/" + filename + "_market", size, is_writting) {

    }
    virtual ~MarketChannel() = default;

public:
    template<typename T>
    bool write(T &value) {
        return insert(value);
    }

    core::datas::Market_base* read(const uintptr_t target_address) const {
        MarketDataHeader *_header = reinterpret_cast<MarketDataHeader*>(target_address);
        if (_header == nullptr) { return nullptr; }

        if (_header->type == core::datas::MarketType::Bbo && _header->data_size == sizeof(core::datas::Market_bbo)) {
            return reinterpret_cast<core::datas::Market_bbo*>(target_address + MarketDataHeaderSize);
        } else if (_header->type == core::datas::MarketType::Depth && _header->data_size == sizeof(core::datas::Market_depth)) {
            return reinterpret_cast<core::datas::Market_depth*>(target_address + MarketDataHeaderSize);
        } else if (_header->type == core::datas::MarketType::Kline && _header->data_size == sizeof(core::datas::Market_kline)) {
            return reinterpret_cast<core::datas::Market_kline*>(target_address + MarketDataHeaderSize);
        }

        return nullptr;
    }

    core::datas::Market_base* read_next(uint64_t &target_displacement) const {
        if (target_displacement >= header->data_lastest_displacement) { return nullptr; }
        auto result = read(target_address(target_displacement));
        if (result) {
            target_displacement = target_displacement + CommandDataHeaderSize;
            return result;
        }

        target_displacement = header->data_lastest_displacement;
        return read(target_address(target_displacement));
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
            // header->data_next_displacement = address + HeaderSize;
            header->data_next_displacement = header->data_front_displacement;

            if (!apply(length)) { return false; }
        }

        // data_header
        header->data_lastest_displacement = header->data_next_displacement;
        MarketDataHeader* _header = reinterpret_cast<MarketDataHeader*>(lastest_address());
        _header->data_size = _market_size;
        _header->type = _type;

        // date_body
        T* data = reinterpret_cast<T*>(lastest_address() + MarketDataHeaderSize);
        std::memcpy(data, &value, _market_size);

        header->data_next_displacement = header->data_lastest_displacement + MarketDataHeaderSize + _market_size;
        return true;
    }

    bool apply(int length) {
        // return (((next_address() + length) - address) <= size);
        return ((header->data_next_displacement + length) <= header->data_tail_displacement);
    }

};


class CommandChannel: public Channel {
public:
    CommandChannel(const std::string& filename, size_t size) : Channel("/tmp/" + filename + "_command", size, true) {
    }
    virtual ~CommandChannel() = default;

public:
    template<typename T>
    bool write(T &value) {
        return insert(value);
    }

    core::datas::Command_base* read(const uintptr_t target_address) const {
        CommandDataHeader *_header = reinterpret_cast<CommandDataHeader*>(target_address);
        if (_header == nullptr) { return nullptr; }

        if (_header->type == core::datas::CommandType::SUBSCRIBE) {
            return reinterpret_cast<core::datas::SymbolObj*>(target_address + CommandDataHeaderSize);
        } else if (_header->type == core::datas::CommandType::UNSUBSCRIBE) {
            return reinterpret_cast<core::datas::SymbolObj*>(target_address + CommandDataHeaderSize);
        } else if (_header->type == core::datas::CommandType::ORDER && _header->data_size == sizeof(core::datas::OrderObj)) {
            return reinterpret_cast<core::datas::OrderObj*>(target_address + CommandDataHeaderSize);
        } else if (_header->type == core::datas::CommandType::CANCEL && _header->data_size == sizeof(core::datas::CancelObj)) {
            return reinterpret_cast<core::datas::CancelObj*>(target_address + CommandDataHeaderSize);
        }

        return nullptr;
    }

    core::datas::Command_base* read_next(uint64_t &target_displacement) const {
        if (target_displacement >= header->data_lastest_displacement) { return nullptr; }
        auto result = read(target_address(target_displacement));
        if (result) {
            target_displacement = target_displacement + CommandDataHeaderSize;
            return result;
        }

        target_displacement = header->data_lastest_displacement;
        return read(target_address(target_displacement));
    }

private:
    template<typename T>
    bool insert(T &value) {
        // lock

        int _command_size = 0;
        core::datas::CommandType _type = core::datas::CommandType::UNKNOW;
        if (std::is_same<T, core::datas::SymbolObj>::value) {
            _command_size = sizeof(core::datas::SymbolObj);
            _type = value.command_type;
        } else if (std::is_same<T, core::datas::OrderObj>::value) {
            _command_size = sizeof(core::datas::OrderObj);
            _type = core::datas::CommandType::ORDER;
        } else if (std::is_same<T, core::datas::CancelObj>::value) {
            _command_size = sizeof(core::datas::CancelObj);
            _type = core::datas::CommandType::CANCEL;
        } else {
            spdlog::error("{} unsupport type", LOGHEAD);
            return false;
        }

        int length = _command_size + CommandDataHeaderSize;
        if (!apply(length)) {
            header->data_next_displacement = header->data_front_displacement;

            if (!apply(length)) { return false; }
        }

        // data_header
        header->data_lastest_displacement = header->data_next_displacement;
        CommandDataHeader* _header = reinterpret_cast<CommandDataHeader*>(lastest_address());
        _header->data_size = _command_size;
        _header->type = _type;
        _header->status = core::datas::CommandStatus::EFFECTIVE;

        // date_body
        T* data = reinterpret_cast<T*>(lastest_address() + CommandDataHeaderSize);
        std::memcpy(data, &value, _command_size);

        header->data_next_displacement = header->data_lastest_displacement + CommandDataHeaderSize + _command_size;
        return true;
    }

    bool apply(int length) {
        if (earliest_address() <= next_address()) {
            return static_cast<int>(tail_address() - next_address()) >= length;
        } else {
            return static_cast<int>(earliest_address() - next_address()) >= length;
        }
    }
};

} // core::message::message
#undef LOGHEAD
#endif
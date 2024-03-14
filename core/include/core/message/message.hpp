#ifndef CORE_MESSAGE
#define CORE_MESSAGE

#include "mmap.hpp"
#include <typeinfo>
#include "core/datas.hpp"


#define LOGHEAD "[message::" + std::string(__func__) + "]"

constexpr int KB = 1024;
constexpr int MB = 1024 * KB;
constexpr int GB = 1024 * MB;

constexpr int MarketBboSize = sizeof(core::datas::Market_bbo);
constexpr int MarketDepthSize = sizeof(core::datas::Market_depth);
constexpr int MarketKlineSize = sizeof(core::datas::Market_kline);

constexpr int SymbolObjSize = sizeof(core::datas::SymbolObj);
constexpr int OrderObjSize = sizeof(core::datas::OrderObj);
constexpr int CancelObjSize = sizeof(core::datas::CancelObj);

namespace {
using namespace core::message::sharememory;

struct alignas(64) Header {
    bool is_cover = false;
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

constexpr size_t HeaderSize = static_cast<size_t>(sizeof(Header));
constexpr size_t MarketDataHeaderSize = static_cast<size_t>(sizeof(core::datas::MarketDataHeader));
constexpr size_t CommandDataHeaderSize = static_cast<size_t>(sizeof(core::datas::CommandDataHeader));

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

        assert(memory_check(this) == true && "Memory overlap");
    }

    virtual ~Channel() {
        release(address, size);
    }

    static bool memory_check(const Channel* const channel) {
        static uint64_t ranges[2] = {0};
        if (std::max(ranges[0], channel->front_address()) <= std::min(ranges[1], channel->tail_address())) {
            spdlog::error("{} R1({}, {}) R2({}, {})", LOGHEAD, ranges[0], ranges[1], channel->front_address(), channel->tail_address());
            return false;
        }

        ranges[0] = channel->front_address();
        ranges[1] = channel->tail_address();
        return true;
    }

    virtual uint64_t earliest_displacement() {
        return header ? header->data_earliest_displacement : 0;
    }

    virtual uint64_t lastest_displacement() {
        return header ? header->data_lastest_displacement : 0;
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

    std::pair<core::datas::Market_base*, core::datas::MarketDataHeader*> read(const uintptr_t target_address) const {
        core::datas::MarketDataHeader* _header = reinterpret_cast<core::datas::MarketDataHeader*>(target_address);
        if (_header == nullptr) { return std::make_pair(nullptr, nullptr); }

        if (_header->type == core::datas::MarketType::Bbo && _header->data_size == MarketBboSize) {
            return std::make_pair(reinterpret_cast<core::datas::Market_bbo*>(target_address + MarketDataHeaderSize), _header);
        } else if (_header->type == core::datas::MarketType::Depth && _header->data_size == MarketDepthSize) {
            return std::make_pair(reinterpret_cast<core::datas::Market_depth*>(target_address + MarketDataHeaderSize), _header);
        } else if (_header->type == core::datas::MarketType::Kline && _header->data_size == MarketKlineSize) {
            return std::make_pair(reinterpret_cast<core::datas::Market_kline*>(target_address + MarketDataHeaderSize), _header);
        }

        return std::make_pair(nullptr, _header);
    }

    // update displacement
    std::pair<core::datas::Market_base*, core::datas::MarketDataHeader*> read(const uintptr_t target_address, uint64_t &target_displacement) const {
        std::pair<core::datas::Market_base*, core::datas::MarketDataHeader*> market_pair = read(target_address);
        if (market_pair.first != nullptr) {
            target_displacement = target_displacement + CommandDataHeaderSize + market_pair.second->data_size;
        }
        return market_pair;
    }

    std::pair<core::datas::Market_base*, core::datas::MarketDataHeader*> read_next(uint64_t &target_displacement) const {
        if (target_displacement == header->data_next_displacement) { return std::make_pair(nullptr, nullptr); }
        auto result = read(target_address(target_displacement), target_displacement);
        if (result.first) { return result; }

        target_displacement = header->data_lastest_displacement;
        return read(target_address(target_displacement), target_displacement);
    }

private:
    template<typename T>
    bool insert(T &value) {
        // lock

        int _market_size = 0;
        core::datas::MarketType _type = core::datas::MarketType::Unknow;
        if constexpr (std::is_same<T, core::datas::Market_bbo>::value) {
            _market_size = MarketBboSize;
            _type = core::datas::MarketType::Bbo;
        } else if constexpr (std::is_same<T, core::datas::Market_depth>::value) {
            _market_size = MarketDepthSize;
            _type = core::datas::MarketType::Depth;
        } else if constexpr (std::is_same<T, core::datas::Market_kline>::value) {
            _market_size = MarketKlineSize;
            _type = core::datas::MarketType::Kline;
        } else {
            spdlog::error("{} unsupport type", LOGHEAD);
            return false;
        }

        int length = _market_size + MarketDataHeaderSize;
        if (!apply(length)) {
            // cover
            std::cout << "cover" << std::endl;
            // header->is_cover = true;
            header->data_next_displacement = header->data_front_displacement;

            if (!apply(length)) { return false; }

            core::datas::MarketDataHeader* earliest_header = reinterpret_cast<core::datas::MarketDataHeader*>(earliest_address());
            header->data_earliest_displacement = header->data_earliest_displacement + earliest_header->data_size + MarketDataHeaderSize;
            if (header->data_earliest_displacement >= header->data_tail_displacement) {
                header->data_earliest_displacement = header->data_front_displacement;
            }
        }

        // data_header
        header->data_lastest_displacement = header->data_next_displacement;
        core::datas::MarketDataHeader* _header = reinterpret_cast<core::datas::MarketDataHeader*>(lastest_address());
        _header->data_size = _market_size;
        _header->type = _type;

        // date_body
        T* data = reinterpret_cast<T*>(lastest_address() + MarketDataHeaderSize);
        std::memcpy(data, &value, _market_size);

        header->data_next_displacement = header->data_lastest_displacement + MarketDataHeaderSize + _market_size;
        return true;
    }

    inline bool apply(int length) {
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

    std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> read(const uintptr_t target_address) const {
        core::datas::CommandDataHeader* _header = reinterpret_cast<core::datas::CommandDataHeader*>(target_address);
        if (_header == nullptr) { return std::make_pair(nullptr, nullptr); }

        if (_header->type == core::datas::CommandType::SUBSCRIBE) {
            return std::make_pair(reinterpret_cast<core::datas::SymbolObj*>(target_address + CommandDataHeaderSize), _header);
        } else if (_header->type == core::datas::CommandType::UNSUBSCRIBE) {
            return std::make_pair(reinterpret_cast<core::datas::SymbolObj*>(target_address + CommandDataHeaderSize), _header);
        } else if (_header->type == core::datas::CommandType::ORDER && _header->data_size == OrderObjSize) {
            return std::make_pair(reinterpret_cast<core::datas::OrderObj*>(target_address + CommandDataHeaderSize), _header);
        } else if (_header->type == core::datas::CommandType::CANCEL && _header->data_size == CancelObjSize) {
            return std::make_pair(reinterpret_cast<core::datas::CancelObj*>(target_address + CommandDataHeaderSize), _header);
        }

        return std::make_pair(nullptr, _header);
    }

    std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> read(const uintptr_t target_address, uint64_t &target_displacement) const {
        std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> command_pair = read(target_address);
        if (command_pair.first != nullptr) {
            target_displacement = target_displacement + CommandDataHeaderSize + command_pair.second->data_size;
        }
        return command_pair;
    }

    std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> read_next(uint64_t &target_displacement) const {
        if (target_displacement > header->data_lastest_displacement) { return std::make_pair(nullptr, nullptr); }
        std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> result = read(target_address(target_displacement), target_displacement);
        if (result.first) { return result; }

        target_displacement = header->data_lastest_displacement;
        return read(target_address(target_displacement));
    }

private:
    template<typename T>
    bool insert(T &value) {
        // lock

        int _command_size = 0;
        core::datas::CommandType _type = core::datas::CommandType::UNKNOW;
        if constexpr (std::is_same<T, core::datas::SymbolObj>::value) {
            _command_size = SymbolObjSize;
            _type = value.command_type;
        } else if constexpr (std::is_same<T, core::datas::OrderObj>::value) {
            _command_size = OrderObjSize;
            _type = core::datas::CommandType::ORDER;
        } else if constexpr (std::is_same<T, core::datas::CancelObj>::value) {
            _command_size = CancelObjSize;
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
        core::datas::CommandDataHeader* _header = reinterpret_cast<core::datas::CommandDataHeader*>(lastest_address());
        _header->data_size = _command_size;
        _header->type = _type;
        _header->status = core::datas::CommandStatus::EFFECTIVE;

        // date_body
        T* data = reinterpret_cast<T*>(lastest_address() + CommandDataHeaderSize);
        std::memcpy(data, &value, _command_size);

        header->data_next_displacement = header->data_lastest_displacement + CommandDataHeaderSize + _command_size;
        return true;
    }

    inline bool apply(int length) {
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
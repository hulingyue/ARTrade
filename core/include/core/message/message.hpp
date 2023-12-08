#include "mmap.hpp"
#include <typeinfo>


#define LOGHEAD "[message::" + std::string(__func__) + "]"

constexpr int KB = 1024;
constexpr int MB = 1024 * KB;
constexpr int GB = 1024 * MB;


namespace {
using namespace core::message::sharememory;

struct alignas(64) Header {
    size_t header_length = 0;
    size_t data_length = 0;
    uintptr_t data_address = 0;
};

class Channel {
public:
    Channel(const std::string& path, size_t size, bool is_writting) : path(path), size(size), is_writting(is_writting) {
        address = load(std::move(path), size, is_writting);

        if (address < 0) {
            throw std::runtime_error(LOGHEAD + " unable to load buffer!");
        }

        header = reinterpret_cast<Header *>(address);
        // header->header_length = static_cast<size_t>(sizeof(Header));
        // header->data_length = size - header->header_length;
        // header->data_address = reinterpret_cast<uintptr_t>(&header + header->data_length);
        std::cout << header << std::endl;
        std::cout << header->header_length << std::endl;
        size_t a;
        std::cout << typeid(header->header_length).name() << std::endl;
        std::cout << sizeof(header->header_length) << std::endl;
        std::cout << typeid(a).name() << std::endl;
        std::cout << sizeof(a) << std::endl;
        header->header_length = 1;
    }

    virtual ~Channel() {

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

private:

};


class OrderChannel: protected Channel {
public:
    OrderChannel() = delete;
    virtual ~OrderChannel() = default;

private:
};

} // core::message::message
#undef LOGHEAD
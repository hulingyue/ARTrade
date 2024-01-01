#ifndef CORE_ORDER
#define CORE_ORDER
#define LOGHEAD "[core::order::Order" + std::string(__func__) + "]"
#include <iostream>
#include <atomic>
#include <unordered_map>
#include "core/datas.hpp"


namespace core::order {

using command_pair_type = std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*>;

class Order final {
private:
    Order() = default;
    ~Order() = default;
    Order& operator=(const Order&) = delete;
    Order& operator=(const Order&& obj) = delete;
    Order(const Order& obj) = delete;
    Order(Order&& obj) = delete;

public:
    static Order* get_instance() {
        static std::atomic<Order*> instance = nullptr;
        static std::atomic_flag flag = ATOMIC_FLAG_INIT;
        if (instance.load() == nullptr) {
            if (!flag.test_and_set()) {
                instance.store(new Order());
                flag.clear();
            }
        }
        return instance.load();
    }

    inline bool exist(const uint64_t client_id) { return (map_orders.find(client_id) != map_orders.end()); }
    
    inline void insert_or_update(const uint64_t client_id, const command_pair_type command_pair) {
        map_orders[client_id] = command_pair;
        if (client_id > client_id_index.load()) {
            client_id_index.store(client_id);
        }
    }
    
    inline bool insert(const uint64_t client_id, const command_pair_type command_pair) {
        if (exist(client_id)) { return false; }
        map_orders[client_id] = command_pair;
        if (client_id > client_id_index.load()) {
            client_id_index.store(client_id);
        }
        return true;
    }
    
    inline bool update(const uint64_t client_id, const command_pair_type command_pair) {
        if (!exist(client_id)) { return false; }
        map_orders[client_id] = command_pair;
        return true;
     }
    
    inline bool remove(const uint64_t client_id) { return (map_orders.erase(client_id) > 0); }
    
    inline bool clean() {
        map_orders.clear();
        return map_orders.empty();
    }

    inline uint64_t next_client_id() { return (client_id_index.load() + 1) % UINT64_MAX; }

private:
    std::unordered_map<uint64_t, command_pair_type> map_orders;
    std::atomic<uint64_t> client_id_index = 1;
};
} // namespace core::order

#undef LOGHEAD
#endif
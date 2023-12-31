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

    inline bool exist(const int client_id) { return (map_orders.find(client_id) != map_orders.end()); }
    
    inline void insert_or_update(const int client_id, const command_pair_type command_pair) { map_orders[client_id] = command_pair; }
    
    inline bool insert(const int client_id, const command_pair_type command_pair) {
        if (exist(client_id)) { return false; }
        map_orders[client_id] = command_pair;
        return true;
    }
    
    inline bool update(const int client_id, const command_pair_type command_pair) {
        if (!exist(client_id)) { return false; }
        map_orders[client_id] = command_pair;
        return true;
     }
    
    inline bool remove(const int client_id) { return (map_orders.erase(client_id) > 0); }
    
    inline bool clean() {
        map_orders.clear();
        return map_orders.empty();
    }

private:
    std::unordered_map<int, command_pair_type> map_orders;
};
} // namespace core::order

#undef LOGHEAD
#endif
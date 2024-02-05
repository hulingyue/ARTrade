#ifndef CORE_ORDER
#define CORE_ORDER
#define LOGHEAD "[core::order::Order" + std::string(__func__) + "]"
#include <iostream>
#include <atomic>
#include <unordered_map>
#include "core/datas.hpp"
#include "core/time.hpp"


namespace core::order {
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

    inline bool exist(const uint64_t client_id) {
        return (map_orders.find(std::to_string(client_id)) != map_orders.end());
    }
    
    inline void insert_or_update(const uint64_t client_id, core::datas::OrderObj* obj) {
        map_orders[std::to_string(client_id)] = obj;
    }
    
    inline bool insert(const uint64_t client_id, core::datas::OrderObj* obj) {
        if (exist(client_id)) { return false; }
        map_orders[std::to_string(client_id)] = obj;
        return true;
    }
    
    inline bool update(const uint64_t client_id, core::datas::OrderObj* obj) {
        if (!exist(client_id)) { return false; }
        map_orders[std::to_string(client_id)] = obj;
        return true;
     }
    
    inline bool remove(const uint64_t client_id) {
        return (map_orders.erase(std::to_string(client_id)) > 0);
    }
    
    inline bool clean() {
        map_orders.clear();
        return map_orders.empty();
    }

    core::datas::OrderObj* find(const std::string client_id) {
        auto iterator = map_orders.find(client_id);
        if (iterator == map_orders.end()) {
            return nullptr;
        }
        return iterator->second;
    }

    core::datas::OrderObj* find(const uint64_t client_id) {
        return find(std::to_string(client_id));
    }

    inline uint64_t next_client_id() {
        constexpr uint64_t key = UINT64_MAX / 1000;
        client_id_index.store((client_id_index.load() + 1) % 1000);
        return (((core::time::Time().to_nanoseconds() % key) * 100) + client_id_index.load());
    }

private:
    std::unordered_map<std::string, core::datas::OrderObj*> map_orders;
    std::atomic<uint64_t> client_id_index = 1;
};
} // namespace core::order

#undef LOGHEAD
#endif
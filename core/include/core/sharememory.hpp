#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

#include <spdlog/spdlog.h>

#include "time.hpp"
#include "util.h"
#include "pimpl.h"

#define LOGHEAD "[SHAREMEMORY::" + std::string(__func__) + "]"


namespace core::sharememory {

enum class memory_type : char {
      unknow
    , market
    , command
    , response
};

template<typename T>
class ShareMemory {
public:
    ShareMemory(std::string name, int proj_id, memory_type _type);
    virtual ~ShareMemory();

public:
    bool create();
    bool connect();

    bool write(T data, bool is_safe = true, bool is_cover = false);
    T* read();

private:
    template_Self<T> &self;
};
    
} // namespace core::sharememory


/********************************/ 

namespace {

const int buffer_size = 1023;

template<typename T>
class Buffer {
public:
    u_int16_t head = 0;
    u_int16_t tail = 0;
    u_int32_t id = 0;
    u_int32_t sign = 0;
    T data[buffer_size];

public:
    inline bool is_empty() {
        return head == tail;
    }

    inline bool is_full() {
        return (tail + 1) % buffer_size == head;
    }

    T* lastest() {
        if (is_empty()) { return nullptr; }
        return &data[tail];
    }

    T* oldest() {
        if (is_empty()) { return nullptr; }
        return &data[head];
    }

    T* read_by_index(int index) {
        if (index < 0 || index >= buffer_size || is_empty()) { return nullptr; }
        return &data[index];
    }

    T* read_by_offset(int offset) {
        if (is_empty()) { return nullptr; }
        int index = (head + offset) % buffer_size;
        return &data[index];
    }

    bool write(T &&value, bool is_safe, bool is_cover) {
        auto nanosecond = core::time::Time::now_nanosecond();

        while (true) {
            uint32_t now = core::time::Time::now_nanosecond();
            if (now - nanosecond > core::time::Time::second_2_nanosecond<1>()) {
                spdlog::error("{} timeout!", LOGHEAD);
                break;
            }

            if (is_safe) {
                if (sign > 0) { continue; }

                sign = now;
                if (sign != now) { continue; }
            }

            if (is_full() && !is_cover) { break; }

            if (is_cover && is_full()) {
                head = (head + 1) % buffer_size;
            }
            tail = (tail + 1) % buffer_size;
            data[tail] = value;
            id++;
            sign = 0;
            return true;
        }

        return false;
    }

};

template<typename T>
struct template_Self {
    std::string name;
    int proj_id;
    int shmid;
    core::sharememory::memory_type _type;
    Buffer<T> *buffer = nullptr;
};


template<typename T>
static inline key_t generate_key(const template_Self<T>& self) {
    // core::util::create_folder()
    int fd = open(self.name.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        spdlog::error("{} Failed to create temporary file!", LOGHEAD);
        exit(-1);
    }
    key_t key = ftok(self.name.c_str(), self.proj_id);

    close(fd);
    if (unlink(self.name.c_str()) == -1) {
        spdlog::error("{} Failed to delete temporary file!", LOGHEAD);
        exit(-1);
    }

    if (key == -1) {
        spdlog::error("{} generate key error! pathname: {} proj_id: {}", LOGHEAD, self.name.c_str(), self.proj_id);
        exit(-1);
    }
    return key;
}

template<typename T>
static inline int generate_permission(const template_Self<T>& self, const int mode = 0000) {
    spdlog::debug("{} Buffer size: {}", LOGHEAD, sizeof(Buffer<T>));
    
    int permission = 0000;
    switch (self._type) {
    case core::sharememory::memory_type::market:
        permission = 0644;
        break;
    case core::sharememory::memory_type::command:
    case core::sharememory::memory_type::response:
        permission = 0666;
        break;
    default:
        spdlog::error("{} memory_type error: {}", LOGHEAD, static_cast<int>(self._type));
        exit(-1);
    }

    return permission | mode;
}

template<typename T>
static inline Buffer<T>* connect_memory(template_Self<T>& self, int permission) {
    key_t key = generate_key(self);

    int capacity = sizeof(Buffer<T>);
    int size = ((capacity + 4095) / 4096) * 4096;

    self.shmid = shmget(key, size, permission);
    if (self.shmid == -1) {
        spdlog::error("{} shmget error! name: {} permission: {}", LOGHEAD, self.name, permission);
        // exit(-1);
        return nullptr;
    }

    // 连接到共享内存区域
    Buffer<T>* buffer = (Buffer<T>*)shmat(self.shmid, nullptr, 0);
    if (buffer == (Buffer<T>*) -1) {
        spdlog::error("{} shmat error!", LOGHEAD);
        exit(-2);
    }

    spdlog::info("{} Successfully connected!", LOGHEAD);
    return buffer;
}

}

namespace core::sharememory {

template<typename T>
ShareMemory<T>::ShareMemory(std::string name, int proj_id, memory_type _type) : self { *new template_Self<T>{} } {
    self.name = std::move(name);
    self.proj_id = proj_id;
    self._type = _type;
}

template<typename T>
ShareMemory<T>::~ShareMemory() {
    shmctl(self.shmid, IPC_RMID, nullptr); 
    if (&self) delete &self;
}

template<typename T>
bool ShareMemory<T>::create() {
    while (connect()) {
        shmctl(self.shmid, IPC_RMID, nullptr); 
    }
    int permission = generate_permission(self, IPC_CREAT);
    self.buffer = connect_memory(self, permission);
    return self.buffer != nullptr;
}

template<typename T>
bool ShareMemory<T>::connect() {
    int permission = generate_permission(self);
    self.buffer = connect_memory(self, permission);
    return self.buffer != nullptr;
}

template<typename T>
bool ShareMemory<T>::write(T data, bool is_safe, bool is_cover) {
    if (self.buffer == nullptr) {
        spdlog::error("{} buffer is nullptr! ", LOGHEAD);
        exit(-1);
    }
    
    return self.buffer->write(std::move(data), is_safe, is_cover);
}

template<typename T>
T* ShareMemory<T>::read() {
    if (self.buffer == nullptr) {
        spdlog::error("{} buffer is nullptr! ", LOGHEAD);
        exit(-1);
    }

    return self.buffer->lastest();
}

} // namespace core::sharememory

#undef LOGHEAD

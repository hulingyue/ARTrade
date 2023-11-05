#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "pimpl.h"


namespace core::sharememory {

enum class memory_type {
    unknow = 0,
    market = 1,
    trade = 10
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

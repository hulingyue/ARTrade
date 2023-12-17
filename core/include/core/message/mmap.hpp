/**
 * Reference declaration: This code draws heavily from [Kungfu].
 * Kungfu: https://github.com/kungfu-origin/kungfu
*/
#ifndef CORE_MMAP
#define CORE_MMAP

#pragma once
#include <iostream>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <spdlog/spdlog.h>


#define LOGHEAD "[mmap::"+ std::string(__func__) +"]"

namespace core::message::sharememory {

inline uintptr_t load(const std::string &path, size_t size, bool is_writting) {
    int fd = open(path.c_str(), (is_writting ? O_RDWR : O_RDONLY) | O_CREAT, (mode_t) 0600);
    if (fd < 0) {
        throw std::runtime_error(LOGHEAD + " failed to open file for page " + path);
    }

    if (is_writting) {
        if (lseek(fd, size - 1, SEEK_SET) == -1) {
            close(fd);
            throw std::runtime_error(LOGHEAD + " failed to stretch for page " + path);
        }
        if (write(fd, "", 1) == -1) {
            close(fd);
            throw std::runtime_error(LOGHEAD + " unable to write for page " + path);
        }
    }

    void *buffer = mmap(0, size, is_writting ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, fd, 0);

    if (buffer == MAP_FAILED) {
        close(fd);
        throw std::runtime_error(LOGHEAD + " Error mapping file to buffer");
    }

    if (madvise(buffer, size, MADV_RANDOM) != 0 && mlock(buffer, size) != 0) {
        munmap(buffer, size);
        close(fd);
        throw std::runtime_error(LOGHEAD + " failed to lock memory for page " + path);
    }

    close(fd);
    spdlog::debug("{} mapped {} - {}", LOGHEAD, path, is_writting ? "rw" : "r");
    return reinterpret_cast<uintptr_t>(buffer);
}

inline bool release(uintptr_t address, size_t size) {
    void *buffer = reinterpret_cast<void *>(address);
    //unlock and unmap
    if (munlock(buffer, size) != 0) {
        return false;
    }

    if (munmap(buffer, size) != 0) {
        return false;
    }

    return true;
}

} // core::message::sharememory

#undef LOGHEAD
#endif
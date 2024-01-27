use cxx::bridge;

#[cxx::bridge]
pub mod ffi {
    extern "C++" {
        include!("/home/ubuntu/Quant/ARTrade/core/include/core/datas.hpp");
        namespace core::datas;
        type MessageType = MessageType;
    }
}
#include "mouse.h"

namespace {
struct Self {

};
}

Mouse::Mouse(int argc, char** argv) : Modules(argc, argv), self (*new Self{}) {}

Mouse::~Mouse() {}

std::string Mouse::project_name() {
    return "SGE";
}

core::datas::MessageType Mouse::message_type() {
    return core::datas::MessageType::ShareMemory;
}

void Mouse::init_config() {}

void Mouse::interval_1s() {}

MouseMarket& Mouse::market() {}
#include <core/modules.h>
#include <core/pimpl.h>
#include <core/datas.hpp>

#include "mouse_market.h"


class Mouse final : public core::modules::Modules {
public:
    Mouse(int argc, char** argv);
    ~Mouse();
    
    std::string project_name() override;
    core::datas::MessageType message_type() override;

    void init_config() override;

    void interval_1s() override;

    MouseMarket& market();

private:
    Self &self;
};
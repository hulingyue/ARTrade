#include <gtest/gtest.h>
#include <core/message/message.hpp>
#include <core/time.hpp>


using namespace core::message::message;


class MarketChannelTest : public ::testing::Test {
protected:
    void SetUp() override {
        channel = new MarketChannel("Bybit", 40 * KB, true);
    }

    void TearDown() override {
        if (channel) { delete channel; }
    }

    MarketChannel *channel;
};

TEST_F(MarketChannelTest, WriteTest) {
    core::datas::Market_bbo bbo{
        {
            .market_type = core::datas::MarketType::Bbo,
            .symbol = "BTCUSDT",
            .exchange = "Bybit",
            .time = core::time::Time::now_nanosecond()
        },
        .price = 40000.2,
        .quantity = 23432.1
    };
    
    uint64_t displacement = 0;
    while (true) {
        spdlog::info("displacement: {} last: {}", displacement, channel->lastest_displacement());
        if (displacement > channel->lastest_displacement()) { break; }
        displacement = channel->lastest_displacement();
        bool result = channel->write(bbo);
        ASSERT_EQ(result, true);
    }
}

TEST_F(MarketChannelTest, ReadTest) {
    core::datas::Market_bbo bbo{
        {
            .market_type = core::datas::MarketType::Bbo,
            .symbol = "BTCUSDT",
            .exchange = "Bybit",
            .time = core::time::Time::now_nanosecond()
        },
        .price = 40000.2,
        .quantity = 23432.1
    };

    int count = 10000;

    for (int index = 0; index < count; index++) {
        bbo.time = core::time::Time::now_nanosecond();
        channel->write(bbo);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        spdlog::info("insert displacement: {}", channel->lastest_displacement());
    }

    uint64_t displacement = channel->earliest_displacement();
    core::datas::Market_base* result = nullptr;
    for(int index = 0; index < count; index++) {
        
        result = channel->read_next(displacement);

        // debug
        if (result == nullptr) {
            spdlog::error("displacement: {} last: {}", displacement, channel->lastest_displacement());
        } else {
            switch (result->market_type) {
            case core::datas::MarketType::Bbo: {
                core::datas::Market_bbo* bbo = reinterpret_cast<core::datas::Market_bbo*>(result);
                spdlog::info("exchange: {} symbol: {} time: {} displacement: {}", bbo->exchange, bbo->symbol, bbo->time, displacement);
                break;
            }
            default:
                break;
            }
        }
        spdlog::info("displacement: {} last: {}", displacement, channel->lastest_displacement());
        result = channel->read_next(displacement);
        ASSERT_NE(result, nullptr);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

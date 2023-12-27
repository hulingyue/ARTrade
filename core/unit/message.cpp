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
        // spdlog::info("displacement: {} last: {}", displacement, channel->lastest_displacement());
        if (displacement > channel->lastest_displacement()) { break; }
        displacement = channel->lastest_displacement();
        bool result = channel->write(bbo);
        ASSERT_EQ(result, true);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

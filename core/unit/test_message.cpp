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
    core::datas::Market_bbo bbo;
    bbo.market_type = core::datas::MarketType::Bbo;
    std::strcpy(bbo.symbol, "BTCUSDT");
    std::strcpy(bbo.exchange, "Bybit");
    bbo.time = core::time::Time().to_nanoseconds();
    bbo.price = 40000.2;
    bbo.quantity = 23432.1;
    
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
    core::datas::Market_bbo bbo;
    bbo.market_type = core::datas::MarketType::Bbo;
    std::strcpy(bbo.symbol, "BTCUSDT");
    std::strcpy(bbo.exchange, "Bybit");
    bbo.time = core::time::Time().to_nanoseconds();
    bbo.price = 40000.2;
    bbo.quantity = 23432.1;

    int count = 10000;

    uint64_t displacement = channel->earliest_displacement();
    std::pair<core::datas::Market_base*, core::datas::MarketDataHeader*> market_pair = std::make_pair(nullptr, nullptr);
    for(int index = 0; index < count; index++) {
        bbo.time = core::time::Time().to_nanoseconds();
        if (index % 2 == 0) {
            memcpy(bbo.symbol, "BTCUSDT", 16);
        } else {
            memcpy(bbo.symbol, "ETHUSDT", 16);
        }
        channel->write(bbo);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        spdlog::info("insert displacement: {}", channel->lastest_displacement());

        market_pair = channel->read_next(displacement);

        // debug
        if (market_pair.first == nullptr) {
            spdlog::error("index: {} displacement: {} last: {}", index, displacement, channel->lastest_displacement());
        } else {
            switch (market_pair.first->market_type) {
            case core::datas::MarketType::Bbo: {
                core::datas::Market_bbo* bbo = reinterpret_cast<core::datas::Market_bbo*>(market_pair.first);
                spdlog::info("exchange: {} symbol: {} time: {} displacement: {}", bbo->exchange, bbo->symbol, bbo->time, displacement);
                break;
            }
            default:
                break;
            }
        }
        ASSERT_NE(market_pair.first, nullptr);
    }
}



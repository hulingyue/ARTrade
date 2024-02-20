#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <core/orderbook.hpp>


class OrderbookManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {

    }

    core::orderbook::OrderbookManager manager;
    std::string symbol = "BTCUSDT";
};

TEST_F(OrderbookManagerTest, UpdateTest_1) {
    int depth = 1;
    manager.key_exist_or_create(symbol, depth);
    
    core::datas::TradePair ask_pair[] = {{ .price = 50000.01, .quantity = 1.01 }};
    core::datas::TradePair bid_pair[] = {{ .price = 49999.99, .quantity = 1.51 }};
    
    // 
    manager.init_asks(symbol, ask_pair, depth);
    manager.init_bids(symbol, bid_pair, depth);
    
    auto asks = manager.get_asks(symbol, depth);
    auto bids = manager.get_bids(symbol, depth);

    ASSERT_FLOAT_EQ(asks->price, ask_pair->price);
    ASSERT_FLOAT_EQ(bids->price, bid_pair->price);

    // 
    manager.update_asks(symbol, depth, 50000.00, 0);
    manager.update_bids(symbol, depth, 50000.00, 0);

    asks = manager.get_asks(symbol, depth);
    bids = manager.get_bids(symbol, depth);
    ASSERT_FLOAT_EQ(asks->price, ask_pair->price);
    ASSERT_FLOAT_EQ(bids->price, bid_pair->price);

    // 
    manager.update_asks(symbol, depth, 50000.01, 0);
    manager.update_bids(symbol, depth, 49999.99, 0);

    asks = manager.get_asks(symbol, depth);
    bids = manager.get_bids(symbol, depth);
    ASSERT_FLOAT_EQ(asks->price, ask_pair->price);
    ASSERT_FLOAT_EQ(bids->price, bid_pair->price);

    // 
    manager.update_asks(symbol, depth, 49999.999, 0.1);
    manager.update_bids(symbol, depth, 49999.991, 0.1);

    asks = manager.get_asks(symbol, depth);
    bids = manager.get_bids(symbol, depth);
    ASSERT_FLOAT_EQ(asks->price, 49999.999);
    ASSERT_FLOAT_EQ(bids->price, 49999.991);
}

TEST_F(OrderbookManagerTest, UpdateTest_3) {
    int depth = 3;
    manager.key_exist_or_create(symbol, depth);

    core::datas::TradePair ask_pair[] = {
        { .price = 50000.02, .quantity = 1.01 },
        { .price = 50000.04, .quantity = 1.01 },
        { .price = 50000.06, .quantity = 1.01 }
    };
    core::datas::TradePair bid_pair[] = {
        { .price = 49999.95, .quantity = 1.51 },
        { .price = 49999.94, .quantity = 1.01 },
        { .price = 49999.93, .quantity = 1.01 }
    };
    
    // 
    manager.init_asks(symbol, ask_pair, depth);
    manager.init_bids(symbol, bid_pair, depth);
    
    auto asks = manager.get_asks(symbol, depth);
    auto bids = manager.get_bids(symbol, depth);

    ASSERT_FLOAT_EQ(asks->price, 50000.02);
    ASSERT_FLOAT_EQ(bids->price, 49999.95);

    // 
    manager.update_asks(symbol, depth, 50000.01, 1.1);

    asks = manager.get_asks(symbol, depth);
    bids = manager.get_bids(symbol, depth);
    ASSERT_FLOAT_EQ(asks->price, 50000.01);
    ASSERT_FLOAT_EQ((asks++)->price, 50000.02);
    spdlog::info("[asks] price: {} quantity: {}", asks->price, asks->quantity);
    spdlog::info("[bids] price: {} quantity: {}", bids->price, bids->quantity);
}


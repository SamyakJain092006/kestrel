#include "price_level.hpp"
#include <gtest/gtest.h>

TEST(PriceLevelTest, FillSequence)
{
    OrderBook::PriceLevel price_level;

    OrderBook::Order order(1, 100, 10, OrderBook::Side::Buy);
    price_level.AppendOrder(order);

    auto fill_4_result = price_level.ApplyFillToOldestOrder(4);
    EXPECT_EQ(fill_4_result.status, OrderBook::FillStatus::PartiallyFilled)
        << "Fill 4 should partially fill the order";
    EXPECT_FALSE(fill_4_result.complete_order_id.has_value())
        << "Fill 4 should not complete the order";

    auto fill_6_result = price_level.ApplyFillToOldestOrder(6);
    EXPECT_EQ(fill_6_result.status, OrderBook::FillStatus::FullyFilled)
        << "Fill 6 should fully fill the order";
    ASSERT_TRUE(fill_6_result.complete_order_id.has_value())
        << "Fill 6 should return a completed order id";
    EXPECT_EQ(fill_6_result.complete_order_id.value(), 1)
        << "Fill 6 should complete order id 1";

    auto oldest_order = price_level.GetOldestOrder();
    EXPECT_FALSE(oldest_order.has_value())
        << "Get oldest should return an empty optional after the order is filled";

    auto fill_1_result = price_level.ApplyFillToOldestOrder(1);
    EXPECT_EQ(fill_1_result.status, OrderBook::FillStatus::NoActiveOrder)
        << "Fill 1 should report no active order";
}

TEST(PriceLevelTest, FifoOrder)
{
    OrderBook::PriceLevel price_level;

    OrderBook::Order order_1(1, 100, 5, OrderBook::Side::Buy);
    OrderBook::Order order_2(2, 100, 5, OrderBook::Side::Buy);

    price_level.AppendOrder(order_1);
    price_level.AppendOrder(order_2);

    auto fully_filled_result = price_level.ApplyFillToOldestOrder(5);

    EXPECT_EQ(fully_filled_result.status, OrderBook::FillStatus::FullyFilled);
    ASSERT_TRUE(fully_filled_result.complete_order_id.has_value());
    EXPECT_EQ(fully_filled_result.complete_order_id.value(), 1);

    auto oldest_order = price_level.GetOldestOrder();

    ASSERT_TRUE(oldest_order.has_value());
    EXPECT_EQ(oldest_order->order_id, 2);
    EXPECT_EQ(oldest_order->remaining_quantity, 5);
}

TEST(PriceLevelTest, ZeroFill)
{
    OrderBook::PriceLevel price_level;

    OrderBook::Order order(1, 100, 10, OrderBook::Side::Buy);
    price_level.AppendOrder(order);

    auto zero_fill_result = price_level.ApplyFillToOldestOrder(0);

    EXPECT_EQ(zero_fill_result.status, OrderBook::FillStatus::InvalidFill);

    auto oldest_order = price_level.GetOldestOrder();

    ASSERT_TRUE(oldest_order.has_value());
    EXPECT_EQ(oldest_order->order_id, 1);
    EXPECT_EQ(oldest_order->remaining_quantity, 10);
}

TEST(PriceLevelTest, OverFill)
{
    OrderBook::PriceLevel price_level;

    OrderBook::Order order(1, 100, 10, OrderBook::Side::Buy);
    price_level.AppendOrder(order);

    auto over_fill_result = price_level.ApplyFillToOldestOrder(11);

    EXPECT_EQ(over_fill_result.status, OrderBook::FillStatus::InvalidFill);

    auto oldest_order = price_level.GetOldestOrder();

    ASSERT_TRUE(oldest_order.has_value());
    EXPECT_EQ(oldest_order->order_id, 1);
    EXPECT_EQ(oldest_order->remaining_quantity, 10);
}

TEST(PriceLevelTest, NoFill)
{
    OrderBook::PriceLevel price_level;

    auto no_fill_result = price_level.ApplyFillToOldestOrder(1);

    EXPECT_EQ(no_fill_result.status, OrderBook::FillStatus::NoActiveOrder);

    auto oldest_order = price_level.GetOldestOrder();

    EXPECT_FALSE(oldest_order.has_value());
}

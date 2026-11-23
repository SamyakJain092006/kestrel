#include "price_level.hpp"
#include <iostream>
#include <cstdint>

std::int32_t fail_counter = 0;

void result_check(bool result, const char *message)
{
    if (!result)
    {
        std::cerr << "FAIL: " << message << std::endl;
        fail_counter++;
    }
    return;
}

void test_fill_sequence()
{
    OrderBook::PriceLevel price_level;

    OrderBook::Order order(1, 100, 10, OrderBook::Side::Buy);
    price_level.AppendOrder(order);

    auto fill_4_result = price_level.ApplyFillToOldestOrder(4);
    result_check(
        fill_4_result.status == OrderBook::FillStatus::PartiallyFilled,
        "Fill 4 should partially fill the order");
    result_check(
        !fill_4_result.complete_order_id.has_value(),
        "Fill 4 should not complete the order");

    auto fill_6_result = price_level.ApplyFillToOldestOrder(6);
    result_check(
        fill_6_result.status == OrderBook::FillStatus::FullyFilled,
        "Fill 6 should fully fill the order");
    result_check(
        fill_6_result.complete_order_id.has_value(),
        "Fill 6 should return a completed order id");
    result_check(
        fill_6_result.complete_order_id == 1,
        "Fill 6 should complete order id 1");

    auto oldest_order = price_level.GetOldestOrder();
    result_check(
        !oldest_order.has_value(),
        "Get oldest should return an empty optional after the order is filled");

    auto fill_1_result = price_level.ApplyFillToOldestOrder(1);
    result_check(
        fill_1_result.status == OrderBook::FillStatus::NoActiveOrder,
        "Fill 1 should report no active order");
}

void test_fifo_order()
{
    OrderBook::PriceLevel price_level;
    OrderBook::Order order_1(1, 100, 5, OrderBook::Side::Buy);
    OrderBook::Order order_2(2, 100, 5, OrderBook::Side::Buy);
    price_level.AppendOrder(order_1);
    price_level.AppendOrder(order_2);
    auto fully_filled_result = price_level.ApplyFillToOldestOrder(5);
    result_check(
        fully_filled_result.status == OrderBook::FillStatus::FullyFilled, "Should be fully filled");
    result_check(
        fully_filled_result.complete_order_id == 1, "Fill should be complete with Order Id: 1");
    auto oldestOrder = price_level.GetOldestOrder();
    result_check(
        oldestOrder.has_value() && oldestOrder->order_id == 2, "Oldest Order Id should be 2");
    result_check(
        oldestOrder.has_value() && oldestOrder->remaining_quantity == 5, "Oldest Order remaining quantity should be 5");
}

void test_zero_fill()
{
    OrderBook::PriceLevel price_level;
    OrderBook::Order order(1, 100, 10, OrderBook::Side::Buy);
    price_level.AppendOrder(order);
    auto zero_fill_result = price_level.ApplyFillToOldestOrder(0);
    result_check(
        zero_fill_result.status == OrderBook::FillStatus::InvalidFill, "Should be invalid filled because of zero fill");
    auto oldestOrder = price_level.GetOldestOrder();
    result_check(
        oldestOrder.has_value() && oldestOrder->order_id == 1, "Oldest Order Id should be 1");
    result_check(
        oldestOrder.has_value() && oldestOrder->remaining_quantity == 10, "Oldest Order Remaining Quantity should be 10");
}

void test_over_fill()
{
    OrderBook::PriceLevel price_level;
    OrderBook::Order order(1, 100, 10, OrderBook::Side::Buy);
    price_level.AppendOrder(order);
    auto over_fill_result = price_level.ApplyFillToOldestOrder(11);
    result_check(
        over_fill_result.status == OrderBook::FillStatus::InvalidFill, "Should be invalid filled because of over fill");
    auto oldestOrder = price_level.GetOldestOrder();
    result_check(
        oldestOrder.has_value() && oldestOrder->order_id == 1, "Oldest Order Id should be 1");
    result_check(
        oldestOrder.has_value() && oldestOrder->remaining_quantity == 10, "Oldest Order Remaining Quantity should be 10");
}

void test_no_fill()
{
    OrderBook::PriceLevel price_level;
    auto no_fill_result = price_level.ApplyFillToOldestOrder(1);
    result_check(
        no_fill_result.status == OrderBook::FillStatus::NoActiveOrder, "Should be no active order");
    auto oldestOrder = price_level.GetOldestOrder();
    result_check(
        !oldestOrder.has_value(), "Oldest Order Id should be empty");
}

int main()
{
    test_fill_sequence();
    test_fifo_order();
    test_zero_fill();
    test_over_fill();
    test_no_fill();
    if (fail_counter == 0)
    {
        std::cout << "All tests passed" << std::endl;
    }
    else
    {
        std::cerr << "Fail Counter: " << fail_counter << std::endl;
    }
    return fail_counter == 0 ? 0 : 1;
}
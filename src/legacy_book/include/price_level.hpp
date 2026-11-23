#pragma once

#include <cstddef>
#include <vector>
#include <optional>
#include <utility>

#include "types.hpp"

namespace OrderBook
{

    enum class FillStatus : std::uint8_t
    {
        InvalidFill,
        NoActiveOrder,
        PartiallyFilled,
        FullyFilled
    };

    struct FillResult
    {
        FillStatus status;
        std::optional<OrderId> complete_order_id;
    };

    struct RestingOrderSnapshot
    {
        OrderId order_id;
        Quantity remaining_quantity;

        RestingOrderSnapshot(OrderId _order_id, Quantity _remaining_quantity)
            : order_id(_order_id),
              remaining_quantity(_remaining_quantity)
        {
        }
    };

    class PriceLevel
    {
    private:
        std::vector<std::optional<Order>> orders;
        std::size_t head_index = 0;
        std::size_t active_order_count = 0;

    public:
        std::size_t AppendOrder(Order order)
        {
            std::size_t index = orders.size();
            orders.emplace_back(std::move(order));
            ++active_order_count;
            return index;
        }

        std::optional<RestingOrderSnapshot> GetOldestOrder()
        {
            if (active_order_count == 0)
                return std::nullopt;
            std::size_t sz = orders.size();
            while (head_index < sz)
            {
                if (!orders[head_index].has_value())
                {
                    ++head_index;
                }
                else
                    break;
            }
            if (head_index == sz)
                return std::nullopt;
            else
                return RestingOrderSnapshot(orders[head_index]->GetOrderId(), orders[head_index]->GetRemainingQuantity());
        }

        FillResult ApplyFillToOldestOrder(Quantity fill_quantity)
        {
            auto oldestOrder = GetOldestOrder();
            if (!oldestOrder)
            {
                return {FillStatus::NoActiveOrder, std::nullopt};
            }
            else
            {
                bool result = orders[head_index]->ApplyFill(fill_quantity);
                if (result == true)
                {
                    if (orders[head_index]->GetRemainingQuantity() == 0)
                    {
                        active_order_count--;
                        orders[head_index].reset();
                        return {FillStatus::FullyFilled, oldestOrder->order_id};
                    }
                    else
                    {
                        return {FillStatus::PartiallyFilled, std::nullopt};
                    }
                }
                else
                {
                    return {FillStatus::InvalidFill, std::nullopt};
                }
            }
        }
    };
}
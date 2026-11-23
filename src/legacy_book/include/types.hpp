#pragma once

#include <cstdint>
#include <chrono>
#include <variant>

namespace OrderBook
{
    using OrderId = std::uint64_t;
    using Price = std::uint64_t;
    using Quantity = std::uint64_t;

    enum class Side : std::uint8_t
    {
        Buy,
        Sell
    };

    enum class SubmissionResult : std::uint8_t
    {
        Accepted,
        DuplicateOrderId,
        InvalidPrice,
        InvalidQuantity,
        InvalidSide
    };

    enum class RejectionReason : std::uint8_t
    {

        DuplicateOrderId,
        InvalidPrice,
        InvalidQuantity,
        InvalidSide
    };

    class Order
    {
    private:
        OrderId order_id;
        Price price;
        Quantity initial_quantity;
        Quantity remaining_quantity;
        Side side;

    public:
        Order(OrderId _order_id, Price _price, Quantity _initial_quantity, Side _side)
            : order_id(_order_id),
              price(_price),
              initial_quantity(_initial_quantity), remaining_quantity(_initial_quantity),
              side(_side)
        {
        }

        OrderId GetOrderId() const
        {
            return order_id;
        }

        Price GetPrice() const
        {
            return price;
        }

        Quantity GetInitialQuantity() const
        {
            return initial_quantity;
        }

        Quantity GetRemainingQuantity() const
        {
            return remaining_quantity;
        }

        Side GetSide() const
        {
            return side;
        }

        bool ApplyFill(Quantity fill_quantity)
        {
            if (fill_quantity > 0)
            {
                if (remaining_quantity >= fill_quantity)
                {
                    remaining_quantity -= fill_quantity;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    };

    struct OrderRequest
    {
        OrderId order_id;
        Side side;
        Price price;
        Quantity requested_quantity;

        OrderRequest(OrderId _order_id, Side _side, Price _price, Quantity _requested_quantity)
            : order_id(_order_id),
              side(_side),
              price(_price),
              requested_quantity(_requested_quantity)
        {
        }
    };

    class EventMetadata
    {
    private:
        std::chrono::system_clock::time_point event_timestamp;
        std::uint64_t event_sequence_number;

    public:
        EventMetadata(std::chrono::system_clock::time_point _event_timestamp, std::uint64_t _event_sequence_number)
            : event_timestamp(_event_timestamp),
              event_sequence_number(_event_sequence_number)
        {
        }

        std::chrono::system_clock::time_point Getevent_Timestamp() const
        {
            return event_timestamp;
        }

        std::uint64_t GetSequenceNumber() const
        {
            return event_sequence_number;
        }
    };

    struct AcceptedSubmissionEvent
    {
        EventMetadata metadata;
        OrderRequest request;

        AcceptedSubmissionEvent(EventMetadata _metadata, OrderRequest _request)
            : metadata(_metadata),
              request(_request)
        {
        }
    };

    struct RejectedSubmissionEvent
    {
        EventMetadata metadata;
        OrderRequest request;
        RejectionReason reason;

        RejectedSubmissionEvent(EventMetadata _metadata, OrderRequest _request, RejectionReason _reason)
            : metadata(_metadata),
              request(_request),
              reason(_reason)
        {
        }
    };

    struct TradeExecutionEvent
    {
        EventMetadata metadata;
        OrderId buy_order_id;
        OrderId sell_order_id;
        Price execution_price;
        Quantity trade_quantity;

        TradeExecutionEvent(EventMetadata _metadata, OrderId _buy_order_id, OrderId _sell_order_id, Price _execution_price, Quantity _trade_quantity)
            : metadata(_metadata),
              buy_order_id(_buy_order_id),
              sell_order_id(_sell_order_id),
              execution_price(_execution_price),
              trade_quantity(_trade_quantity)
        {
        }
    };

    struct CancellationEvent
    {
        EventMetadata metadata;
        OrderId order_id;
        Quantity cancelled_quantity;

        CancellationEvent(EventMetadata _metadata, OrderId _order_id, Quantity _cancelled_quantity)
            : metadata(_metadata),
              order_id(_order_id),
              cancelled_quantity(_cancelled_quantity)
        {
        }
    };

    using Event = std::variant<
        AcceptedSubmissionEvent,
        RejectedSubmissionEvent,
        TradeExecutionEvent,
        CancellationEvent>;

        
}
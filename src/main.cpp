#include "matching_engine.hpp"

#include <cassert>
#include <iostream>

int main()
{

    OrderBook::MatchingEngine engine;

    OrderBook::OrderRequest r1(101, OrderBook::Side::Buy, 500, 10);
    OrderBook::OrderRequest r2(102, OrderBook::Side::Sell, 0, 10);
    OrderBook::OrderRequest r3(101, OrderBook::Side::Buy, 500, 10);
    OrderBook::OrderRequest r4(102, OrderBook::Side::Sell, 500, 10);

    assert(
        engine.SubmitOrder(r1) ==
        OrderBook::SubmissionResult::Accepted);
    assert(
        engine.SubmitOrder(r2) ==
        OrderBook::SubmissionResult::InvalidPrice);
    assert(
        engine.SubmitOrder(r3) ==
        OrderBook::SubmissionResult::DuplicateOrderId);
    assert(
        engine.SubmitOrder(r4) ==
        OrderBook::SubmissionResult::DuplicateOrderId);

    const auto &events =
        engine.GetEventHistory().GetEvents();

    assert(events.size() == 4);

    assert(
        std::holds_alternative<OrderBook::AcceptedSubmissionEvent>(
            events[0]));

    const auto *rejected =
        std::get_if<OrderBook::RejectedSubmissionEvent>(
            &events[1]);

    assert(rejected != nullptr);
    assert(rejected->reason == OrderBook::RejectionReason::InvalidPrice);

    for (const auto &event : events)
    {
        auto sequence = std::visit(
            [](const auto &item)
            {
                return item.metadata.GetSequenceNumber();
            },
            event);

        std::cout << sequence << '\n';
    }

    return 0;
}

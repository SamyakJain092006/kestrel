#include "matching_engine.hpp"
#include <iostream>

int main()
{

    OrderBook::MatchingEngine engine;

    OrderBook::OrderRequest r1(101, OrderBook::Side::Buy, 500, 10);
    OrderBook::OrderRequest r2(102, OrderBook::Side::Sell, 0, 10);
    OrderBook::OrderRequest r3(101, OrderBook::Side::Buy, 500, 10);
    OrderBook::OrderRequest r4(102, OrderBook::Side::Sell, 500, 10);

    OrderBook::SubmissionResult result_1 = engine.SubmitOrder(r1);
    OrderBook::SubmissionResult result_2 = engine.SubmitOrder(r2);
    OrderBook::SubmissionResult result_3 = engine.SubmitOrder(r3);
    OrderBook::SubmissionResult result_4 = engine.SubmitOrder(r4);
    if (result_1 != OrderBook::SubmissionResult::Accepted)
    {
        std::cerr << "Result 1: unexpected submission result\n";
        return 1;
    }

    if (result_2 != OrderBook::SubmissionResult::InvalidPrice)
    {
        std::cerr << "Result 2: unexpected submission result\n";
        return 1;
    }

    if (result_3 != OrderBook::SubmissionResult::DuplicateOrderId)
    {
        std::cerr << "Result 3: unexpected submission result\n";
        return 1;
    }

    if (result_4 != OrderBook::SubmissionResult::DuplicateOrderId)
    {
        std::cerr << "Result 4: unexpected submission result\n";
        return 1;
    }

    const auto &events =
        engine.GetEventHistory().GetEvents();

    if (events.size() != 4)
    {
        std::cerr << "Unexpected event count\n";
        return 1;
    }

    if (!std::holds_alternative<OrderBook::AcceptedSubmissionEvent>(
            events[0]))
    {
        std::cerr << "Accepted submission event was not found\n";
        return 1;
    }

    const auto *rejected =
        std::get_if<OrderBook::RejectedSubmissionEvent>(
            &events[1]);

    if (rejected == nullptr)
    {
        std::cerr << "Rejected event was not found\n";
        return 1;
    }

    if (rejected->reason != OrderBook::RejectionReason::InvalidPrice)
    {
        std::cerr << "Rejected event has an unexpected reason\n";
        return 1;
    }

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

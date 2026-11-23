#pragma once

#include "types.hpp"
#include "event_history.hpp"
#include <cstdint>
#include <unordered_set>

namespace OrderBook
{
    class MatchingEngine
    {
    private:
        std::unordered_set<OrderId> attempted_order_ids;
        std::uint64_t next_sequence_number;
        EventHistory event_history;

        EventMetadata CreateEventMetadata()
        {
            EventMetadata event_metadata(std::chrono::system_clock::now(), next_sequence_number);
            next_sequence_number += 1;
            return event_metadata;
        }

        void RecordAcceptedSubmission(const OrderRequest &request)
        {
            EventMetadata event_metadata = CreateEventMetadata();
            AcceptedSubmissionEvent accepted_event(event_metadata, request);
            event_history.Append(accepted_event);
        }

        void RecordRejectedSubmission(const OrderRequest &request, RejectionReason reason)
        {
            EventMetadata event_metadata = CreateEventMetadata();
            RejectedSubmissionEvent rejected_event(event_metadata, request, reason);
            event_history.Append(rejected_event);
        }

    public:
        MatchingEngine()
            : next_sequence_number(1)
        {
        }

        SubmissionResult SubmitOrder(const OrderRequest &request)
        {
            if (attempted_order_ids.find(request.order_id) != attempted_order_ids.end())
            {
                RecordRejectedSubmission(request, RejectionReason::DuplicateOrderId);
                return SubmissionResult::DuplicateOrderId;
            }

            attempted_order_ids.insert(request.order_id);

            if (request.price == 0)
            {
                RecordRejectedSubmission(request, RejectionReason::InvalidPrice);
                return SubmissionResult::InvalidPrice;
            }

            if (request.requested_quantity == 0)
            {
                RecordRejectedSubmission(request, RejectionReason::InvalidQuantity);
                return SubmissionResult::InvalidQuantity;
            }

            if (request.side != Side::Buy && request.side != Side::Sell)
            {
                RecordRejectedSubmission(request, RejectionReason::InvalidSide);
                return SubmissionResult::InvalidSide;
            }

            RecordAcceptedSubmission(request);
            return SubmissionResult::Accepted;
        }

        const EventHistory &GetEventHistory() const
        {
            return event_history;
        }
    };
}
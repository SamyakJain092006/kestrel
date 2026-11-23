#pragma once

#include "types.hpp"
#include <vector>
namespace OrderBook
{
    class EventHistory
    {
    private:
        std::vector<Event> events;

    public:
        void Append(const Event &event)
        {
            events.push_back(event);
        }

        const std::vector<Event> &GetEvents() const
        {
            return events;
        }
    };
}
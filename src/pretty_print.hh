/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PRETTY_PRINT_HH
#define PRETTY_PRINT_HH

#include <iostream>
#include <limits>

template <typename T>
void printPacketsSent(T& container)
{
    std::cout << "[ ";
    bool is_first = true;
    for ( auto & pkt : container ){
        if (not is_first) {
            std::cout << "| ";
        }
        std::cout << pkt.time << ". " << pkt.owner << " ";
        is_first = false;
    }
    std::cout << "]" << std::endl;
}

template <typename T>
void print_consecutive_slot_range(T *start, T* end)
{
    std::cout << start->time << "-" << end->time << ": ";

    if (start->owner != "") {
        std::cout << start->owner;
    } else {
        std::cout << "null";
    }

    std::cout << " $";
    if ( start->has_offers() ) {
        std::cout << start->best_offer().cost;
    } else {
        std::cout << "null";
    }
}

template <typename T>
void printOrderBook(T& container)
{
    std::cout << "[ ";
    auto *first_consecutive_slot = &container.front(); 
    auto *last_consecutive_slot = &container.front(); 

    for (auto &slot : container)
    {
        bool owner_matches = slot.owner == last_consecutive_slot->owner;
        bool cost_matches = last_consecutive_slot->has_offers() == slot.has_offers();
        if ( cost_matches and slot.has_offers() ) {
            cost_matches = last_consecutive_slot->best_offer().cost == slot.best_offer().cost;
        }

        if ( not owner_matches or not cost_matches ) {
            print_consecutive_slot_range( first_consecutive_slot, last_consecutive_slot );
            std::cout << " | ";

            first_consecutive_slot = &slot;
        }
        last_consecutive_slot = &slot;
    }
    print_consecutive_slot_range( first_consecutive_slot, last_consecutive_slot );
    std::cout << " ]" << std::endl;
}

#endif /* PRETTY_PRINT_HH */

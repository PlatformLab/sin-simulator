/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PRETTY_PRINT_HH
#define PRETTY_PRINT_HH

#include <iostream>
#include "slot.hh"
#include "market_event.hh"

void print_cost( const PacketSent * );
void print_cost( const SingleSlot *p );

template <typename T>
void print_consecutive_slot_range(T *start, T* end)
{
    if ( start->time == end->time ) {
        std::cout << start->time << ": ";
    } else {
        std::cout << start->time << "-" << end->time << ": ";
    }

    std::cout << start->owner;

    print_cost( start );
}

bool matches( const PacketSent &s1, const PacketSent *s2 );
bool matches( const SingleSlot &s1, const SingleSlot *s2 );

template <typename T>
void printSlots(T& container)
{
    std::cout << "[ ";
    auto *first_consecutive_slot = &container.front(); 
    auto *last_consecutive_slot = &container.front(); 

    for (auto &slot : container)
    {

        if ( not matches(slot, last_consecutive_slot ) ) {
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

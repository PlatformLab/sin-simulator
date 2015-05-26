/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PRETTY_PRINT_HH
#define PRETTY_PRINT_HH

#include <iostream>

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
void printOrderBook(T& container)
{
    std::cout << "[ ";
    bool is_first = true;
    for (const auto &slot : container)
    {
        if (not is_first) {
            std::cout << " | ";
        }

        if (slot.owner != "") {
            std::cout << slot.owner;
        } else {
            std::cout << "null";
        }

        std::cout << " $";
        if (slot.has_offers()) {
            std::cout << slot.best_offer().cost;
        } else {
            std::cout << "null";
        }

        is_first = false;
    }
    std::cout << " ]" << std::endl;
}

#endif /* PRETTY_PRINT_HH */

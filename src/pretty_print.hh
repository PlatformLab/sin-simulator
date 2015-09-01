/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PRETTY_PRINT_HH
#define PRETTY_PRINT_HH

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "flow.hh"
#include "interval.hh"


static std::string uid_to_string( const size_t uid )
{
    if ( uid ) {
        return std::string( 1, 'A' + uid - 1 );
    } else {
        return std::string("ccast");
    }
}

static void print_flows( const std::vector<Flow> &flows )
{

    for ( auto &flow : flows )
    {
        std::cout << "User " << uid_to_string( flow.uid ) << " flow start time: " << flow.start << " num packets: " << flow.num_packets << std::endl;
    }
}

static void print_allocation( const std::vector<Interval> allocation )
{
    for ( auto &i : allocation )
    {
        std::cout << "[" << i.start << "," << i.end << "] owned by " << uid_to_string( i.owner ) << " with sell price " << i.cost << std::endl;
    }
}

#endif /* PRETTY_PRINT_HH */

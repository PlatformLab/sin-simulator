/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ALLOCATION_ANALYSIS_HH
#define ALLOCATION_ANALYSIS_HH

#include <iostream>
#include <vector>
#include <cassert>
#include "pretty_print.hh"
#include "interval.hh"

void print_allocation( std::vector<Interval> allocation )
{
    for ( auto &i : allocation )
    {
        std::cout << "[" << i.start << "," << i.end << "] owned by " << uid_to_string( i.owner ) << " with sell price " << i.cost << std::endl;
    }
}

#endif /* ALLOCATION_ANALYSIS_HH */

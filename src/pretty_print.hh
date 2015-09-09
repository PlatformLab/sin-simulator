/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PRETTY_PRINT_HH
#define PRETTY_PRINT_HH

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "flow.hh"
#include "interval.hh"
#include "meta_interval.hh"
#include "transaction.hh"


static std::string uid_to_string( const size_t uid )
{
    if ( uid ) {
        return std::string( 1, 'A' + uid - 1 );
    } else {
        return std::string("ccast");
    }
}

static void print_transactions( const std::vector<Transaction> &transactions )
{
    for ( auto &t : transactions ) {
        std::cout << t.amount << " from " << uid_to_string( t.from ) << " to " << uid_to_string( t.to ) << std::endl;
    }
}

static void print_flows( const std::vector<Flow> &flows )
{
    for ( auto &flow : flows ) {
        std::cout << "User " << uid_to_string( flow.uid ) << " flow start time: " << flow.start << " num packets: " << flow.num_packets << std::endl;
    }
}

static void print_intervals( const std::vector<Interval> intervals )
{
    for ( auto &i : intervals ) {
        if ( i.owner != 0 ) { // XXX temp not printing ccast right now
            std::cout << "[" << i.start << "," << i.end << "] owned by " << uid_to_string( i.owner );

            if ( i.cost < std::numeric_limits<double>::max() ) {
                std::cout << " with sell price " << i.cost << std::endl;
            } else {
                std::cout << std::endl;
            }
        }
    }
}

static void print_meta_intervals( const std::vector<MetaInterval> meta_intervals )
{
    for ( auto &m : meta_intervals ) {
            std::cout << "meta interval from: " << uid_to_string( m.owner ) << " with intervals: ";

            for ( auto &i : m.intervals ) {
                std::cout << "[" << i->start << "," << i->end << " " << uid_to_string( i->owner ) << "]";
            }

            std::cout << std::endl << " and offers: ";

            for ( auto &o : m.offers ) {
                std::cout << "(" << o.first << "," << o.second << ")";
            }
            std::cout << std::endl;
        }
}

#endif /* PRETTY_PRINT_HH */

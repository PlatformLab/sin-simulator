/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PRETTY_PRINT_HH
#define PRETTY_PRINT_HH

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include "flow.hh"
#include "opportunity.hh"
#include "transaction.hh"


static std::string uid_to_string( const size_t uid )
{
    if ( uid ) {
        return std::string( 1, 'A' + uid - 1 );
    } else {
        return std::string("ccast");
    }
}

static std::string opportunity_to_string( const Opportunity &o )
{
    return std::string( "[" ) + std::to_string( o.interval.start ) + "," + std::to_string( o.interval.end ) + "] " + std::to_string( o.num_packets ) + " packets sent by " + uid_to_string( o.provider_uid );
}

__attribute__ ((unused)) static void print_transactions( const std::vector<Transaction> &transactions )
{
    for ( auto &t : transactions ) {
        std::cout << t.amount << " from " << uid_to_string( t.from ) << " to " << uid_to_string( t.to ) << std::endl;
    }
}

__attribute__ ((unused)) static void print_flows( const std::vector<Flow> &flows )
{
    for ( auto &flow : flows ) {
        std::cout << "User " << uid_to_string( flow.uid ) << " flow start time: " << flow.start << " num packets: " << flow.num_packets << std::endl;
    }
}

__attribute__ ((unused)) static void print_allocation( const std::unordered_map<Flow, std::vector<Opportunity>> &allocation )
{
    std::vector<std::pair<size_t, Opportunity>> toPrint;
    for ( auto &allocation_pair : allocation ) {
        size_t uid = allocation_pair.first.uid;
            for ( const Opportunity &o : allocation_pair.second ) {
                toPrint.emplace_back( uid, o );
            }
    }
    // sort to print
    auto compare_interval_ends = [] ( const std::pair<size_t, Opportunity> &a, const std::pair<size_t, Opportunity> &b )
    { return a.second.interval.end < b.second.interval.end; };

    std::sort( toPrint.begin(), toPrint.end(), compare_interval_ends );

    for ( auto &pair : toPrint ) {
        if ( pair.first != 0 ) { // XXX temp not printing ccast right now
            std::cout << uid_to_string( pair.first ) << ": " << opportunity_to_string( pair.second ) << std::endl;
        }
    }
}


#endif /* PRETTY_PRINT_HH */

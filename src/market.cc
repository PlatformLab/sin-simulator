/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"

using namespace std;

void Market::add_offer( Offer &offer )
{
    offers_.emplace_back( offer ); 
    increment_version();
}

std::vector<Offer> Market::offers_in_interval( Interval &i ) const
{
    std::vector<Offer> toRet;
    for ( const Offer &o : offers_ ) {
        if ( o.interval.contained_within( i ) ) {
            toRet.push_back( o );
        }
    }
    return toRet;
}

bool Market::buy_offer( size_t uid, Offer &o )
{
    for ( auto it = offers_.begin(); it != offers_.end(); ++it ) {
        //if ( (*it) == o ) { XXX
            transactions_.push_back( { o.seller_uid, uid, o.cost } );  // to, from, amount
            // transfer underlying asset?
            offers_.erase( it );
            increment_version();
            return true;
        //}
    }
    return false;
}

const std::vector<Opportunity> Market::backing_opportunities( ) const
{
    std::vector<Opportunity> toRet;
    for ( const Offer &o : offers_ ) {
        toRet.insert( toRet.end(), o.backing_opportunities.begin(), o.backing_opportunities.end() );
    }
    return toRet;
}

bool Market::empty() const
{
    // return true if all interval start times have passed
    for ( auto &o : offers_ ) {
        if ( o.interval.start >= time_ )
        {
            return false;
        }
    }
    return true;
}

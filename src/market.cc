/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"

using namespace std;

void add_offer( Offer &offer )
{
    offers_.emplace_back( offer ); 
    increment_version();
}

std::vector<Offer> offers_in_interval( Interval &i ) const
{
    std::vector<Offer> toRet;
    for ( Offer &o : offers_ ) {
        if ( o.interval.contained_within( i ) ) {
            toRet.push_back( o );
        }
    }
    return toRet;
}

bool buy_offer( size_t uid, Offer &o )
{
    for ( auto it = offers_.begin(); it != offers_.end(); offers_++ ) {
        if ( it == o ) {
            transactions_.push_back( { o.seller_uid, uid, o.cost } );  // to, from, amount
            // transfer underlying asset?
            offers.erase( it );
            increment_version();
            return true;
        }
    }
    return false;
}

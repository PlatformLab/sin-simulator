/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "pretty_print.hh"
#include "market.hh"

using namespace std;

bool Market::add_offer( Offer &offer )
{
    offers_.insert( offer ); 
    increment_version();
    return true;
}

const std::vector<Offer> Market::offers_in_interval( const size_t start, const size_t end ) const
{
    std::vector<Offer> toRet;
    for ( const Offer &o : offers_ ) {
        if ( start <= o.interval.start and end >= o.interval.end ) {
            toRet.push_back( o );
        }
    }
    return toRet;
}

bool Market::buy_offer( size_t uid, const Offer &o )
{
    auto market_offer = offers_.find( o );
    if ( market_offer != offers_.end() ) {
        transactions_.push_back( { o.seller_uid, uid, o.cost } );  // to, from, amount

        taken_offers_.push_back( move( *market_offer ) );
        offers_.erase( market_offer );
        increment_version();

        cout << "offer on interval " << interval_to_string( o.interval ) << " taken" << endl;
        return true;
    } else {
        cout << "offer on interval " << interval_to_string( o.interval ) << " not found" << endl;
        return false;
    }
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

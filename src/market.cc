/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "pretty_print.hh"
#include "market.hh"

using namespace std;

bool Market::add_offer( const Offer &offer )
{
    offers_.insert( offer ); 
    increment_version();
    return true;
}

const std::vector<Offer> Market::offers_in_interval( const size_t start, const size_t end ) const
{
    std::vector<Offer> toRet;
    for ( const Offer &o : offers_ ) {
        if ( start <= o.opportunity.interval.start and end >= o.opportunity.interval.end ) {
            toRet.push_back( o );
        }
    }
    return toRet;
}

bool Market::buy_offer( size_t uid, const Offer &o, const std::pair<Opportunity, double> * exchange_opportunity )
{
    auto market_offer = offers_.find( o );
    if ( market_offer != offers_.end() ) {
        //Credit to_owner = { };
        //Debit from_buyer = { }; TODO messages

        offers_.erase( market_offer );
        increment_version();

        cout << "offer on opportunity " << opportunity_to_string( o.opportunity ) << " taken" << endl;
        return true;
    } else {
        cout << "offer on opportunity " << opportunity_to_string( o.opportunity ) << " not found" << endl;
        return false;
    }
}

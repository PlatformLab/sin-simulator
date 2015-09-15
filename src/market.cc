/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "market.hh"

using namespace std;

const std::vector<std::shared_ptr<Opportunity>> Market::add_opportunities( const std::vector<Opportunity> &opportunties )
{
    std::vector<std::shared_ptr<Opportunity>> toRet;

    for ( const Opportunity &toAdd : opportunties ) {
        auto result_pair = opportunities_.emplace( make_shared<Opportunity>( toAdd ) );
        assert( result_pair.second );
        toRet.push_back( *result_pair.first );
    }

    increment_version();
    return toRet;
}

bool Market::add_offer( Offer &offer )
{
    // TODO double check they own backing intervals for offer
    offers_.insert( offer ); 

    increment_version();
    return true;
}

const std::vector<Offer> Market::offers_in_interval( Interval &i ) const
{
    std::vector<Offer> toRet;
    for ( const Offer &o : offers_ ) {
        if ( o.interval.contained_within( i ) ) {
            toRet.push_back( o );
        }
    }
    return toRet;
}

const std::vector<std::shared_ptr<Opportunity>> Market::buy_offer( size_t uid, Offer &o )
{
    auto market_offer = offers_.find( o );
    if ( market_offer != offers_.end() ) {
        for ( auto &market_opportunity : market_offer->backing_opportunities ) {
            // transfer underlying asset?
            auto mutableIt = opportunities_.find( market_opportunity );
            assert( mutableIt != opportunities_.end() );

            (*mutableIt)->owner = uid;
            assert( market_opportunity->owner == uid ); // shared pointer working and const version of opportunity is not a copy
        }
        transactions_.push_back( { o.seller_uid, uid, o.cost } );  // to, from, amount

        increment_version();

        return o.backing_opportunities;
    } else {
        return { };
    }
}

const std::vector<Opportunity> Market::opportunities() const
{
    std::vector<Opportunity> toRet;
    for ( auto &ptr : opportunities_ ) {
        toRet.emplace_back( *ptr );
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

/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_EVENT_HH
#define MARKET_EVENT_HH

struct PacketSent
{
    size_t owner;
    uint64_t time;

    inline bool operator==( const PacketSent& rhs ) const {
        return owner == rhs.owner and time == rhs.time;
    }
};

struct MoneyExchanged
{
    size_t from;
    size_t to;
    double amount;
    uint64_t time;
};

#endif /* MARKET_EVENT_HH */

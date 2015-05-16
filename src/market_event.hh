/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef MARKET_EVENT_HH
#define MARKET_EVENT_HH

struct PacketSent
{
    std::string owner;
    uint64_t time;

    inline bool operator==( const PacketSent& rhs ) const {
        return owner == rhs.owner and time == rhs.time;
    }
};

struct MoneyExchanged
{
    std::string from;
    std::string to;
    uint32_t amount;
    uint64_t time;

    inline bool operator==( const MoneyExchanged& rhs ) const {
        return from == rhs.from and to == rhs.to and amount == rhs.amount and time == rhs.time;
    }
};

#endif /* MARKET_EVENT_HH */

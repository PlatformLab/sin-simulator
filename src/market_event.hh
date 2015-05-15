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

/*
struct MarketEvent
{
    enum {PACKET_SENT, MONEY_EXCHANGED} type;

    // maybe make union later
    PacketSent packet_sent;
    MoneyExchanged money_exchanged;

    inline bool operator==( const MarketEvent& rhs ) const {
        if ( type != rhs.type)
            return false;
        if (type == MarketEvent::PACKET_SENT)
            return packet_sent == rhs.packet_sent;
        if (type == MarketEvent::MONEY_EXCHANGED)
            return money_exchanged == rhs.money_exchanged;
        return true;
    }
};
*/

#endif /* MARKET_EVENT_HH */

struct PacketSent
{
    std::string owner;
    uint64_t time;
}

struct MoneyExchanged
{
    std::string from;
    std::string to;
    uint32_t amount;
    uint64_t time;
}

struct MarketEvent
{
    enum {PACKET_SENT, MONEY_EXCHANGED} type;

    union {
        PacketSent packet_sent;
        MoneyExchanged money_exchanged;
    };
}

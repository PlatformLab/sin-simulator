#ifndef COMMON
#define COMMON

struct Buyer {
    std::string name;
};

// information about a slot that market gives to a buyer
struct Slot_view {
    uint64_t time;
    uint32_t current_offer;
    bool you_own;
};

struct Slot_action {
    uint64_t slot_time;
    union action {
        uint32_t offer_price;
        uint32_t buy_price;
        std::string packet_to_send;
    };
};

#endif /* COMMON */

struct Buyer {
    std::string name;
};

// information about a slot that market gives to a buyer
struct Slot_view {
    uint64_t time;
    uint32_t current_offer;
    bool you_own;
};

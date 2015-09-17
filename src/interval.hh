/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef INTERVAL_HH
#define INTERVAL_HH

struct Interval {
    size_t owner;
    size_t start;
    size_t end;
    size_t num_packets;

    bool operator==(const Interval& other) const {
        return owner == other.owner and start == other.start and end == other.end and num_packets == other.num_packets;
    }
};

namespace std {
    template <>
        struct hash<Interval>
        {
            std::size_t operator()(const Interval& i) const
            {
                using std::size_t;
                using std::hash;

                return ( hash<size_t>()(i.owner) )
                    ^ ( hash<size_t>()(i.start) + 1 )
                    ^ ( hash<size_t>()(i.end) + 2 )
                    ^ ( hash<size_t>()(i.num_packets) + 3 );
            }
        };
}

#endif /* INTERVAL_HH */

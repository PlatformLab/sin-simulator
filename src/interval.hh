/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef INTERVAL_HH
#define INTERVAL_HH

struct Interval {
    size_t start;
    size_t end;

    bool contained_within( const Interval &other ) const
    {
        return start >= other.start and end <= other.end;
    }

    bool operator==(const Interval& other) const {
        return start == other.start and end == other.end;
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

                // Compute individual hash values for first,
                // second and third and combine them using XOR
                // and bit shifting:

                return ( hash<size_t>()(i.start) ) ^ ( hash<size_t>()(i.end) << 1 );
            }
        };
}

#endif /* INTERVAL_HH */

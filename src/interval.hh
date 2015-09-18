/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef INTERVAL_HH
#define INTERVAL_HH

struct Interval {
    size_t start;
    size_t end;

    bool operator==( const Interval& other ) const {
        return start == other.start and end == other.end;
    }
};

namespace std {
    template <>
        struct hash<Interval>
        {
            std::size_t operator()( const Interval& i ) const
            {
                return ( hash<size_t>()( i.start ) )
                    ^ ( hash<size_t>()( i.end ) + 1 );
            }
        };
}

#endif /* INTERVAL_HH */

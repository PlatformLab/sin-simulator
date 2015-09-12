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

    /*
    bool operator==(const Interval& other) {
        return start == other.start and end == other.end;
    }
    */
};

#endif /* INTERVAL_HH */

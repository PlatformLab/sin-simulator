/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef FLOW_HH
#define FLOW_HH

struct Flow {
    size_t uid;
    size_t start;
    size_t num_packets;

    bool operator==( const Flow& other ) const {
        return uid == other.uid and start == other.start and num_packets == other.num_packets;
    }
};

namespace std {
    template <>
        struct hash<Flow>
        {
            std::size_t operator()(const Flow& i) const
            {
                return ( hash<size_t>()(i.uid) )
                    ^ ( hash<size_t>()(i.start) + 1 )
                    ^ ( hash<size_t>()(i.num_packets) + 2 );
            }
        };
}

#endif /* FLOW_HH */

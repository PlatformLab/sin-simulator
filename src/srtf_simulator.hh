/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SRTF_SIMULATOR_HH
#define SRTF_SIMULATOR_HH

#include <iostream>
#include <vector>
#include <list>

#include "flow.hh"
#include "link.hh"

class SRTFSimulator {
    std::vector<Link> links_;
    std::vector<Flow> flows_;

    public:
    SRTFSimulator( std::vector<Link> &links, std::vector<Flow> &flows )
    : links_( links ),
    flows_( flows )
    { }

    void run_to_completion()
    {

    }

    void print_outcome()
    {

    }
};

#endif /* SRTF_SIMULATOR_HH */

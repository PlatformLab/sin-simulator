/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PRETTY_PRINT_HH
#define PRETTY_PRINT_HH

#include <iostream>
#include <string>

std::string uid_to_string( const size_t uid )
{
    if ( uid ) {
        return std::string( 1, 'A' + uid - 1 );
    } else {
        return std::string("ccast");
    }
}

#endif /* PRETTY_PRINT_HH */

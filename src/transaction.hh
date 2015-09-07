/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef TRANSACTION_HH
#define TRANSACTION_HH

struct Transaction {
    size_t to;
    size_t from;
    double amount;
};

#endif /* TRANSACTION_HH */

/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PENDING_MESSAGE_HH
#define PENDING_MESSAGE_HH

#include "opportunity.hh"
#include "offer.hh"
#include <vector>

struct Credit
{
    Offer offer_that_got_taken; /* references ONE opportunity (the one on offer) */
    double money_recieved;
    std::vector<Opportunity> opportinuties_recieved; /* could be zero */
};

struct Debit
{
    //Bid bid_that_got_taken; /* indicates what we owe: might be all cash OR cash + opportunity */
    double money_owed;
    std::vector<Opportunity> opportinuties_recieved;
};

struct PendingMessage {
    size_t uid;
    Credit credit;
    Debit debit;
};

#endif /* PENDING_MESSAGE_HH */

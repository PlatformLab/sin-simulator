/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ScarletBot
#define ScarletBot

#include "abstract_user.hh"
#include "market.hh"

class ScarletBot: public AbstractUser
{
    public:
    ScarletBot(const std::string &name) : AbstractUser(name) { };

    void take_actions(Market& ) {
/*      const std::deque<SingleSlot> &order_book = mkt.order_book();
      const std::deque<PacketSent> &packets_sent = mkt.packets_sent();
      BidOffer new_bid(0,"Scarlet");

      for(size_t idx=0; idx<order_book.size();idx++)
      {
        const SingleSlot &ss = order_book[idx];
        add_bid_to_slot(idx, new_bid);
      }

      for(size_t idx=0; idx<packets_sent.size();idx++)
      {
        const PacketSent &ps = packets_sent[idx];
        std::cout <<  ps.time +" "+ ps.owner << std::endl;
      }
    }
*/
        std::cout <<  "I like greg" << std::endl;

    }
    void print_stats(const Market& ) const { std::cout << "noop stats" << std::endl; }
};

#endif /* NOOP_USER */

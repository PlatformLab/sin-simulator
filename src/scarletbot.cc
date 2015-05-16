#include "scarletbot.hh"
#define SCAR "Scarlet"
using namespace std;

ScarletBot::ScarletBot(const std::string &name, const size_t flow_start_time, const size_t num_packets, const size_t budget)
: AbstractUser(name),
    flow_start_time_(flow_start_time),
    num_packets_(num_packets),
    budget_(budget)
{
}

void take_actions(Market& mkt) {
  const std::deque<SingleSlot> &order_book = mkt.order_book();
  const std::deque<PacketSent> &packets_sent = mkt.packets_sent();
  /*

     for(size_t idx=0; idx<order_book.size();idx++)
     {
     const SingleSlot &ss = order_book[idx];
     mkt.add_bid_to_slot(idx, new_bid);
     }

     for(size_t idx=0; idx<packets_sent.size();idx++)
     {
     const PacketSent &ps = packets_sent[idx];
     std::cout <<  ps.time +" "+ ps.owner << std::endl;
     }
     */
  const SingleSlot &ss = order_book[idx];
  BidOffer &my_bid = determine_bid(ss);
  if(my_bid.owner == SCAR)
    mkt.add_bid_to_slot(idx,my_bid);
}

void print_stats(const Market& ) const {
  std::cout << "scarletbot status" << std::endl;
}

const struct BidOffer &determine_bid(const SingleSlot &ss)
{
  bool has_bids = ss.has_bids();
  const struct BidOffer &best_bid;
  if(has_bids)
  {
    best_bid = ss.best_bid();
  }

  if(!ss.has_offers() && !has_bids)
  {
    return BidOffer &new_bid{0, SCAR};
  }
  else if(!has_bids)
  {
    return BidOffer &new_bid{best_offer().cost, SCAR};
  }
  else if(best_bid.cost >= budget)
  {
    return BidOffer &new_bid{0, "giveup"};
  }
  else
  {
    return BidOffer &new_bid{best_bid.cost+1, SCAR};
  }
}



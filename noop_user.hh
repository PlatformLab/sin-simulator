#ifndef NOOP_USER
#define NOOP_USER

#include "abstract_user.hh"

class NoopUser : public AbstractUser
{
    public:
    struct User_actions get_actions(std::vector<struct Slot_view> &given_view) 
    {
        return {};
    }
};

#endif /* NOOP_USER */

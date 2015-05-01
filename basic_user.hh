#include <iostream>
#include "abstract_user.hh"

class BasicUser : public AbstractUser
{
    const std::string name;
    const uint32_t flow_size;
    public:
        BasicUser(std::string name, uint32_t flow_size) : name(name), flow_size(flow_size) {}

        void take_actions(struct Market& mkt)
        {
            std::cout << "I'm a basic user named " << name;
            std::cout << " with a flow of size " << flow_size << std::endl;
        }
};

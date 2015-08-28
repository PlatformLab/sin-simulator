/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ABSTRACT_USER
#define ABSTRACT_USER

struct Market;

class AbstractUser {
    public:
        const size_t uid_;

        AbstractUser(const size_t &uid) : uid_(uid) { }

        virtual void take_actions( Market& mkt ) = 0;

        virtual ~AbstractUser() = default;
};

#endif /* ABSTRACT_USER */

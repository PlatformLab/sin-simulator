/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef ABSTRACT_USER
#define ABSTRACT_USER

struct Market;

class AbstractUser {
    public:
        const size_t uid_;
        const size_t start_;

        AbstractUser( const size_t &uid, const size_t &start )
            : uid_( uid ),
            start_( start )
        { }

        virtual void take_actions( Market& mkt ) = 0;
        virtual std::vector<Opportunity> opportunities() = 0;
        virtual ~AbstractUser() = default;
};

#endif /* ABSTRACT_USER */

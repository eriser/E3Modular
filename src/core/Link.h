
#pragma once

#include <vector>
#include <string>
#include "core/Parameter.h"


namespace e3 {

    //-------------------------------------------
    // class Link
    // Represents a connection between ports.
    //-------------------------------------------

    class Link : public Parameter
    {
    public:
        bool operator==(const Link& other) const;
        bool operator==(const Link* other) const   { return operator==(*other); }
        bool operator!=(const Link& other) const   { return !(*this == other); }
        bool operator!=(const Link* other) const   { return !(this == other); }
        bool operator<(const  Link& other) const;
        bool operator>(const  Link& other) const   { return !(*this == other && *this < other); }

        uint16_t leftModule_  = 0;  // module that sends data into the link
        uint16_t rightModule_ = 0;  // module to where the link is sending. This module is the owner of the link.
        uint16_t leftPort_    = 0;  // port within the left module
        uint16_t rightPort_   = 0;  // port within the right module
    };


    //----------------------------------------------------------------
    // class LinkList
    // A container for Links
    //----------------------------------------------------------------

    class LinkList : public std::vector< Link >
    {
    public:
        void add(Link& link);
        Link& get(const Link& link);
        void update(const Link& link);
        void remove(const Link& link);
        int16_t getIndex(const Link& link);
        bool contains(const Link& link);
        bool hasLeftModule(uint16_t id);

    private:
        iterator find(const Link& link);
    };

} // namespace e3




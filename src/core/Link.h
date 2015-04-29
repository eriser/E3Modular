
#pragma once

#include <vector>
#include <string>
#include "core/Port.h"
#include "core/Parameter.h"


namespace e3 {

    //-------------------------------------------
    // class Link
    // Represents a connection between ports.
    //-------------------------------------------

    class Link : public Parameter
    {
    public:
        bool operator==(const Link& other) const
        {
            return
                other.leftModule_  == leftModule_ &&
                other.rightModule_ == rightModule_ &&
                other.leftPort_    == leftPort_ &&
                other.rightPort_   == rightPort_;
        }


        bool operator<(const Link& other) const
        {
            if (other.rightModule_ != rightModule_) return rightModule_ < other.rightModule_;
            if (other.leftModule_  != leftModule_)  return leftModule_  < other.leftModule_;
            if (other.rightPort_   != rightPort_)   return rightPort_   < other.rightPort_;
            if (other.leftPort_    != leftPort_)    return leftPort_    < other.leftPort_;

            return false;
        }

        bool operator==(const Link* other) const   { return operator==(*other); }
        bool operator!=(const Link& other) const   { return !(*this == other); }
        bool operator!=(const Link* other) const   { return !(this == other); }
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

    template <class T>
    class LinkListTemplate : public std::vector< T >
    {
    public:
        T& get(const T& link)
        {
            iterator pos = find(link);
            if (pos == end()) {
                THROW(std::out_of_range, "Link not found");
            }
            return *pos;
        }


        size_t add(const T& link)
        {
            bool linkExists = contains(link);
            ASSERT(linkExists == false);

            if (linkExists == false)  {
                push_back(link);
            }
            return size();
        }


        void replace(const T& link)
        {
            iterator pos = find(link);
            ASSERT(pos != end());

            if (pos != end()) {
                *pos = link;
            }
        }


        size_t remove(const T& link)
        {
            iterator pos = find(link);
            ASSERT(pos != end());

            if (pos != end()) {
                erase(pos);
            }
            return size();
        }


        int16_t getIndex(const T& link)
        {
            iterator pos = find(link);

            return pos == end() ? -1 : (int16_t)(pos - begin());
        }


        bool contains(const T& link)
        {
            iterator pos = find(link);
            return pos != end();
        }

        
        //void getLinksForModule(int16_t moduleId, LinkListTemplate<Link*>& list, PortType portType)
        //{
        //    for (LinkList::iterator it = begin(); it != end(); ++it)
        //    {
        //        T& link = *it;
        //        if ((portType == kInport || portType == kPortUndefined) && link.rightModule_ == moduleId) {
        //            list.add(&link);
        //        }
        //        else if ((portType == kOutport || portType == kPortUndefined) && link.leftModule_ == moduleId) {
        //            list.add(&link);
        //        }
        //    }
        //}


    private:
        iterator find(const T& link)
        {
            return std::find(begin(), end(), link);
        }
    };

    typedef LinkListTemplate<Link> LinkList;
    typedef LinkListTemplate<Link*> LinkPointerList;

} // namespace e3




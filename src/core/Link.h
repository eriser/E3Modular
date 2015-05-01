
#pragma once

#include <vector>
#include <string>
#include <sstream>
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


        bool isLeftValid() const
        {
            return ( leftModule_ >= 0 && leftPort_ >= 0 );
        }


        bool isRightValid() const
        {
            return (rightModule_ >= 0 && rightPort_ >= 0);
        }


        bool isValid() const
        {
            return isLeftValid() && isRightValid();
        }


        bool isSameModule() const
        {
            return (leftModule_ == rightModule_);
        }


        bool isSamePort() const
        {
            return isSameModule() && (leftPort_ == rightPort_);
        }


        void resetLeft()
        {
            leftModule_ = -1;
            leftPort_   = -1;
        }


        void resetRight()
        {
            rightModule_ = -1;
            rightPort_   = -1;
        }


        std::string toString() const
        {
            std::ostringstream os;
            os << "Link: (" << label_ << ") ";
            os << "leftModule:" << leftModule_ << " rightModule:" << rightModule_;
            os << " leftPort:" << leftPort_ << " rightPort:" << rightPort_;
            return os.str();
        }


        int leftModule_  = -1;  // source module
        int leftPort_    = -1;  // outport of source module
        int rightModule_ = -1;  // target module
        int rightPort_   = -1;  // inport of target module
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
            const_iterator pos = find(link);
            if (pos == end()) {
                THROW(std::out_of_range, "Link not found");
            }
            return const_cast<T&>(*pos);
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
            iterator pos = std::find( begin(), end(), link );
            ASSERT(pos != end());

            if (pos != end()) {
                *pos = link;
            }
        }


        size_t remove(const T& link)
        {
            const_iterator pos = find(link);
            ASSERT(pos != end());

            if (pos != end()) {
                erase(pos);
            }
            return size();
        }


        int16_t getIndex(const T& link)
        {
            const_iterator pos = find(link);

            return pos == end() ? -1 : (int16_t)(pos - begin());
        }


        bool contains(const T& link)
        {
            const_iterator pos = find(link);
            return pos != end();
        }


    private:
        const_iterator find(const T& link) const
        {
            return std::find(begin(), end(), link);
        }
    };

    typedef LinkListTemplate<Link> LinkList;
    typedef LinkListTemplate<Link*> LinkPointerList;

} // namespace e3





#pragma once

#include <vector>
#include <string>
#include <set>
#include <sstream>


namespace e3 {

    //-------------------------------------------
    // class Link
    // Stores the moduleId and portId of a
    // connection between module ports.
    //-------------------------------------------

    class Link  
    {
        friend class LinkSet;

    public:
        Link() {}
        Link( int id, int leftModule, int leftPort, int rightModule, int rightPort ) :
            linkId_( id ),
            leftModule_( leftModule ),
            leftPort_( leftPort ),
            rightModule_( rightModule ),
            rightPort_( rightPort )
        {}

        bool operator==(const Link& other) const
        {
            return
                other.leftModule_ == leftModule_ &&
                other.rightModule_ == rightModule_ &&
                other.leftPort_ == leftPort_ &&
                other.rightPort_ == rightPort_;
        }
        bool operator==(const Link* other) const   { return operator==(*other); }
        bool operator!=(const Link& other) const   { return !(*this == other); }
        bool operator!=(const Link* other) const   { return !(this == other); }

        bool operator<(const Link& other) const
        {
            if (other.rightModule_ != rightModule_) return rightModule_ < other.rightModule_;
            if (other.rightPort_ != rightPort_)   return rightPort_ < other.rightPort_;
            if (other.leftModule_ != leftModule_)  return leftModule_ < other.leftModule_;
            if (other.leftPort_ != leftPort_)    return leftPort_ < other.leftPort_;

            return false;
        }

        void setId( int id )  { linkId_ = id; }
        int getId() const     { return linkId_; }


        bool isLeftValid() const
        {
            return (leftModule_ >= 0 && leftPort_ >= 0);
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
            os << "leftModule:" << leftModule_ << " rightModule:" << rightModule_;
            os << " leftPort:" << leftPort_ << " rightPort:" << rightPort_;
            return os.str();
        }


        int linkId_      = -1;
        mutable int leftModule_  = -1;  // source module
        mutable int leftPort_    = -1;  // outport of source module
        mutable int rightModule_ = -1;  // target module
        mutable int rightPort_   = -1;  // inport of target module
    };


    //----------------------------------------------------------------
    // class LinkList
    // A vector container for Links
    //----------------------------------------------------------------

    typedef std::vector<Link> LinkList;

    //class LinkList : public std::vector < Link >
    //{
    //public:
    //    size_t add( const Link& link );
    //    size_t remove( const Link& link );
    //    size_t getIndex( const Link& link ) const;
    //    bool contains( const Link& link ) const;

    //private:
    //    const_iterator find( const Link& link ) const;
    //};


    //-------------------------------------------------------------------
    // class LinkSet
    // A unique container for Links
    //-------------------------------------------------------------------

    class LinkSet : public std::set < Link >
    {
    public:
        const Link& get( const Link& link ) const;
        const Link& get( int id ) const;

        size_t add( Link& link );
        size_t remove( const Link& link );
        size_t remove( int id );

        bool contains( const Link& link );
        int createUniqueId();

    private:
        const_iterator find( const Link& link ) const;
        const_iterator find( int id ) const;
    };


} // namespace e3




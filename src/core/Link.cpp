

#include <e3_Exception.h>
#include "core/Link.h"


namespace e3 {

    //----------------------------------------------------------------
    // class Link
    //----------------------------------------------------------------
    
    bool Link::operator==(const Link& other) const
    {
        return
            other.leftModule_  == leftModule_ &&
            other.rightModule_ == rightModule_ &&
            other.leftPort_    == leftPort_ &&
            other.rightPort_   == rightPort_;
    }


    bool Link::operator<(const Link& other) const
    {
        if (other.rightModule_ != rightModule_) return rightModule_ < other.rightModule_;
        if (other.leftModule_  != leftModule_) return leftModule_ < other.leftModule_;
        if (other.rightPort_   != rightPort_)  return rightPort_  < other.rightPort_;
        if (other.leftPort_    != leftPort_) return leftPort_ < other.leftPort_;

        return false;
    }


    //----------------------------------------------------------------
    // class LinkList
    //----------------------------------------------------------------

    Link& LinkList::get(const Link& link)
    {
        iterator pos = find(link);
        return *pos;
    }

    void LinkList::add(Link& link)
    {
        push_back(link);
    }


    void LinkList::update(const Link& link)
    {
        iterator pos = find(link);
        *pos = link;
    }


    void LinkList::remove(const Link& link)
    {
        iterator pos = find(link);
        erase(pos);
    }


    int16_t LinkList::getIndex(const Link& link)
    {
        iterator pos = find(link);
        return (int16_t)(pos - begin());
    }


    bool LinkList::contains(const Link& link)
    {
        iterator pos = std::find(begin(), end(), link);
        return pos != end();
    }


    bool LinkList::hasLeftModule(uint16_t id)
    {
        for (size_t i = 0; i < size(); i++)
        {
            Link& l = at(i);
            if (l.leftModule_ == id) {
                return true;
            }
        }
        return false;
    }

    LinkList::iterator LinkList::find(const Link& model)
    {
        iterator pos = std::find(begin(), end(), model);
        if (pos == end()) THROW(std::out_of_range, "Link not found: %d", model.id_);

        return pos;
    }


} // namespace e3


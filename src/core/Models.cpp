#include <e3_Exception.h>
#include "core/Models.h"


namespace e3 {

    //----------------------------------------------------------------
    // class ParameterModelMap
    //----------------------------------------------------------------

    void ParameterModelMap::add(const ParameterModel& model)
    {
        insert(std::make_pair(model.id_, model));
    }


    bool ParameterModelMap::has(const uint16_t id) const
    {
        return find(id) != end();
    }

    
    void ParameterModelMap::update(const ParameterModel& model)
    {
        at(model.id_) = model;
    }

    void ParameterModelMap::setDefaultValues()
    {
        for (auto it = begin(); it != end(); it++) 
        {
            ParameterModel& model = it->second;
            model.value_ = model.defaultValue_;
        }
    }

    //----------------------------------------------------------------
    // class LinkInfo
    //----------------------------------------------------------------

    bool LinkModel::operator==(const LinkModel& other) const
    {
        return
            other.leftModule_ == leftModule_ &&
            other.rightModule_ == rightModule_ &&
            other.leftPort_ == leftPort_ &&
            other.rightPort_  == rightPort_;
    }

    //bool LinkModel::operator==(LinkModel* other) const
    //{
    //    return operator==(*other);
    //}

    //bool LinkModel::operator!=(const LinkModel& other) const
    //{
    //    return !(*this == other);
    //}

    //bool LinkModel::operator!=(LinkModel* other) const
    //{
    //    return !(this == other);
    //}

    bool LinkModel::operator<(const LinkModel& other) const
    {
        if (other.rightModule_ != rightModule_) return rightModule_ < other.rightModule_;
        if (other.leftModule_ != leftModule_) return leftModule_ < other.leftModule_;
        if (other.rightPort_  != rightPort_)  return rightPort_  < other.rightPort_;
        if (other.leftPort_ != leftPort_) return leftPort_ < other.leftPort_;

        return false;
    }


    //bool LinkModel::operator>(const LinkModel& other) const
    //{
    //    return !(*this == other && *this < other);
    //}


    //----------------------------------------------------------------
    // class LinkModelList
    //----------------------------------------------------------------

    LinkModel& LinkModelList::get(const LinkModel& link)
    {
        iterator pos = find(link);
        return *pos;
    }

    void LinkModelList::add(LinkModel& link)
    {
        push_back(link);
    }


    void LinkModelList::update(const LinkModel& link)
    {
        iterator pos = find(link);
        *pos = link;
    }


    void LinkModelList::remove(const LinkModel& link)
    {
        iterator pos = find(link);
        erase(pos);
    }


    int16_t LinkModelList::getIndex(const LinkModel& link)
    {
        iterator pos = find(link);
        return (int16_t)(pos - begin());
    }


    bool LinkModelList::contains(const LinkModel& link)
    {
        iterator pos = std::find(begin(), end(), link);
        return pos != end();
    }


    bool LinkModelList::hasLeftModule(uint16_t id)
    {
        for (size_t i = 0; i < size(); i++)
        {
            LinkModel& l = at(i);
            if (l.leftModule_ == id) {
                return true;
            }
        }
        return false;
    }

    LinkModelList::iterator LinkModelList::find(const LinkModel& model)
    {
        iterator pos = std::find(begin(), end(), model);
        if (pos == end()) THROW(std::out_of_range, "LinkModel not found: %d", model.id_);

        return pos;
    }


    ////----------------------------------------------------------------
    //// class ModuleModel
    ////----------------------------------------------------------------

    void ModuleModel::addLink(LinkModel& link)
    {
        if (removedLinks_.contains(link))
            removedLinks_.remove(link);

        link.rightModule_ = id_;
        links_.push_back(link);
    }


    void ModuleModel::removeLink(const LinkModel& link)
    {
        links_.remove(link);

        if (removedLinks_.contains(link)) {
            removedLinks_.remove(link);
        }
        removedLinks_.push_back(link);
    }


    LinkModel& ModuleModel::getLink(uint16_t index)
    {
        return links_.at(index);
    }

} // namespace e3


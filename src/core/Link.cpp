
#include <e3_Exception.h>
#include "core/Link.h"


namespace e3 {


    ////-------------------------------------------------------------------
    //// class LinkList
    ////-------------------------------------------------------------------

    //size_t LinkList::add( const Link& link )
    //{
    //    bool exists = contains( link );
    //    ASSERT( exists == false );

    //    if (exists == false)  {
    //        push_back( link );
    //    }
    //    return size();
    //}


    //size_t LinkList::remove( const Link& link )
    //{
    //    const_iterator pos = find( link );
    //    ASSERT( pos != end() );

    //    if (pos != end()) {
    //        erase( pos );
    //    }
    //    return size();
    //}


    //size_t LinkList::getIndex( const Link& link ) const
    //{
    //    const_iterator pos = find( link );

    //    return pos == end() ? -1 : (pos - begin());
    //}


    //bool LinkList::contains( const Link& link ) const
    //{
    //    const_iterator pos = find( link );
    //    return pos != end();
    //}


    //LinkList::const_iterator LinkList::find( const Link& link ) const
    //{
    //    return std::find( begin(), end(), link );
    //}


    //-------------------------------------------------------------------
    // class LinkSet
    //-------------------------------------------------------------------

    const Link& LinkSet::get( const Link& link ) const
    {
        const_iterator pos = find( link );
        if (pos == end()) {
            THROW( std::out_of_range, "Link not found" );
        }
        return const_cast<Link&>(*pos);
    }


    const Link& LinkSet::get( int id ) const
    {
        const_iterator pos = find( id );
        if (pos == end()) {
            THROW( std::out_of_range, "No Link found with id=%d", id );
        }
        return *pos;
    }


    size_t LinkSet::add( Link& link )
    {
        if (link.linkId_ < 0) {
            link.linkId_ = createUniqueId();
        }
        auto result = insert( link );
        VERIFY( result.second != false );

        return size();
    }


    size_t LinkSet::remove( const Link& link )
    {
        const_iterator pos = find( link );
        ASSERT( pos != end() );
        if (pos != end()) {
            erase( pos );
        }
        return size();
    }


    size_t LinkSet::remove( int id )
    {
        const_iterator pos = find( id );
        ASSERT( pos != end() );
        if (pos != end()) {
            erase( pos );
        }
        return size();
    }


    bool LinkSet::contains( const Link& link )
    {
        const_iterator pos = find( link );
        return pos != end();
    }


    int LinkSet::createUniqueId()
    {
        int minId = 0;
        int maxId = size();
        int id    = minId;

        for (; id <= maxId; id++) {
            const_iterator pos = find( id );
            if (pos == end()) break;
        }
        ASSERT( id <= maxId );
        return id;
    }


    LinkSet::const_iterator LinkSet::find( const Link& link ) const
    {
        return std::find( begin(), end(), link );
    }


    LinkSet::const_iterator LinkSet::find( int id ) const
    {
        for (iterator it = begin(); it != end(); ++it)
        {
            const Link& link = *it;
            if (link.linkId_ == id) {
                return it;
            }
        }
        return end();
    }

} // namespace e3
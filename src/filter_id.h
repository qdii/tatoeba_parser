#ifndef FILTER_ID_H
#define FILTER_ID_H

#include "filter.h"
#include "sentence.h"

NAMESPACE_START

struct filterId : public filter
{
    filterId( sentence::id _id ): m_filteredId( _id ) { }
    virtual ~filterId() { }
    virtual bool parse( const sentence & _sentence ) TATO_OVERRIDE
    {
        return _sentence.getId() == m_filteredId;
    }

private:
    sentence::id m_filteredId;
};

NAMESPACE_END

#endif // FILTER_H

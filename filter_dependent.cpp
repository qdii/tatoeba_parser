#include "prec.h"
#include "filter_dependent.h"

NAMESPACE_START

int FilterDependent::addFilter(Filter & _filter)
{
    m_filterList.push_back(& _filter);
    return SUCCESS;
}

//------------------------------------------------------------

int FilterDependent::filter( const sentence & _sentence )
{
    int ret = SUCCESS;
    const FilterList::iterator end = m_filterList.end();
    
    // going through all filters till one does not match or the list is empty
    for (
        FilterList::iterator matchFilter = m_filterList.begin(); 
        ret == SUCCESS && matchFilter != end; 
        ++matchFilter
    )
    {
         ret = (*matchFilter)->filter(_sentence);
    }
    
    return ret;
}

NAMESPACE_END

#include "prec.h"
#include "selecter.h"
#include "filter.h"

NAMESPACE_START

// __________________________________________________________________________ //

int selecter::matches( const sentence & _candidate )
{
    int ret = SUCCESS;
    Filters::iterator end = m_filters.end();

    for( Filters::iterator filter = m_filters.begin();
            ret == SUCCESS && filter != end;
            ++filter )
    {
        ret = (*filter)->filter( _candidate );
    }

    return ret;
}

NAMESPACE_END

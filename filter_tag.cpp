#include "prec.h"
#include "filter_tag.h"

NAMESPACE_START

// __________________________________________________________________________ //

int FilterTag::filter( const sentence & _sentence )
{
    return _sentence.hasTag( m_tag ) == NO_SUCH_TAG ? DOES_NOT_MATCH : SUCCESS;
}

// __________________________________________________________________________ //



NAMESPACE_END

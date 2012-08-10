#include "prec.h"
#include "filter_translation.h"
#include "sentence.h"

NAMESPACE_START

// __________________________________________________________________________ //

int FilterTranslation::addFilter(Filter & _filter)
{
    m_filters.push_back(&_filter);
    return SUCCESS;
}

// __________________________________________________________________________ //

int FilterTranslation::filter(const sentence & _sentence)
{
    int ret = DOES_NOT_MATCH;
    
    const std::vector<sentence *> &                 translationList = _sentence.getAllTranslations();
    const std::vector<sentence *>::const_iterator   end = translationList.end();
    const std::vector<Filter*>::const_iterator      endFilter = m_filters.end();
    
    std::vector<sentence *>::const_iterator iter = translationList.begin();
    
    // going through all the translations
    for (; ret == DOES_NOT_MATCH && iter != end; ++iter)
    {
        const sentence * translation = *iter;
        bool passesAllFilters = true;

        // going through each filter
        std::vector<Filter*>::const_iterator filter = m_filters.begin();
        for (; passesAllFilters && filter != endFilter; ++filter)
        {
            passesAllFilters = (*filter)->filter( *translation );
        }
        
        ret = passesAllFilters == SUCCESS ? SUCCESS : DOES_NOT_MATCH;
    }
    
    return ret;
}
NAMESPACE_END

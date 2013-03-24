#ifndef FILTER_H
#define FILTER_H

#include <memory>
#include <vector>

NAMESPACE_START

struct sentence;

/**@struct filter
 * @brief Checks that a sentence against a set of criterions */
struct filter
{
    /**@brief Destructs a filter */
    virtual ~filter()
    {
    }

    /**@brief Checks a sentence
     * @return true if the sentence matches the set of criterion, false otherwise */
    virtual bool parse( const sentence & _sentence ) = 0;

    /**@brief Should the sentence be displayed */
    virtual bool postProcess( const sentence & _sentence ) { return true; }
};

typedef std::vector< std::shared_ptr< filter > > FilterVector ;

NAMESPACE_END

#endif // FILTER_H

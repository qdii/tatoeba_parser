#ifndef FILTER_H
#define FILTER_H

#include "prec.h"

NAMESPACE_START

struct sentence;

/**@struct Filter
 * @brief A Filter accepts or discards a sentence
 *
 * When the user wants a given set of sentences (like, all the sentences that
 * are in French and contains the word "babar"), he creates two Filters, one
 * for the first condition ("the sentence is in French"), and another one for
 * the second condition ("it contains the word babar"). Both Filters should
 * implement this class.
 */
struct Filter
{
    /**@brief Checks whether the passed sentence matches the condition
     * @param[in] _sentence The sentence to verify
     * @return This function returns SUCCESS if the criterion is matched,
     * DOES_NOT_MATCH otherwise, ERROR_INTERNAL if something bad happened */
    virtual int filter( const sentence & _sentence ) = 0;

    virtual ~Filter() { }
};

NAMESPACE_END
#endif //FILTER_H

#ifndef FILTER_USER_H
#define FILTER_USER_H

#include "filter.h"

NAMESPACE_START

/**@struct filterUser
 * @brief Checks that a sentence belongs to an user
 */
struct filterUser : public filter
{
    /**@brief Constructs a filterLink
     * @param[in] _linkset A list of the links of the sentences
     * @param[in] _id The id of the first sentence. */
    filterUser( const std::string & _user )
        :m_user( _user )
    {
    }

    /**@brief Checks that a sentence belongs to an user
     * @param[in] _sentence The sentence to check the author of
     * @return True if the passed sentence belongs to the user */
    bool parse( const sentence & _sentence ) throw() TATO_OVERRIDE
    {
        return _sentence.belongsTo( m_user );
    }


private:
    std::string m_user;
};
NAMESPACE_END

#endif //FILTER_USER_H

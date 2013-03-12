#ifndef FILTER_USER_H
#define FILTER_USER_H

#include "filter.h"

NAMESPACE_START

/**@struct filterUser
 * @brief Checks that a sentence belongs to an user
 */
struct filterUser : public filter
{
    /**@brief Constructs a filterUser
     * @param[in] _user The name of the user to check
     * @param[in] _orphansOnly Checks for sentences belonging to no-one.
     * @note Setting _orphansOnly to true results in the first parameter being ignored */
    explicit
    filterUser( const std::string & _user, bool _orphansOnly = false )
        :m_user( _user )
        ,m_orphansOnly( _orphansOnly )
    {
    }

    /**@brief Checks that a sentence belongs to an user
     * @param[in] _sentence The sentence to check the author of
     * @return True if the passed sentence belongs to the user */
    bool parse( const sentence & _sentence ) throw() TATO_OVERRIDE
    {
        bool ret = false;
        if ( m_orphansOnly )
        {
            ret = _sentence.belongsTo( "" ) || _sentence.belongsTo( "\\N" );
        }
        else
            ret = _sentence.belongsTo( m_user );

        return ret;
    }


private:
    std::string m_user;
    bool m_orphansOnly;
};
NAMESPACE_END

#endif //FILTER_USER_H

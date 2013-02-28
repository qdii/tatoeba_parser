#ifndef TATO_LISTSET_H
#define TATO_LISTSET_H

#include <functional> //hash
#include <string>
#include <unordered_map>
#include "sentence.h"

NAMESPACE_START

struct listset
{
    listset() = default;
    listset & operator=( listset&& ) = default;

    // a list is a group of sentence::id
    typedef std::vector< sentence::id > list;
    typedef std::vector< list > ctn;
    typedef std::vector< list >::size_type offset;
    typedef std::hash< std::string >::result_type list_hash;
    typedef std::unordered_map< list_hash, offset > offset_list;

    /**@brief Checks if a sentence belongs to a list
     * @param[in] _id An identifier for the sentence
     * @param[in] _listName The name of the list, there should not be any capital letters in.
     * @return true if the sentence is part of the list */
    bool isSentenceInList( sentence::id _id, const std::string & _listName ) const;

    /**@brief Checks if a sentence belongs to a list
     * @param[in] _id An identifier for the sentence
     * @param[in] _hash The hash of the list
     * @return true if the sentence is part of the list */
    bool isSentenceInList( sentence::id _id, list_hash _hash ) const;

    /**@brief Checks if a list exists
     * @param[in] _listName The name of the list */
    bool doesListExist( const std::string & _listName ) const;

public:
    /**@brief Inserts a sentence into a list
     * @param[in] _id the sentence id
     * @param[in] _listName The name of the list */
    void addSentenceToList( sentence::id _id, const std::string & _listName );

    /**@brief Computes a hash from the name of a list
     * @param[in] _listName the name of the list. There should not be any capital letters in.
     * @return An hash corresponding to that name */
    static list_hash computeHash( const std::string & _listName );

private:
    // returns the offset inside m_lists, or -1 if it cannot be found
    offset findOffset( list_hash ) const;
    void addNewList( list_hash );
    list & getList( const offset & );
    const list & getList( const offset & ) const ;

private:
    ctn m_lists;
    offset_list m_offsets;
};

NAMESPACE_END

#endif //TATO_LISTSET_H

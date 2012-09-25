#ifndef PARSER_H
#define PARSER_H

#include "dataset.h"

NAMESPACE_START
struct parser
{
    /**@brief Construct a parser
     * @param[in] _begin A pointer to a buffer that is to parse
     * @param[in] _end A pointer to the character right after the last character
     *            of the buffer to parse */
    explicit
    parser( char * _begin, char * _end );

    virtual ~parser() { }

    /**@brief Starts parsing the files
      *@return INVALID_ARG if the dataset has not been set, the nb of lines
               parsed otherwise */
    virtual int start() = 0;

    int setOutput( dataset & _data ) { m_dataset = &_data; return SUCCESS; }

protected:
    dataset      *      getDataset();
    char        *       getMapBegin()   { return m_mapBegin; }
    char        *       getMapEnd()     { return m_mapEnd;   }

private:
    parser( const parser & ) = delete;
    parser & operator=( const parser & ) = delete;

private:
    dataset      *      m_dataset;
    char        *       m_mapBegin;
    char        *       m_mapEnd;
};

// -------------------------------------------------------------------------- //

inline
dataset * parser::getDataset()
{
    return m_dataset;
}
NAMESPACE_END

#endif //PARSER_H

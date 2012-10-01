#ifndef PARSER_H
#define PARSER_H

#include "dataset.h"

NAMESPACE_START

template<typename iterator>
struct parser
{
    /**@brief Construct a parser
     * @param[in] _begin A pointer to a buffer that is to parse
     * @param[in] _end A pointer to the character right after the last character
     *            of the buffer to parse */
    explicit
    parser( iterator _begin, iterator _end );

    virtual ~parser() { }

    /**@brief Starts parsing the files
      *@return INVALID_ARG if the dataset has not been set, the nb of lines
               parsed otherwise */
    virtual int start() = 0;

    int setOutput( dataset & _data ) { m_dataset = &_data; return SUCCESS; }

protected:
    dataset      *      getDataset();
    iterator            getMapBegin()   { return m_mapBegin; }
    iterator            getMapEnd()     { return m_mapEnd;   }

private:
    parser( const parser & ) = delete;
    parser & operator=( const parser & ) = delete;

private:
    dataset      *      m_dataset;
    iterator            m_mapBegin;
    iterator            m_mapEnd;
};

// -------------------------------------------------------------------------- //

template<typename iterator> inline
dataset * parser<iterator>::getDataset()
{
    return m_dataset;
}

// -------------------------------------------------------------------------- //

template<typename iterator>
parser<iterator>::parser( iterator _begin, iterator _end )
    :m_dataset( nullptr )
    ,m_mapBegin( _begin )
    ,m_mapEnd( _end )
{
    qlog::trace << "parser::parser(const std::string&)\n";
}

NAMESPACE_END

#endif //PARSER_H

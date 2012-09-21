#ifndef PARSER_H
#define PARSER_H

#include "dataset.h"

NAMESPACE_START
struct fileMapper;
struct parser
{
    /**@brief Construct a parser
     * @param[in] _path The path of the file to parse */
    explicit
    parser( const fileMapper &  _map );

    virtual ~parser() { }   

    /**@brief Starts parsing the files
      *@return INVALID_ARG if the dataset has not been set, the nb of lines
               parsed otherwise */
    virtual int start() = 0;

    int setOutput( dataset & _data ) { m_dataset = &_data; return SUCCESS; }
    
protected:
    const fileMapper &  getMap();
    dataset *           getDataset();
        
private:
    parser( const parser& ) = delete;
    parser& operator=( const parser& ) = delete;
    
private:
    const fileMapper & m_fileMap;
    dataset * m_dataset;
};

// -------------------------------------------------------------------------- //

inline
const fileMapper & parser::getMap() 
{
    return m_fileMap;
}

// -------------------------------------------------------------------------- //

inline
dataset * parser::getDataset()
{
    return m_dataset;
}
NAMESPACE_END

#endif //PARSER_H

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "dataset.h"

NAMESPACE_START

/**@struct parser
 * @brief Parse a file to create a data set of sentences
 *
 * Tatoeba.org offers its database as a sentences.csv file,
 * available on this link:
 * http://tatoeba.org/files/downloads/sentences.csv
 *
 * This class parses such file to create a dataset of sentences */
struct parser
{

    /**@brief Creates a parser
     * @param[in] _sentences A tatoeba sentences.csv file
     * @param[in] _links A tatoeba links.csv file, or ""
     *
     * Creates a parser which will be linked to the file passed as a parameter
     */
    parser( const std::string & _sentences, const std::string & _links );

    /**@brief Sets the dataset
     * @param[in] _output The output dataset
     * @return SUCCESS on success
     *
     * During the parsing, every sentences that can be recognized will be added
     * to the dataset */
    int setOutput( dataset & _output );

    /**@brief Starts parsing the file
     * @return This function will return:
     * SUCCESS              on success
     * CANT_OPEN_FILE       if the file cannot be opened
     *
     * Parsing the file means going line by line and creating a sentence object
     * for whichever fits the regular expression (has a correct id, a decent
     * country code, and text), then adding that sentence to the dataset */
    int start();

private:
    std::string m_sentences, m_links;		// the path to sentences.csv and file.csv
    dataset  *	m_output;	// the dataset where we will put the parsed sentences

private:
    /**@brief Parses the sentences.csv file
       @return SUCCESS on success */
    int parseSentences(std::ifstream &);
    
    /**@brief Parses the links.csv file
     * @return SUCCESS on success */
    int parseLinks(std::ifstream &);
    
};

NAMESPACE_END

#endif //PARSER_H

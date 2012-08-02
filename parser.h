#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "dataset.h"

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
	 * @param[in] _file A tatoeba sentences.csv file
	 *
	 * Creates a parser which will be linked to the file passed as a parameter
	 */
	parser ( const std::string & _file );

	/**@brief Sets the dataset
	 * @param[in] _output The output dataset
	 * @return 0 on success
	 *
	 * During the parsing, every sentences that can be recognized will be added
	 * to the dataset */
	int setOutput ( dataset & _output );

	/**@brief Starts parsing the file
	 * @return 0 on success
	 *
	 * Parsing the file means going line by line and creating a sentence object
	 * for whichever fits the regular expression (has a correct id, a decent
	 * country code, and text), then adding that sentence to the dataset */
	int start();

private:
	std::string m_file;		// sentences.csv
	dataset * 	m_output;	// the dataset where we will put the parsed sentences

};

#endif //PARSER_H

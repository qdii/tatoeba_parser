#ifndef DATASET_H
#define DATASET_H
#include <map>
#include "sentence.h"

NAMESPACE_START

/**@brief a dataset is a sentence container */
typedef std::map<sentence::id, sentence> dataset;

NAMESPACE_END

#endif //DATASET_H

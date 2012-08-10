#ifndef DATASET_H
#define DATASET_H
#include <unordered_map>
#include "sentence.h"

NAMESPACE_START

/**@brief a dataset is a sentence container */
typedef std::unordered_map<sentence::id, sentence> dataset;

NAMESPACE_END

#endif //DATASET_H

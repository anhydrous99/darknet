#ifndef TREE_H
#define TREE_H
#include "darknet.h"

int hierarchy_top_prediction(const float *predictions, tree *hier, float thresh, int stride);
float get_hierarchy_probability(const float *x, tree *hier, int c, int stride);

#endif

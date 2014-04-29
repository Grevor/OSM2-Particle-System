#include "NeighbourTree.h"
#include "../kdtree/kdtree.h"

NeighbourTree::NeighbourTree(int dimensions) {
    tree = kd_create(dimensions);
    dim = dimensions;
}

void NeighbourTree::updateParticlePosition(int *startPosition, int *endPosition) {
    struct kdres *result;
    double x = *(startPosition);
    result = kd_nearest(tree, &x);
    kd_res_free(result);
    
    for (int i = 0; i < dim; i++) { 
        // cast it to a double (is usually safe)
        double y = *(endPosition + i);
        kd_insert(tree, &y, 0);
    }
}

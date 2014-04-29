/*
 * NeighbourTree.h
 *
 * Created on: 29 apr 2014
 *  Author: andreas
 *
 */

#ifndef NEIGHBOURTREE_H_
#define NEIGHBOURTREE_H_

#include "../kdtree/kdtree.h"

class NeighbourTree {
    private:
        struct kdtree* tree;
        int dim;
    public:
        /* This assumes particles can not have the same position */
        void updateParticlePosition(int *startPosition, int *endPosition);

        NeighbourTree(int dimensions);

        ~NeighbourTree() {};
    
};


#endif /* NEIGHBOURTREE_H_ */


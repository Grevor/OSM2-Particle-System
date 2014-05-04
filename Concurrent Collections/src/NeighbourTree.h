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
#include <ANN/ANN.h>

/* This struct is used to "carry out" our result from calling functions. */
typedef struct indexAndDist *IndexAndDist;

class NeighbourTree {
    private:
        ANNpointArray dataPts;
        ANNkd_tree* tree;
        int dim;
        int amountOfParticles;
        int free_index;
    public:
        /* This assumes particles can not have the same position */
        /* Removes the particle at startPosition, and adds a particle at endPosition */
        void updateParticlePosition(int *startPosition, int *endPosition);

        /* initializes a particle in the NeighbourTree */
        void initParticle(int position[]);

        /* Returns the pointer to an int array of size dim */
        IndexAndDist getNeighbours(int position[], int amountOfNeighbours);

        /* Prints the result */
        void printResult(IndexAndDist result);

        /* Frees a IndexAndDist struct */
        void freeResult(IndexAndDist carry);

        NeighbourTree(int dimensions, int amountParticles, int firstParticle[]);

        ~NeighbourTree(void);
    
};


#endif /* NEIGHBOURTREE_H_ */


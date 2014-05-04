#include "NeighbourTree.h"
#include <ANN/ANN.h>
#include <assert.h>

using namespace std;

NeighbourTree::~NeighbourTree(void) {
    //annDeallocPts(dataPts);
    //delete tree;
    //annClose();
}

NeighbourTree::NeighbourTree(int dimensions, int amountParticles, int firstParticle[]) {
    // index is 1 because we initialise our first entry here in the constructor
    amountOfParticles = amountParticles;
    dim = dimensions;
    free_index = 0;
    dataPts = annAllocPts(amountParticles, dimensions);
    tree = new ANNkd_tree(dataPts, amountParticles, dimensions);
}

typedef struct indexAndDist {
    int *index;
    int *dist;
    int length;
} *IndexAndDist;

void NeighbourTree::printResult(IndexAndDist result) {
    for (int i = 0; i < result->length; i++) {
        cout << "INDEX: " << result->index[i] << "\n";
        cout << "COORDS: {";
        for (int d = 0; d < dim; d++) {
            cout << *dataPts[result->index[i]];
            if (d != dim-1) {
                cout << ", ";
            }
        }
        cout << "}\n";
        cout << "DISTANCE (squared): " << result->dist[i];
        cout << "\n\n";
    }
}

IndexAndDist NeighbourTree::getNeighbours(int position[], int amountOfNeighbours) {
    //set up result arrays
    ANNidxArray nnIdx = new ANNidx[amountOfNeighbours];
    ANNdistArray dists = new ANNdist[amountOfNeighbours];
    //ANNpoint query_point = translatePoints(position, dim);
    double eps = 0;
    tree->annkSearch(
            position,
            amountOfNeighbours,             //amount of neighbours to find
            nnIdx,                          //nearest neighbours (return value)
            dists,                          //distance to each neighbour (return value)
            eps);                             //error bound (epsilon). 
    //annDeallocPt(query_point);
    IndexAndDist carry = new struct indexAndDist;
    carry->index = nnIdx;
    carry->dist = dists;
    carry->length = amountOfNeighbours;
    return carry;
}

void NeighbourTree::initParticle(int position[]) {
    for (int i = 0; i < dim; i++) {
        dataPts[free_index][i] = position[i];
    }
    free_index++;
}

void NeighbourTree::freeResult(IndexAndDist carry) {
    delete [] carry->index;
    delete [] carry->dist;
    delete carry;
}

void NeighbourTree::updateParticlePosition(int startPosition[], int endPosition[]) {
    IndexAndDist carry = getNeighbours(startPosition, 1);           // Get indices of the particles
    assert(*carry->dist == 0 /*Distance to itself is not zero*/);   // The distance is zero if it's the correct node, since it's itself.
    int *start_index = carry->index;                                // Get the index
    annDeallocPt(dataPts[*start_index]);                            // deallocate the pointer

    for (int i = 0; i < dim; i++) {
        dataPts[*start_index][i] = endPosition[i];
    }
    startPosition = endPosition;                                    // if we call it with (a, {5,5}), point a to {5,5} when odne.
    freeResult(carry);
}

#include "NeighbourTree.h"

int main(void) {
    int first[] = {60, 60};
    NeighbourTree* tree = new NeighbourTree(2, 50, first);
    for (int x = 50, y = 50; x != 1; x--,y--) {
        int a[] = {x,y};
        tree->initParticle(a);        
    }
    int sec[] = {80, 80};
    tree->initParticle(sec);
    int b[] = {10, 10};
    IndexAndDist result = tree->getNeighbours(b,11);
    tree->printResult(result);
    tree->freeResult(result);
    delete tree;
    return 0;
}

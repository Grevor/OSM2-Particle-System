#include "NeighbourTree.h"

int main(void) {
    NeighbourTree* tree = new NeighbourTree(2);
    int a[] = {2, 3};
    int b[] = {4, 5};
    int new_pos[] = {6, 7};
    tree->updateParticlePosition(a, new_pos);
    return 0;
}

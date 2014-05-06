//#include "NeighbourTree.h"

#include "NeighbourTree.h"
#include <time.h>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/foreach.hpp>
#include <boost/geometry/geometries/point.hpp>

using namespace std;

int main(void) {
    NeighbourTree* tree = new NeighbourTree();
    for (double i = 0; i < 10; i++) {
        double pos[] = {i, i};
        tree->initParticle(pos);
    }
    double q_p[] = {6, 8};
    ReturnCarry result = tree->getNeighbours(q_p, 3);
    tree->printResult(result);
    
    return 0;
}

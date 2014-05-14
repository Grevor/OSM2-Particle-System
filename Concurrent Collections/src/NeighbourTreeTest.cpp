//#include "NeighbourTree.h"

#include "NeighbourTree.h"
#include <time.h>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/foreach.hpp>
#include <boost/geometry/geometries/point.hpp>
//#include <boost/test/included/unit_test.hpp>
#include <assert.h>

#define PARTICLES 100 
#define NEIGHBOUR_SEARCHES 1
#define BOOST_TEST_MODULE MyTest

using namespace std;

int main(void) {
    NeighbourTree* tree = new NeighbourTree();
    for (double i = 0; i < PARTICLES; i++) {
        double pos[] = {i, i, i};
        tree->initParticle(pos);
    }
    double q_p[] = {6, 8, 7};
    ReturnCarry result;
    result = tree->getKNeighbours(q_p, 3);

    //ask for 3 neighbours, check we get 3 neighbours
    assert((tree->getKNeighbours(q_p, 3)->resultIterator).size() == 3);
    
    q_p[0] = 50;
    q_p[1] = 50;
    q_p[2] = 50;
    
    assert((tree->getRadiusNeighbours(q_p, 1)->resultIterator).size() == 1);

    double old_pos[] = {1, 1, 1};
    double new_pos[] = {51, 50, 50};
    tree->updateParticlePosition(old_pos, new_pos, 0);

    result = tree->getRadiusNeighbours(q_p, 5);
    tree->printResult(result);

    assert((tree->getRadiusNeighbours(q_p, 1)->resultIterator).size() == 2);

    return 0;
}

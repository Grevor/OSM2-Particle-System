#include "NeighbourTree.h"
#include <iostream>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/geometry/algorithms/distance.hpp>

using namespace std;

typedef struct returnCarry {
    std::vector<value> resultIterator;
    int distances[];
} *ReturnCarry;

NeighbourTree::~NeighbourTree(void) {
}

NeighbourTree::NeighbourTree(void) {
    // index is 1 because we initialise our first entry here in the constructor
    bgi::rtree<value, bgi::quadratic<32> > tree;
}

void NeighbourTree::printResult(ReturnCarry result) {
    std::vector<value> v = result->resultIterator;
    int *distances = result->distances;
    int i = 0;
    BOOST_FOREACH(const value& t, v) {
        std::cout << bg::wkt(t.first) << ", POINTER: " << t.second << ", DISTANCE: " << distances[i] << std::endl;
        i++;
    }
}

ReturnCarry NeighbourTree::getNeighbours(double position[], int amountOfNeighbours) {
    point query_point;
    query_point.set<0>(position[0]);
    query_point.set<1>(position[1]);
    std::vector<value> return_values;
    tree.query(bgi::nearest(query_point, amountOfNeighbours), std::back_inserter(return_values));

    ReturnCarry return_value = new struct returnCarry;

    return_value->resultIterator = return_values;

    // We want the distance betwen them as well!
    int i = 0;
    BOOST_FOREACH(const value& v, return_values) {
        return_value->distances[i] = bg::distance(v.first, query_point);
        i++;
    }
    return return_value;
}

void NeighbourTree::initParticle(double position[]) {
    initParticle(position, 0);
}

void NeighbourTree::initParticle(double position[], void *particle) {
    point *p = new point();
    p->set<0>(position[0]);
    p->set<1>(position[1]);
    tree.insert(std::make_pair(*p, particle));
}

void NeighbourTree::freeResult(void *carry) {
}

void NeighbourTree::updateParticlePosition(double startPosition[], double endPosition[]) {
}

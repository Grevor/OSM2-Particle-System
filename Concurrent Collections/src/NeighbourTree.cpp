#include "NeighbourTree.h"
#include <iostream>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/geometry/algorithms/distance.hpp>

using namespace std;

NeighbourTree::~NeighbourTree(void) {
}

NeighbourTree::NeighbourTree(void) {
    bgi::rtree<value, bgi::quadratic<32> > tree;
}

template <typename Point>
struct predicate 
{
    predicate(const Point& point_,unsigned distance_):point(point_),distance(distance_){}
    
    template <typename Value>
    bool operator()(const Value& value) const
    {
        return bg::distance(point,value.first) < distance;
    }
    
    Point point;
    unsigned distance;
};

void NeighbourTree::printResult(ReturnCarry result) {
    std::vector<value> v = result->resultIterator;
    int *distances = result->distances;
    int i = 0;
    BOOST_FOREACH(const value& t, v) {
        std::cout << bg::wkt(t.first) << ", POINTER: " << t.second << ", DISTANCE: " << distances[i] << std::endl;
        i++;
    }
}

ReturnCarry NeighbourTree::getRadiusNeighbours(double position[], double distance) {
    point qp = point(position[0], position[1], position[2]);
    predicate<point> query_point(point(qp), distance);
    std::vector<value> result = *(new std::vector<value>);
    tree.query(bgi::satisfies(query_point), std::back_inserter(result));

    ReturnCarry return_value = new struct returnCarry;
    return_value->resultIterator = result;
    return_value->distances = new int[result.size()];
    int i = 0;
    BOOST_FOREACH(const value& v, result) {
        return_value->distances[i] = bg::distance(v.first, qp);
        i++;
    }
    return return_value;
}

ReturnCarry NeighbourTree::getKNeighbours(double position[], int amountOfNeighbours) {
    point query_point;
    query_point.set<0>(position[0]);
    query_point.set<1>(position[1]);
    query_point.set<2>(position[2]);
    std::vector<value> return_values = *(new std::vector<value>);
    tree.query(bgi::nearest(query_point, amountOfNeighbours), std::back_inserter(return_values));

    ReturnCarry return_value = new struct returnCarry;

    return_value->resultIterator = return_values;
    return_value->distances = new int[amountOfNeighbours];

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
    p->set<2>(position[2]);
    tree.insert(std::make_pair(*p, particle));
}

void NeighbourTree::freeResult(ReturnCarry result) {
    delete [] result->distances;
    delete &(result->resultIterator);
}

void NeighbourTree::updateParticlePosition(double startPosition[], double endPosition[], void *particle) {
    ReturnCarry old_v = getKNeighbours(startPosition, 1);
    std::vector<value> old_iterator = old_v->resultIterator;
    BOOST_FOREACH(value const &v, old_iterator) {
        tree.remove(v);
    }
    value new_v = std::make_pair(point(endPosition[0], endPosition[1], endPosition[2]), particle);
    tree.insert(new_v);
}

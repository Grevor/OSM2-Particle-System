/*
 * NeighbourTree.h
 *
 * Created on: 29 apr 2014
 *  Author: andreas
 *
 */

#ifndef NEIGHBOURTREE_H_
#define NEIGHBOURTREE_H_

#define DIMENSIONS 2

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace bgm = boost::geometry::model;

//A point is DIMENSION amount of double.
typedef bgm::point<double, 2, bg::cs::cartesian> point;

//Each pair in the tree is:
//  * a point (used for localization)
//  * a distance (used for returning values)
//  * an arbitrary void pointer
typedef std::pair<point, void*> value;

typedef struct returnCarry *ReturnCarry;

class NeighbourTree {
    private:
        bgi::rtree<value, bgi::quadratic<32> > tree;
    public:
        /* This assumes particles can not have the same position */
        /* Removes the particle at startPosition, and adds a particle at endPosition */
        void updateParticlePosition(double *startPosition, double *endPosition);

        /* initializes a particle, represented only as a coordinate */
        void initParticle(double position[]);

        /* initializes a particle in the NeighbourTree */
        void initParticle(double position[], void *particle);

        /* Returns the pointer to an vector */
        ReturnCarry getNeighbours(double position[], int amountOfNeighbours);

        /* Prints the result */
        void printResult(ReturnCarry result);

        /* Frees a IndexAndDist struct */
        void freeResult(void *carry);

        NeighbourTree(void);

        ~NeighbourTree(void);
    
};


#endif /* NEIGHBOURTREE_H_ */


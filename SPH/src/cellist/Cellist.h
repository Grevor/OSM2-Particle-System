#ifndef CELLIST_H_
#define CELLIST_H_

#include <list>
#include <vector>
//#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>

typedef struct lockedList {
    std::list<void*> *Particles;
    boost::mutex Lock;
} *LockedList;

class Cellist {
private:
    float rowsAndColumns;
    float width;
    float height;
    LockedList outOfBounds;
    // bjoo-tee-fuuul
    std::vector<std::vector<LockedList> > matrix;
    boost::mutex getLock;
    int cellistSize;
    int outOfBoundsSize;

    bool isNeighbours(float mainPos[], float neighbourPos[]);

public:
    Cellist(int width, int height, float h);
    ~Cellist();

    int size();
    int OOBsize();

    void assertCorrectness();

    void insert(float position[], void *particle);

    void remove(float position[], void *particle);

    void move(float startPosition[], void *particle, float endPosition[]);

    bool isOutOfBounds(float position[]);

    std::list<LockedList>* get(float position[]);
};

#endif

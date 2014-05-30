#include "Cellist.h"
#include <cmath>
#include <math.h>
#include <list>
#include "../WaterParticleStruct.h"

Cellist::Cellist(int width, int height, float h) {
   this->width = width; 
   this->height = height;
   this->rowsAndColumns = h;
   outOfBounds = new struct lockedList;
   outOfBounds->Particles = new std::list<void*>;
   this->matrix = *(new std::vector<std::vector<LockedList> >);
   for (int i = 0; i < (width / h); i ++) {
       matrix.push_back(*(new std::vector<LockedList>));
       for (int j = 0; j < (height / h); j ++) {
           LockedList ll = new struct lockedList;
           ll->Particles = new std::list<void*>;
           matrix[i].push_back(ll);
       }
   }
}

Cellist::~Cellist() {
   for (int i = 0; i < (width / rowsAndColumns); i ++) {
       for (int j = 0; j < (height / rowsAndColumns); j ++) {
           delete matrix.at(i).at(j);
       }
       delete &(matrix.at(i));
   }
   delete &(matrix);
}

std::list<LockedList>* Cellist::get(float position[]) {
    std::list<LockedList> *returnList = new std::list<LockedList>;
    if (isOutOfBounds(position)) {
        outOfBounds->Lock.lock();
        returnList->push_back(outOfBounds);
        return returnList;
    }
    int x = int(position[0] / rowsAndColumns);
    int y = int(position[1] / rowsAndColumns);
    //only start grabbing adjacent buckets if no one else is
    if (getLock.try_lock()) {
        if (x > 0 && y > 0) {
            matrix.at(x-1).at(y-1)->Lock.lock();
            returnList->push_back(matrix.at(x-1).at(y-1));
        }
        if (x > 0) {
            matrix.at(x-1).at(y)->Lock.lock();
            returnList->push_back(matrix.at(x-1).at(y));
        }
        if (x > 0 && y+1 < (height / rowsAndColumns)) {
            matrix.at(x-1).at(y+1)->Lock.lock();
            returnList->push_back(matrix.at(x-1).at(y+1));
        }
        if (y > 0) {
            matrix.at(x).at(y-1)->Lock.lock();
            returnList->push_back(matrix.at(x).at(y-1));
        }
        if (y+1 < (height / rowsAndColumns)) {
            matrix.at(x).at(y+1)->Lock.lock();
            returnList->push_back(matrix.at(x).at(y+1));
        }
        if (x+1 < (width / rowsAndColumns) && y+1 < (height / rowsAndColumns)) {
            matrix.at(x+1).at(y+1)->Lock.lock();
            returnList->push_back(matrix.at(x+1).at(y+1));
        }
        if (x+1 < (width / rowsAndColumns)) {
            matrix.at(x+1).at(y)->Lock.lock();
            returnList->push_back(matrix.at(x+1).at(y));
        }
        if (x+1 < (width / rowsAndColumns) && y > 0) {
            matrix.at(x+1).at(y-1)->Lock.lock();
            returnList->push_back(matrix.at(x+1).at(y-1));
        }
        getLock.unlock();
    }
    return returnList;
}

void Cellist::insert(float position[], void *particle) {
    if (isOutOfBounds(position)) {
        outOfBoundsSize++;
        outOfBounds->Lock.lock();
        outOfBounds->Particles->push_back(particle);
        outOfBounds->Lock.unlock();
        return;
    }
    int x = int(position[0] / rowsAndColumns);
    int y = int(position[1] / rowsAndColumns);
    cellistSize++;
    matrix.at(x).at(y)->Lock.lock();
    matrix.at(x).at(y)->Particles->push_back(particle);
    matrix.at(x).at(y)->Lock.unlock();
    return;
}

bool Cellist::isNeighbours(float mainPos[], float neighbourPos[]) {
    int bucketMain[] = {int(mainPos[0] / rowsAndColumns), int(mainPos[1] / rowsAndColumns)};
    int bucketNeighbour[] = {int(neighbourPos[0] / rowsAndColumns), int(neighbourPos[1] / rowsAndColumns)};
    int x = 0;
    int y = 0;
    //if the x-axis is correct
    if (bucketMain[0]-1 == bucketNeighbour[0] || bucketMain[0] == bucketNeighbour[0] || bucketMain[0]+1 == bucketNeighbour[0]) {
        x = 1;
    }
    //if the y-axis is correct
    if (bucketMain[1]-1 == bucketNeighbour[1] || bucketMain[1] == bucketNeighbour[1] || bucketMain[1]+1 == bucketNeighbour[1]) {
        y = 1;
    }
    return x && y;
}

void Cellist::assertCorrectness() {
    std::list<void*>::iterator it;
    for (int i = 0; i < int((width / rowsAndColumns)); i ++) {
        for (int j = 0; j < int((height / rowsAndColumns)); j ++) {
            it = matrix.at(i).at(j)->Particles->begin();
            while (it != matrix.at(i).at(j)->Particles->end()) {
                //this is a segfault waiting to happen
                WaterParticle_t p = (WaterParticle_t) (*it);
                std::list<LockedList> *neighbours = get(&p->pos[0]);
                for (std::list<LockedList>::iterator Lists=neighbours->begin(); Lists!=neighbours->end(); ++Lists) {
                    for (std::list<void*>::iterator it=(*Lists)->Particles->begin(); it!=(*Lists)->Particles->end(); ++it) {
                        WaterParticle_t neighbourParticle = (WaterParticle_t) (*it);
                        if (!(isNeighbours(&p->pos[0], &neighbourParticle->pos[0]))) {
                            printf("MAIN BUCKET: %d, %d\n", i,j);
                            printf("MAIN: %lf, %lf\n", p->pos[0], p->pos[1]);
                            int x = int(neighbourParticle->pos[0] / rowsAndColumns);
                            int y = int(neighbourParticle->pos[1] / rowsAndColumns);
                            printf("NEIGHBOUR BUCKET: %d, %d\n", x,y);
                            printf("NEIGHBOUR: %lf, %lf\n", neighbourParticle->pos[0], neighbourParticle->pos[1]);
                        }
                    }
                (*Lists)->Lock.unlock();
                }
                it++;
                delete neighbours;
            }
        }
    }
}

int Cellist::size() {
    return this->cellistSize;
}

int Cellist::OOBsize() {
    return this->outOfBoundsSize;
}

void Cellist::remove(float position[], void *particle) {
    std::list<void*>::iterator it;
    if (isOutOfBounds(position)) {
        outOfBounds->Lock.lock();
        it = outOfBounds->Particles->begin();
        while (it != outOfBounds->Particles->end()) {
            if (*(it) == particle) {
                outOfBounds->Particles->erase(it);
                outOfBoundsSize--;
                break;
            }
            it++;
        }
        outOfBounds->Lock.unlock();
    }
    else {
        int idx_start_x = int(position[0] / rowsAndColumns);
        int idx_start_y = int(position[1] / rowsAndColumns);
        matrix.at(idx_start_x).at(idx_start_y)->Lock.lock();
        it = matrix.at(idx_start_x).at(idx_start_y)->Particles->begin();
        while (it != matrix.at(idx_start_x).at(idx_start_y)->Particles->end()) {
            if (*it == particle) {
                matrix.at(idx_start_x).at(idx_start_y)->Particles->erase(it);
                cellistSize--;
                break;
            }
            it++;
        }
        matrix.at(idx_start_x).at(idx_start_y)->Lock.unlock();
    }
}

bool Cellist::isOutOfBounds(float position[]) {
    return (isnan(position[0]) || isnan(position[1]) || position[0] < 0 || position[1] < 0 || position[0] > width || position[1] > height);
}

void Cellist::move(float startPosition[], void *particle, float endPosition[]) {
    remove(startPosition, particle);
    insert(endPosition, particle);
}

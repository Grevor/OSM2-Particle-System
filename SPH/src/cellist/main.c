#include "Cellist.h"
#include <list>
#include <stdio.h>

int main(void) {
    Cellist *c = new Cellist(100, 100, 10);
    float a[2] = {15,15};
    c->insert(a, 0);
    float b[2] = {5, 5};
    c->insert(b, 0);
    std::list<Point> ret = c->get(a);
    printf("%lf, %lf\n", ret.front()->pos[0], ret.front()->pos[1]);

    ret = c->get(b);
    printf("%lf, %lf\n", ret.front()->pos[0], ret.front()->pos[1]);
    float newPos[2] = {12, 12};

    c->move(a, 0, newPos);
    ret = c->get(b);
    printf("%lf, %lf\n", ret.front()->pos[0], ret.front()->pos[1]);


    return 0;
}

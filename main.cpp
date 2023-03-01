#include <vector>
#include <iostream>
#include <math.h>
#include "DCEL.cpp"

// https://stackoverflow.com/a/3487062
int find_angle(Vertex* a, Vertex* b, Vertex* c) {
    Vertex* ab = new Vertex(b->x - a->x, b->y - a->y);
    Vertex* cb = new Vertex(b->x - c->x, b->y - c->y);

    float dot = (ab->x * cb->x + ab->y * cb->y); // dot product
    float cross = (ab->x * cb->y - ab->y * cb->x); // cross product

    float alpha = atan2(cross, dot);

    return (int)floor(alpha * 180. / M_PI + 0.5);
}

int main() {

}

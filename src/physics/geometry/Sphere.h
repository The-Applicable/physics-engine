#pragma once
#include "Shape.h"

class Sphere : public Shape {
public:
    float radius;

    Sphere(float r) : Shape(SPHERE), radius(r) {}
};
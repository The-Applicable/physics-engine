#pragma once
#include "Shape.h"

class Pyramid : public Shape {
public:
    float halfWidth;
    float height;

    Pyramid(float w, float h) : Shape(PYRAMID), halfWidth(w / 2.0f), height(h) {}
};
#pragma once

enum ShapeType {
    SPHERE,
    BOX,
    PLANE,
    CYLINDER,
    PYRAMID
};

class Shape {
public:
    ShapeType type;

    Shape(ShapeType t) : type(t) {}
    virtual ~Shape() {}
};
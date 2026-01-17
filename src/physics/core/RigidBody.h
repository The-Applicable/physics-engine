#pragma once
#include "Vector3.h"
#include "../geometry/Shape.h"
#include "../geometry/Sphere.h"

class RigidBody {
public:
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;

    float inverseMass;
    float damping = 0.99f; 
    float restitution = 0.7f;

    Shape* shape;

    RigidBody(Shape* s, float x, float y, float z, float mass) : shape(s), position(x, y, z), velocity(0, 0, 0), acceleration(0, 0, 0) {
        if (mass > 0.0f) {
            inverseMass = 1.0f / mass;
        } else {
            inverseMass = 0.0f; // infinite mass, meaning a static object
        }
    }

    bool hasFiniteMass() const {
        return inverseMass > 0.0f;
    }

    void integrate(float dt) {
        if (!hasFiniteMass()) {
            return;
        }

        position += velocity * dt;

        velocity += acceleration * dt;

        velocity *= std::pow(damping, dt);
    }

    emscripten::val toJs() const {
        emscripten::val obj = emscripten::val::object();
        obj.set("pos", position.toJs());
        return obj;
    }
};
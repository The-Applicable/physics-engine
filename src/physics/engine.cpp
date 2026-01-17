#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <vector>
#include "core/Vector3.h"
#include "core/RigidBody.h"
#include "geometry/Sphere.h"

using namespace emscripten;

class PhysicsWorld {
    std::vector<RigidBody*> bodies;
    Vector3 gravity = Vector3(0, -9.81f, 0);
   
public:
    PhysicsWorld() {}

    ~PhysicsWorld() {
        for (auto body : bodies) {
            delete body->shape;
            delete body;
        }
    }

    void addSphere(float x, float y, float z, float radius, float mass) {
        Sphere* sphereShape = new Sphere(radius);
        RigidBody* body = new RigidBody(sphereShape, x, y, z, mass);
        bodies.push_back(body);
    }

    void step(float dt) {
        for (auto body : bodies) {
            if (body->hasFiniteMass()) {
                body->velocity += gravity * dt;
            }

            body->integrate(dt);

            // temp coll hacks
            if (body->position.y < 0.5f) {
                body->position.y = 0.5f;
                body->velocity.y *= -body->restitution;
            }
        }
    }

    val getBodyPosition(int index) {
        if (index >= 0 and index < bodies.size()) {
            return bodies[index]->toJs();
        }
        return val::null();
    }

    int getBodyCount() {
        return bodies.size(); 
    }

};

EMSCRIPTEN_BINDINGS(applicable_physics_engine) {
    class_<PhysicsWorld>("PhysicsWorld")
        .constructor<>()
        .function("addSphere", &PhysicsWorld::addSphere)
        .function("step", &PhysicsWorld::step)
        .function("getBodyCount", &PhysicsWorld::getBodyCount)
        .function("getBodyPosition", &PhysicsWorld::getBodyPosition);
}

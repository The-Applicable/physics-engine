#include "core/CollisionDetector.h"
#include "core/ContactResolver.h"
#include "core/RigidBody.h"
#include "core/Vector3.h"
#include "geometry/Box.h"
#include "geometry/Sphere.h"
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <vector>

using namespace emscripten;

class PhysicsWorld
{
    std::vector<RigidBody*> bodies;
    Vector3 gravity = Vector3(0, -9.81f, 0);

public:
    PhysicsWorld() {}

    ~PhysicsWorld()
    {
        for (auto body : bodies)
        {
            delete body->shape;
            delete body;
        }
    }

    void addSphere(float x, float y, float z, float radius, float mass)
    {
        Sphere* sphereShape = new Sphere(radius);
        RigidBody* body = new RigidBody(sphereShape, x, y, z, mass);
        body->friction = 0.5f;
        bodies.push_back(body);
    }

    void addBox(float x, float y, float z, float w, float h, float d, float mass)
    {
        Box* boxShape = new Box(w, h, d);
        RigidBody* body = new RigidBody(boxShape, x, y, z, mass);
        body->restitution = 0.5f;
        body->friction = 0.5f;
        bodies.push_back(body);
    }

    void setGravity(float gy) { gravity.y = gy; }

    void setRestitution(float r)
    {
        for (auto body : bodies)
        {
            body->restitution = r;
        }
    }

    void reset()
    {
        for (auto body : bodies)
        {
            delete body->shape;
            delete body;
        }
        bodies.clear();
    }

    void step(float dt)
    {
        // Sleep management (once per frame)
        for (auto body : bodies)
        {
            if (!body->hasFiniteMass()) continue;
            if (body->isAwake)
            {
                float currentMotion = body->velocity.dot(body->velocity) +
                                      body->angularVelocity.dot(body->angularVelocity);
                float bias = 0.96f;
                body->motion = bias * body->motion + (1.0f - bias) * currentMotion;
                if (body->motion < body->sleepEpsilon)
                    body->setAwake(false);
                else if (body->motion > 10.0f * body->sleepEpsilon)
                    body->motion = 10.0f * body->sleepEpsilon;
            }
        }

        const int substeps = 4;
        float subDt = dt / substeps;

        for (int sub = 0; sub < substeps; sub++)
        {
        updateInertiaTensors();
        for (auto body : bodies)
        {
            if (!body->hasFiniteMass() || !body->isAwake) continue;
            body->velocity += gravity * subDt;
            body->integrate(subDt);
        }

        for (auto body : bodies)
        {
            Contact contact;
            bool hitFloor = false;

            if (body->shape->type == SPHERE)
            {
                hitFloor = CollisionDetector::checkSpherePlane(body, 0.0f, contact);
            }
            else if (body->shape->type == BOX)
            {
                hitFloor = CollisionDetector::checkBoxPlane(body, 0.0f, contact);
            }
            else if (body->shape->type == CYLINDER)
            {
                hitFloor = CollisionDetector::checkCylinderPlane(body, 0.0f, contact);
            }

            if (hitFloor)
            {
                ContactResolver::resolve(contact);
            }
        }

        for (size_t i = 0; i < bodies.size(); i++)
        {
            for (size_t j = i + 1; j < bodies.size(); j++)
            {
                RigidBody* bodyA = bodies[i];
                RigidBody* bodyB = bodies[j];

                Contact contact;
                bool collided = false;

                if (bodyA->shape->type == SPHERE and bodyB->shape->type == SPHERE)
                {
                    collided = CollisionDetector::checkSphereSphere(bodyA, bodyB, contact);
                }
                else if (bodyA->shape->type == BOX and bodyB->shape->type == BOX)
                {
                    collided = CollisionDetector::checkBoxBox(bodyA, bodyB, contact);
                }
                else if (bodyA->shape->type == BOX and bodyB->shape->type == SPHERE)
                {
                    collided = CollisionDetector::checkBoxSphere(bodyA, bodyB, contact);
                }
                else if (bodyA->shape->type == SPHERE and bodyB->shape->type == BOX)
                {
                    collided = CollisionDetector::checkSphereBox(bodyA, bodyB, contact);
                }
                else if (bodyA->shape->type == SPHERE and bodyB->shape->type == CYLINDER)
                {
                    collided = CollisionDetector::checkSphereCylinder(bodyA, bodyB, contact);
                }
                else if (bodyA->shape->type == CYLINDER and bodyB->shape->type == SPHERE)
                {
                    collided = CollisionDetector::checkSphereCylinder(bodyB, bodyA, contact);
                }
                else if (bodyA->shape->type == CYLINDER and bodyB->shape->type == BOX)
                {
                    collided = CollisionDetector::checkCylinderBox(bodyA, bodyB, contact);
                }
                else if (bodyA->shape->type == BOX and bodyB->shape->type == CYLINDER)
                {
                    collided = CollisionDetector::checkCylinderBox(bodyB, bodyA, contact);
                }
                else if (bodyA->shape->type == CYLINDER and bodyB->shape->type == CYLINDER)
                {
                    collided = CollisionDetector::checkCylinderCylinder(bodyA, bodyB, contact);
                }

                if (collided)
                {
                    ContactResolver::resolve(contact);
                }
            }
        }
        } // end substep loop
    }

    val getBodyPosition(int index)
    {
        if (index >= 0 and index < bodies.size())
        {
            return bodies[index]->toJs();
        }
        return val::null();
    }

    int getBodyCount() { return bodies.size(); }

    void setVelocity(int index, float vx, float vy, float vz)
    {
        if (index >= 0 and index < bodies.size())
        {
            bodies[index]->velocity = Vector3(vx, vy, vz);
        }
    }

    void applyForce(int index, float fx, float fy, float fz)
    {
        if (index >= 0 and index < bodies.size())
        {
            bodies[index]->addForce(Vector3(fx, fy, fz));
        }
    }

    void setFriction(float f)
    {
        for (auto body : bodies)
        {
            body->friction = f;
        }
    }

    void updateInertiaTensors()
    {
        for (auto body : bodies)
        {
            if (body->inverseMass > 0)
            {
                Quaternion& q = body->orientation;
                Matrix3 rotMatrix;
                float xx = q.x * q.x, xy = q.x * q.y, xz = q.x * q.z, xw = q.x * q.w;
                float yy = q.y * q.y, yz = q.y * q.z, yw = q.y * q.w;
                float zz = q.z * q.z, zw = q.z * q.w;
                rotMatrix.data[0] = 1.0f - 2.0f * (yy + zz);
                rotMatrix.data[1] = 2.0f * (xy - zw);
                rotMatrix.data[2] = 2.0f * (xz + yw);
                rotMatrix.data[3] = 2.0f * (xy + zw);
                rotMatrix.data[4] = 1.0f - 2.0f * (xx + zz);
                rotMatrix.data[5] = 2.0f * (yz - xw);
                rotMatrix.data[6] = 2.0f * (xz - yw);
                rotMatrix.data[7] = 2.0f * (yz + xw);
                rotMatrix.data[8] = 1.0f - 2.0f * (xx + yy);
                Matrix3 rotT = rotMatrix.transpose();
                body->inverseInertiaTensorWorld = rotMatrix * body->inverseInertiaTensor * rotT;
            }
        }
    }

    void addCylinder(float x, float y, float z, float radius, float height, float mass)
    {
        Cylinder* cylinder = new Cylinder(radius, height);
        RigidBody* body = new RigidBody(cylinder, x, y, z, mass);
        body->friction = 0.5f;
        body->restitution = 0.5f;
        bodies.push_back(body);
    }
};

EMSCRIPTEN_BINDINGS(applicable_physics_engine)
{
    class_<PhysicsWorld>("PhysicsWorld")
        .constructor<>()
        .function("addSphere", &PhysicsWorld::addSphere)
        .function("addBox", &PhysicsWorld::addBox)
        .function("addCylinder", &PhysicsWorld::addCylinder)
        .function("setGravity", &PhysicsWorld::setGravity)
        .function("setRestitution", &PhysicsWorld::setRestitution)
        .function("step", &PhysicsWorld::step)
        .function("setFriction", &PhysicsWorld::setFriction)
        .function("setVelocity", &PhysicsWorld::setVelocity)
        .function("applyForce", &PhysicsWorld::applyForce)
        .function("reset", &PhysicsWorld::reset)
        .function("getBodyCount", &PhysicsWorld::getBodyCount)
        .function("getBodyPosition", &PhysicsWorld::getBodyPosition);
}

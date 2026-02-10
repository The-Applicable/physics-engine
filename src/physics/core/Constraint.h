#pragma once
#include "RigidBody.h"

class Constraint
{
public:
    RigidBody* bodyA;
    RigidBody* bodyB;
    Vector3 anchorA; 
    Vector3 anchorB; 
    float length;

    Constraint(RigidBody* a, RigidBody* b, float len) : bodyA(a), bodyB(b), length(len)
    {
        anchorA = Vector3(0, 0, 0);
        anchorB = Vector3(0, 0, 0);
    }

    void resolve()
    {
        Vector3 worldA = bodyA->position + bodyA->orientation.rotate(anchorA);
        Vector3 worldB = bodyB->position + bodyB->orientation.rotate(anchorB);

        Vector3 delta = worldA - worldB;
        float currentLen = delta.magnitude();
        if (currentLen == 0.0f)
            return;

        float error = currentLen - length;
        float invMassSum = bodyA->inverseMass + bodyB->inverseMass;
        if (invMassSum == 0.0f)
            return;

        Vector3 correction = delta * (error / currentLen / invMassSum);

        if (bodyA->hasFiniteMass() && bodyA->isAwake)
        {
            bodyA->position = bodyA->position - correction * bodyA->inverseMass;
            bodyA->velocity *= 0.99f; 
        }
        if (bodyB->hasFiniteMass() && bodyB->isAwake)
        {
            bodyB->position += correction * bodyB->inverseMass;
            bodyB->velocity *= 0.99f;
        }
    }
};
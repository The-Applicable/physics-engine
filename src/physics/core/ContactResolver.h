#pragma once
#include "Contact.h"
#include <cmath>
#include <algorithm>

class ContactResolver {
public:
    static void resolve(Contact& contact) {
        
        Vector3 relativeVelocity = contact.a->velocity;
        if (contact.b) {
            relativeVelocity = relativeVelocity - contact.b->velocity;
        }

        float velocityAlongNormal = relativeVelocity.dot(contact.normal);

        if (velocityAlongNormal > 0) return;

        float invMassSum = contact.a->inverseMass;
        if (contact.b) invMassSum += contact.b->inverseMass;
        if (invMassSum <= 0) return; // Both infinite mass?

        float e = contact.a->restitution;
        if (contact.b) e = std::min(e, contact.b->restitution);

        float jn = -(1 + e) * velocityAlongNormal;
        jn /= invMassSum;

        Vector3 impulse = contact.normal * jn;
        contact.a->velocity += impulse * contact.a->inverseMass;
        if (contact.b) contact.b->velocity = contact.b->velocity - impulse * contact.b->inverseMass;



        relativeVelocity = contact.a->velocity;
        if (contact.b) relativeVelocity = relativeVelocity - contact.b->velocity;

        Vector3 tangent = relativeVelocity - (contact.normal * relativeVelocity.dot(contact.normal));
        float tangentMag = tangent.magnitude();

        if (tangentMag > 0.001f) {
            // Normalize the tangent
            tangent = tangent * (1.0f / tangentMag);

            float jf = -relativeVelocity.dot(tangent);
            jf /= invMassSum;

            float mu = contact.a->friction;
            if (contact.b) mu = (mu + contact.b->friction) * 0.5f; // Average friction

            float maxFriction = mu * jn;
            
            if (std::abs(jf) > maxFriction) {
                jf = (jf > 0) ? maxFriction : -maxFriction;
            } 

            Vector3 frictionImpulse = tangent * jf;
            contact.a->velocity += frictionImpulse * contact.a->inverseMass;
            if (contact.b) contact.b->velocity = contact.b->velocity - frictionImpulse * contact.b->inverseMass;
        }

        const float percent = 0.2f;
        const float slop = 0.01f;
        float correctionMag = std::max(contact.penetration - slop, 0.0f) / invMassSum * percent;
        Vector3 correction = contact.normal * correctionMag;
        
        contact.a->position += correction * contact.a->inverseMass;
        if (contact.b) contact.b->position = contact.b->position - correction * contact.b->inverseMass;
    }
};
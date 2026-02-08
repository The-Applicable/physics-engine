#pragma once
#include "../geometry/Box.h"
#include "../geometry/Cylinder.h"
#include "../geometry/Sphere.h"
#include "Contact.h"
#include "math.h"

class CollisionDetector
{
public:
    static Vector3 toLocal(RigidBody* body, const Vector3& worldPt)
    {
        Vector3 rel = worldPt - body->position;
        Quaternion invQ = body->orientation;
        invQ.invert();
        return invQ.rotate(rel);
    }

    static Vector3 toWorld(RigidBody* body, const Vector3& localPt)
    {
        return body->position + body->orientation.rotate(localPt);
    }

    static bool checkSpherePlane(RigidBody* sphereBody, float planeY, Contact& contact)
    {
        Sphere* sphere = (Sphere*)sphereBody->shape;

        float distance = sphereBody->position.y - planeY;
        if (distance < sphere->radius)
        {
            // this is the case of collision detection
            contact.a = sphereBody;
            contact.b = nullptr; // since this is a floor

            contact.normal = Vector3(0, 1, 0);
            contact.penetration = sphere->radius - distance;
            contact.point = sphereBody->position - Vector3(0, sphere->radius, 0);

            return true;
        }
        return false;
    }

    static bool checkBoxPlane(RigidBody* boxBody, float planeY, Contact& contact)
    {
        Box* box = (Box*)boxBody->shape;

        Vector3 corners[8] = {
            Vector3(box->halfExtents.x, box->halfExtents.y, box->halfExtents.z),
            Vector3(-box->halfExtents.x, box->halfExtents.y, box->halfExtents.z),
            Vector3(box->halfExtents.x, -box->halfExtents.y, box->halfExtents.z),
            Vector3(-box->halfExtents.x, -box->halfExtents.y, box->halfExtents.z),
            Vector3(box->halfExtents.x, box->halfExtents.y, -box->halfExtents.z),
            Vector3(-box->halfExtents.x, box->halfExtents.y, -box->halfExtents.z),
            Vector3(box->halfExtents.x, -box->halfExtents.y, -box->halfExtents.z),
            Vector3(-box->halfExtents.x, -box->halfExtents.y, -box->halfExtents.z)};

        float maxPenetration = 0;
        Vector3 avgPoint(0, 0, 0);
        int contactCount = 0;

        for (int i = 0; i < 8; i++)
        {
            Vector3 worldPos = boxBody->position + boxBody->orientation.rotate(corners[i]);

            if (worldPos.y < planeY)
            {
                float pen = planeY - worldPos.y;
                if (pen > maxPenetration) maxPenetration = pen;
                avgPoint += worldPos;
                contactCount++;
            }
        }

        if (contactCount > 0)
        {
            contact.a = boxBody;
            contact.b = nullptr;
            contact.normal = Vector3(0, 1, 0);
            contact.penetration = maxPenetration;
            contact.point = avgPoint * (1.0f / contactCount);
            return true;
        }
        return false;
    }

    static bool checkSphereSphere(RigidBody* a, RigidBody* b, Contact& contact)
    {
        Sphere* sA = (Sphere*)a->shape;
        Sphere* sB = (Sphere*)b->shape;

        Vector3 midLine = a->position - b->position;
        float distance = midLine.magnitude();
        float radiusSum = sA->radius + sB->radius;

        if (distance <= radiusSum and distance > 0)
        {
            contact.a = a;
            contact.b = b;
            contact.normal = midLine * (1.0f / distance);
            contact.penetration = radiusSum - distance;
            Vector3 dir = contact.normal; // Normalized direction A->B
            contact.point = a->position + (dir * sA->radius);
            return true;
        }
        return false;
    }

    static bool checkBoxBox(RigidBody* a, RigidBody* b, Contact& contact)
    {
        Box* boxA = (Box*)a->shape;
        Box* boxB = (Box*)b->shape;

        Vector3 posA = a->position;
        Vector3 posB = b->position;

        float x_overlap = (boxA->halfExtents.x + boxB->halfExtents.x) - std::abs(posA.x - posB.x);
        if (x_overlap <= 0)
        {
            return false;
        }

        float y_overlap = (boxA->halfExtents.y + boxB->halfExtents.y) - std::abs(posA.y - posB.y);
        if (y_overlap <= 0)
        {
            return false;
        }

        float z_overlap = (boxA->halfExtents.z + boxB->halfExtents.z) - std::abs(posA.z - posB.z);
        if (z_overlap <= 0)
        {
            return false;
        }

        contact.a = a;
        contact.b = b;

        if (x_overlap < y_overlap and x_overlap < z_overlap)
        {
            contact.penetration = x_overlap;
            contact.normal = (posA.x > posB.x) ? Vector3(1, 0, 0) : Vector3(-1, 0, 0);
            contact.point = Vector3((posA.x + posB.x) * 0.5f, posA.y, posA.z);
        }
        else if (y_overlap < z_overlap)
        {
            contact.penetration = y_overlap;
            contact.normal = (posA.y > posB.y) ? Vector3(0, 1, 0) : Vector3(0, -1, 0);
            contact.point = Vector3(posA.x, (posA.y + posB.y) * 0.5f, posA.z);
        }
        else
        {
            contact.penetration = z_overlap;
            contact.normal = (posA.z > posB.z) ? Vector3(0, 0, 1) : Vector3(0, 0, -1);
            contact.point = Vector3(posA.x, posA.y, (posA.z + posB.z) * 0.5f);
        }

        return true;
    }

    static bool checkSphereBox(RigidBody* sphereBody, RigidBody* boxBody, Contact& contact)
    {
        Sphere* sphere = (Sphere*)sphereBody->shape;
        Box* box = (Box*)boxBody->shape;

        Vector3 center = sphereBody->position;
        Vector3 boxPos = boxBody->position;

        Vector3 relCenter = center - boxPos;

        Vector3 closestPoint;
        closestPoint.x = std::max(-box->halfExtents.x, std::min(relCenter.x, box->halfExtents.x));
        closestPoint.y = std::max(-box->halfExtents.y, std::min(relCenter.y, box->halfExtents.y));
        closestPoint.z = std::max(-box->halfExtents.z, std::min(relCenter.z, box->halfExtents.z));

        Vector3 distVec = relCenter - closestPoint;
        float distance = distVec.magnitude();

        if (distance < sphere->radius && distance > 0)
        {
            contact.a = sphereBody;
            contact.b = boxBody;
            contact.penetration = sphere->radius - distance;
            contact.normal = distVec * (1.0f / distance);
            contact.point = boxPos + closestPoint;

            return true;
        }
        return false;
    }

    static bool checkBoxSphere(RigidBody* boxBody, RigidBody* sphereBody, Contact& contact)
    {
        bool result = checkSphereBox(sphereBody, boxBody, contact);
        if (result)
        {
            contact.a = boxBody;
            contact.b = sphereBody;
            contact.normal = contact.normal * -1.0f;
        }
        return result;
    }

    static bool checkCylinderPlane(RigidBody* cylBody, float planeY, Contact& contact)
    {
        Cylinder* cylinder = (Cylinder*)cylBody->shape;

        const int segments = 16;
        float angleStep = (3.14159f * 2.0f) / segments;

        float maxPenetration = 0;
        Vector3 avgPoint(0, 0, 0);
        int contactCount = 0;

        for (int i = 0; i < segments; i++)
        {
            float theta = i * angleStep;
            float x = cylinder->radius * std::cos(theta);
            float z = cylinder->radius * std::sin(theta);

            // Bottom rim
            Vector3 worldBottom = cylBody->position + cylBody->orientation.rotate(Vector3(x, -cylinder->halfHeight, z));
            if (worldBottom.y < planeY)
            {
                float pen = planeY - worldBottom.y;
                if (pen > maxPenetration) maxPenetration = pen;
                avgPoint += worldBottom;
                contactCount++;
            }

            // Top rim
            Vector3 worldTop = cylBody->position + cylBody->orientation.rotate(Vector3(x, cylinder->halfHeight, z));
            if (worldTop.y < planeY)
            {
                float pen = planeY - worldTop.y;
                if (pen > maxPenetration) maxPenetration = pen;
                avgPoint += worldTop;
                contactCount++;
            }
        }

        // Cap centers
        Vector3 worldCenterBottom = cylBody->position + cylBody->orientation.rotate(Vector3(0, -cylinder->halfHeight, 0));
        if (worldCenterBottom.y < planeY)
        {
            float pen = planeY - worldCenterBottom.y;
            if (pen > maxPenetration) maxPenetration = pen;
            avgPoint += worldCenterBottom;
            contactCount++;
        }
        Vector3 worldCenterTop = cylBody->position + cylBody->orientation.rotate(Vector3(0, cylinder->halfHeight, 0));
        if (worldCenterTop.y < planeY)
        {
            float pen = planeY - worldCenterTop.y;
            if (pen > maxPenetration) maxPenetration = pen;
            avgPoint += worldCenterTop;
            contactCount++;
        }

        if (contactCount > 0)
        {
            contact.a = cylBody;
            contact.b = nullptr;
            contact.normal = Vector3(0, 1, 0);
            contact.penetration = maxPenetration;
            contact.point = avgPoint * (1.0f / contactCount);
            return true;
        }
        return false;
    }


    static Vector3 radiusAtAngle(float angle, float radius)
    {
        return Vector3(std::cos(angle) * radius, 0, std::sin(angle) * radius);
    }

    static void generateCylinderPoints(Cylinder* cylinder, std::vector<Vector3>& outPoints)
    {
        int segments = 16;
        float step = 6.28318f / segments;
        for (int i = 0; i < segments; i++)
        {
            float angle = i * step;
            float x = std::cos(angle) * cylinder->radius;
            float z = std::sin(angle) * cylinder->radius;

            outPoints.push_back(Vector3(x, cylinder->halfHeight, z));
            outPoints.push_back(Vector3(x, -cylinder->halfHeight, z));
        }
        // Cap centers
        outPoints.push_back(Vector3(0, cylinder->halfHeight, 0));
        outPoints.push_back(Vector3(0, -cylinder->halfHeight, 0));
    }

    static bool checkSphereCylinder(RigidBody* sphereBody, RigidBody* cylBody, Contact& contact)
    {
        Sphere* sphere = (Sphere*)sphereBody->shape;
        Cylinder* cylinder = (Cylinder*)cylBody->shape;

        Vector3 localSphere = toLocal(cylBody, sphereBody->position);
        float clampedY =
            std::max(-cylinder->halfHeight, std::min(localSphere.y, cylinder->halfHeight));

        Vector3 closestLocal;
        Vector3 xzVec(localSphere.x, 0, localSphere.z);
        float distXZ = xzVec.magnitude();

        if (distXZ > 0.0001f)
        {
            float clamedR = std::min(distXZ, cylinder->radius);

            if (distXZ < cylinder->radius and std::abs(localSphere.y) < cylinder->halfHeight)
            {
                float distToSide = cylinder->radius - distXZ;
                float distToTop = cylinder->halfHeight - localSphere.y;
                float distToBottom = localSphere.y - (-cylinder->halfHeight);

                if (distToSide < distToTop and distToSide < distToBottom)
                {
                    closestLocal = xzVec * (cylinder->radius / distXZ);
                    closestLocal.y = localSphere.y;
                }
                else if (distToTop < distToBottom)
                {
                    closestLocal = Vector3(localSphere.x, cylinder->halfHeight, localSphere.z);
                }
                else
                {
                    closestLocal = Vector3(localSphere.x, -cylinder->halfHeight, localSphere.z);
                }
            }
            else
            {
                float r = cylinder->radius;
                Vector3 edge = xzVec * (r / distXZ);
                closestLocal = Vector3(edge.x, clampedY, edge.z);

                if (distXZ < cylinder->radius)
                {
                    closestLocal.x = localSphere.x;
                    closestLocal.z = localSphere.z;
                }
            }
        }
        else
        {
            closestLocal = Vector3(radiusAtAngle(0, cylinder->radius).x, clampedY,
                                   radiusAtAngle(0, cylinder->radius).z);
        }

        // NOTE: This is almost correct, will make it more accurate in the future, peace!
        Vector3 worldClosest = toWorld(cylBody, closestLocal);

        Vector3 diff = sphereBody->position - worldClosest;
        float dist = diff.magnitude();

        if (dist < sphere->radius)
        {
            contact.a = sphereBody;
            contact.b = cylBody;
            contact.penetration = sphere->radius - dist;
            contact.normal = (dist > 0) ? diff * (1.0f / dist) : Vector3(0, 1, 0);
            contact.point = worldClosest;
            return true;
        }
        return false;
    }

    static bool checkCylinderBox(RigidBody* cylBody, RigidBody* boxBody, Contact& contact)
    {
        Cylinder* cylinder = (Cylinder*)cylBody->shape;
        Box* box = (Box*)boxBody->shape;

        float deepestPenetration = -1000.0f;
        Vector3 collisionPoint;
        Vector3 collisionNormal;
        bool hit = false;

        std::vector<Vector3> cylPoints;
        generateCylinderPoints(cylinder, cylPoints);

        for (const auto& localPt : cylPoints)
        {
            Vector3 worldPt = toWorld(cylBody, localPt);
            Vector3 boxLocal = toLocal(boxBody, worldPt);

            if (std::abs(boxLocal.x) < box->halfExtents.x and
                std::abs(boxLocal.y) < box->halfExtents.y and
                std::abs(boxLocal.z) < box->halfExtents.z)
            {
                float dx = box->halfExtents.x - std::abs(boxLocal.x);
                float dy = box->halfExtents.y - std::abs(boxLocal.y);
                float dz = box->halfExtents.z - std::abs(boxLocal.z);

                float pen = std::min(dx, std::min(dy, dz));
                if (pen > deepestPenetration)
                {
                    deepestPenetration = pen;
                    collisionPoint = worldPt;

                    Vector3 localNormal;
                    if (pen == dx)
                    {
                        localNormal = Vector3((boxLocal.x > 0) ? 1 : -1, 0, 0);
                    }
                    else if (pen == dy)
                    {
                        localNormal = Vector3(0, (boxLocal.y > 0) ? 1 : -1, 0);
                    }
                    else
                    {
                        localNormal = Vector3(0, 0, (boxLocal.z > 0) ? 1 : -1);
                    }

                    collisionNormal = boxBody->orientation.rotate(localNormal);
                    hit = true;
                }
            }
        }

        if (hit)
        {
            contact.a = cylBody;
            contact.b = boxBody;
            contact.penetration = deepestPenetration;
            contact.normal = collisionNormal;
            contact.point = collisionPoint;
            return true;
        }
        return false;
    }

    static bool checkCylinderCylinder(RigidBody* a, RigidBody* b, Contact& contact)
    {
        Cylinder* cylA = (Cylinder*)a->shape;
        Cylinder* cylB = (Cylinder*)b->shape;

        float deepestPenetration = -1000.0f;
        Vector3 collisionPoint;
        Vector3 collisionNormal;
        bool hit = false;

        // Test A's points against B (normal from B toward A = correct convention)
        {
            std::vector<Vector3> ptsA;
            generateCylinderPoints(cylA, ptsA);
            for (auto& lp : ptsA)
            {
                Vector3 wp = toWorld(a, lp);
                Vector3 localB = toLocal(b, wp);

                if (std::abs(localB.y) < cylB->halfHeight)
                {
                    float distSq = localB.x * localB.x + localB.z * localB.z;
                    if (distSq < cylB->radius * cylB->radius)
                    {
                        float dist = std::sqrt(distSq);
                        float penR = cylB->radius - dist;
                        float penY = cylB->halfHeight - std::abs(localB.y);
                        float pen = std::min(penR, penY);
                        if (pen > deepestPenetration)
                        {
                            deepestPenetration = pen;
                            collisionPoint = wp;
                            if (pen == penY)
                            {
                                Vector3 ln(0, (localB.y > 0) ? 1.0f : -1.0f, 0);
                                collisionNormal = b->orientation.rotate(ln);
                            }
                            else if (dist > 0.0001f)
                            {
                                Vector3 ln = Vector3(localB.x, 0, localB.z) * (1.0f / dist);
                                collisionNormal = b->orientation.rotate(ln);
                            }
                            hit = true;
                        }
                    }
                }
            }
        }

        // Test B's points against A (normal from A toward B, negate for B->A convention)
        {
            std::vector<Vector3> ptsB;
            generateCylinderPoints(cylB, ptsB);
            for (auto& lp : ptsB)
            {
                Vector3 wp = toWorld(b, lp);
                Vector3 localA = toLocal(a, wp);

                if (std::abs(localA.y) < cylA->halfHeight)
                {
                    float distSq = localA.x * localA.x + localA.z * localA.z;
                    if (distSq < cylA->radius * cylA->radius)
                    {
                        float dist = std::sqrt(distSq);
                        float penR = cylA->radius - dist;
                        float penY = cylA->halfHeight - std::abs(localA.y);
                        float pen = std::min(penR, penY);
                        if (pen > deepestPenetration)
                        {
                            deepestPenetration = pen;
                            collisionPoint = wp;
                            if (pen == penY)
                            {
                                Vector3 ln(0, (localA.y > 0) ? -1.0f : 1.0f, 0);
                                collisionNormal = a->orientation.rotate(ln);
                            }
                            else if (dist > 0.0001f)
                            {
                                Vector3 ln = Vector3(localA.x, 0, localA.z) * (-1.0f / dist);
                                collisionNormal = a->orientation.rotate(ln);
                            }
                            hit = true;
                        }
                    }
                }
            }
        }

        if (hit)
        {
            contact.a = a;
            contact.b = b;
            contact.penetration = deepestPenetration;
            contact.normal = collisionNormal;
            contact.point = collisionPoint;
            return true;
        }
        return false;
    }
};

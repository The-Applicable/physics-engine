#include <iostream>
#include "core/CollisionDetector.h"
#include "core/RigidBody.h"
#include "geometry/Cylinder.h"
#include "geometry/Box.h"

int main() {
    // Setup Cylinder
    Cylinder* cylShape = new Cylinder(1.0f, 2.0f); // Radius 1, Height 2 (HalfHeight 1)
    RigidBody* cylBody = new RigidBody(cylShape, 0, 0, 0, 1.0f);
    
    // Setup Box
    Box* boxShape = new Box(1.0f, 1.0f, 1.0f); // 2x2x2 box
    RigidBody* boxBody = new RigidBody(boxShape, 1.5f, 0, 0, 1.0f); 
    // Cylinder is at 0,0,0. Radius 1. Extends x from -1 to 1.
    // Box is at 1.5,0,0. Extents 1. Extends x from 0.5 to 2.5.
    // They should overlap in range [0.5, 1.0].

    Contact contact;
    bool result = CollisionDetector::checkCylinderBox(cylBody, boxBody, contact);
    
    if (result) {
        std::cout << "Collision DETECTED!" << std::endl;
        std::cout << "Penetration: " << contact.penetration << std::endl;
    } else {
        std::cout << "Collision NOT detected." << std::endl;
    }

    delete cylBody;
    delete cylShape;
    delete boxBody;
    delete boxShape;

    return 0;
}

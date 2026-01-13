# Physics Engine Documentation

This document outlines the mathematical models and physical principles implemented in the C++ physics engine for this project.

## 1. Particle Physics Model

The engine simulates point-mass particles moving in 3D space under the influence of gravity and collision constraints. While the particles are rendered as cubes, the physical simulation treats them as particles with a defined collision radius (half-height).

### State Variables

For each particle $i$, we define:
- Position vector: $\vec{p}_i = (x, y, z)$
- Velocity vector: $\vec{v}_i = (v_x, v_y, v_z)$

### Constants

- Gravity: $\vec{g} = (0, -9.81, 0) \, \text{m/s}^2$
- Time step: $\Delta t$ (variable, passed from the simulation loop)
- Coefficient of Restitution: $e = 0.5$ (bounciness)

## 2. Equations of Motion

The motion of the particles is governed by Newton's Second Law. In this simple simulation, the only external force is gravity.

$$ \vec{a} = \vec{g} $$

Therefore, the differential equations are:

$$ \frac{d\vec{v}}{dt} = \vec{g} $$
$$ \frac{d\vec{p}}{dt} = \vec{v} $$

## 3. Numerical Integration

To solve these equations numerically, we use the **Semi-Implicit Euler Method** (also known as Symplectic Euler). This method is chosen for its stability and energy conservation properties compared to the standard explicit Euler method.

### Integration Steps

In each simulation step of size $\Delta t$:

1.  **Update Velocity**:
    The velocity is updated based on the constant acceleration (gravity).

    $$ \vec{v}_{t+\Delta t} = \vec{v}_t + \vec{g} \cdot \Delta t $$

2.  **Update Position**:
    The position is updated using the *new* velocity. This is the "semi-implicit" part.

    $$ \vec{p}_{t+\Delta t} = \vec{p}_t + \vec{v}_{t+\Delta t} \cdot \Delta t $$

## 4. Collision Detection and Response

The simulation includes a simple collision detection with the ground plane at $y=0$.

### Ground Constraint

We assume the particle represents a cube of height 1. Thus, the collision occurs when the center of the cube falls below $y = 0.5$ (half-height).

**Condition:**
$$ p_y < 0.5 $$

### Collision Response

When a collision is detected, we perform two actions:

1.  **Positional Correction**:
    To prevent the object from sinking into the floor, we clamp the vertical position.
    $$ p_y = 0.5 $$

2.  **Velocity Reflection**:
    We invert the vertical velocity to simulate a bounce. The energy loss during the bounce is modeled by the coefficient of restitution ($e$).
    $$ v_y = -e \cdot v_y $$

    With $e = 0.5$, the object loses 50% of its vertical speed upon each impact, causing the bounces to decay over time.

## 5. Implementation Details

The core logic is implemented in C++ and compiled to WebAssembly.

```cpp
void step(float dt) {
    for (auto &particle : particles) {
        // Semi-implicit Euler integration
        particle.velocity.y += gravity * dt;

        particle.position.x += particle.velocity.x * dt;
        particle.position.y += particle.velocity.y * dt;
        particle.position.z += particle.velocity.z * dt;
        
        // Floor collision
        if (particle.position.y < 0.5) {
            particle.position.y = 0.5;
            particle.velocity.y *= -0.5;
        }
    }
}
```

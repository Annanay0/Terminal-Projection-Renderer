# 3D ASCII Terminal Renderer

A terminal renderer that projects and rotates 3D shapes to a 2D view using ASCII chars.

## Implementation Details

* **Adding Shapes:** The Code is modular so a user can write their own parametric equations for any shape, and code them in. Kinda similar to integration for bodies.
* **Rotation:** Used Rotation matrices to rotate the points by small angles.
* **Projection:** Uses X_screen = x/z and Y_screen = y/z formulae.
* **Header files used:** 
* iostream for std::cout and std::cin
* chrono for miliseconds
* thread for sleep_for()
* vector for std::vector and cmath for trigno identities

## How to Run
Compile with any cpp compiler :)

## Future Fixes

* Let users input angular speeds for x y & z axis in terminal instead of hardcoding
* Separate the logic and the shapes into different files instead of one Mega file
* cant think more

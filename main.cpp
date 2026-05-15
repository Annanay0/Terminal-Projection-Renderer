#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>

const float PI = 3.14159f;

// 1. Data Struct
struct Point3D {
    float x;
    float y;
    float z;
    char ch; // The character to draw for this point or locatn
};

// This holds all the settings for our virtual camera and screen
struct SceneConfig {
    float camera_dist = 10.0f;
    float focal_length_x = 80.0f; // Horizontal zoom + aspect correction  (or FOV vertical)
    float focal_length_y = 35.0f; // Vertical zoom (FOV horizontal)
    int screen_width = 80;
    int screen_height = 22;
    int buffer_size = 80 * 22; 
};

// 2. User Input Function
int getUserInput() {
    int choice;
    std::cout << "Select a shape to render:\n";
    std::cout << "1. Sphere\n";
    std::cout << "2. Cylinder\n";           //TODO : use list here and let user input more data (speed, starting posn etc)
    std::cout << "3. Donut\n";
    std::cout << "4. Cube\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;
    return choice;
}

// 3. Rotation Functions
Point3D rotateX(Point3D p, float angle) {
    Point3D rotated = p;
    rotated.y = p.y * std::cos(angle) - p.z * std::sin(angle);
    rotated.z = p.y * std::sin(angle) + p.z * std::cos(angle);
    return rotated;
}

Point3D rotateY(Point3D p, float angle) {
    Point3D rotated = p;
    rotated.x = p.x * std::cos(angle) + p.z * std::sin(angle);
    rotated.z = -p.x * std::sin(angle) + p.z * std::cos(angle);
    return rotated;
}

Point3D rotateZ(Point3D p, float angle) {
    Point3D rotated = p;
    rotated.x = p.x * std::cos(angle) - p.y * std::sin(angle);
    rotated.y = p.x * std::sin(angle) + p.y * std::cos(angle);
    return rotated;
}

// // 4. illumination Function (shades based on dist from camera)
// char calculateIllumination(float z_val) {
//     if (z_val <= 7.0f)
//         return "@#$&!*^."[int(z_val)];
//     return ' ';
// }=                                                              TODO: make this fn work like a swtich.

//------------------------------------------------------------------------------------------------------


// 4. Shape Generation Function
//(returns a vector of 3D points.)


// --- HOW TO ADD A NEW SHAPE ---
// 0. Write parametric eqns for your shape (duh)
// 1. Create a loop that generates x, y, and z coordinates for the shape.
// 2. Create a Point3D struct and assign p.x, p.y, p.z.
// 3. Assign a character to p.ch.
// 4. Use points.push_back(p) to save it.
std::vector<Point3D> generateShape(int choice) {
    std::vector<Point3D> points;

    if (choice == 1) { // TODO: dont forget to apply list logic here too
        float R = 3.0f;
        for (float phi = 0; phi < PI; phi += 0.01f) {
            for (float theta = 0; theta < 2 * PI; theta += 0.01f) {
                Point3D p;
                p.x = R * std::cos(theta) * std::sin(phi);
                p.y = R * std::sin(theta) * std::sin(phi);
                p.z = R * std::cos(phi);
                
                // semi circle or moon pattern
                p.ch = (std::cos(theta)>0) ? '#' : '.';  // only # when theta is 3/2 Pi to pi/2
                points.push_back(p); // Adding this point to our list
            }
        }
    } 
    
    else if (choice == 2) {
        float R=2.0f, H=5.0f;
        // float stripe_thickness=0.5f,stripe_frequency=1.0f;
        float h_theta=0;
        for(float y_height = -H/2 ; y_height <= H/2 ; y_height+=0.05f ){
            for (float theta = 0; theta < 2 * PI; theta += 0.01f) {
                Point3D p;
                p.x = R*std::cos(theta);
                p.z = R*std::sin(theta);
                p.y = y_height;

                // h_theta=(y_height + H/2) * (2*PI / H);
                //(stripe_frequency*(h_theta) <= theta +stripe_thickness && stripe_frequency*(h_theta) >= theta-stripe_thickness)
                if(p.y> -H/2 + 0.1f && p.y<H/2 -0.1f)p.ch='.';
                else p.ch='*';

                points.push_back(p);
            }
        }
    }
    else if (choice == 3) { 
        float R = 2.0f,r=1.0f;
        for (float phi = 0; phi < 2*PI; phi += 0.01f) {
            for (float theta = 0; theta < 2 * PI; theta += 0.01f) {
                Point3D p;
                p.x= (R + r*std::cos(theta))*std::cos(phi);
                p.y= r*std::sin(theta);
                p.z= (R + r*std::cos(theta))*std::sin(phi);

                p.ch = '#';
                points.push_back(p);
                
            }
        }
    }
    else if(choice == 4){
        //cube Not completed yet
        float S=4.0f;
        for (float i = -S/2; i < S/2; i+=0.03f) {
            for (float j = -S/2; j < S/2; j+=0.03f) {
                Point3D p;
                //bad code ahead :(
                p.x=i,p.y=j,p.z=S/2,p.ch='^';
                points.push_back(p);
                p.x=i,p.y=j,p.z=-S/2,p.ch='^';
                points.push_back(p);
                p.z=i,p.y=j,p.x=S/2,p.ch='*';
                points.push_back(p);
                p.z=i,p.y=j,p.x=-S/2,p.ch='*';
                points.push_back(p);
                p.x=i,p.z=j,p.y=S/2,p.ch='.';
                points.push_back(p);
                p.x=i,p.z=j,p.y=-S/2,p.ch='.';
                points.push_back(p);
            }
            
        }
        
    }

    return points;
}

// 5. Projection and Rendering:

void projectAndRender(const std::vector<Point3D>& points, SceneConfig config, float alpha, float beta,float gamma) {
    // srceen and Z-buffer arrays
    std::vector<char> screen(config.buffer_size, ' ');
    std::vector<float> z_buffer(config.buffer_size, 999.0f);

    // Rotate every point in shape
    for (Point3D p : points) {
        
        p = rotateX(p, alpha); 
        p = rotateY(p, beta);  
        p = rotateZ(p, gamma); 

        // rotated and shifted z:
        float final_z = p.z + config.camera_dist;

        // Prevent division by zero if the object passes through the camera :)
        if (final_z <= 0) continue; 

        // Projection (3D to 2D)
        float x_proj = ((p.x / final_z) * config.focal_length_x) + (config.screen_width / 2.0f);
        float y_proj = ((p.y / final_z) * config.focal_length_y) + (config.screen_height / 2.0f);

        int xs = (int)x_proj;
        int ys = (int)y_proj;

        // Check if the point is within screen boundaries:
        if (xs >= 0 && xs < config.screen_width && ys >= 0 && ys < config.screen_height) {
            int pos = xs + config.screen_width * ys;
            
            // Drawing the closest point in z buffer only:
            if (final_z < z_buffer[pos]) {
                z_buffer[pos] = final_z;
                
                //adding shape cahracter to screen buffer
                screen[pos] = p.ch; 
            }
        }
    }

    // ansi code to move cursor to top left
    std::cout << "\x1b[H";

    // Draw frame
    for (int i = 0; i < config.buffer_size; i++) {

        std::cout << screen[i];

        if ((i + 1) % config.screen_width == 0) {
            std::cout << '\n';
        }
    }
}



int main() {
    SceneConfig config;
    int shapeChoice = getUserInput();
    
    // Generate the points once, before the loop starts.
    std::vector<Point3D> shapePoints = generateShape(shapeChoice);

    float alpha = PI/2; 
    float beta = 0.0f;  // default or starting values.
    float gamma = 0.0f;
    // Clean terminal and hide cursor 
    std::cout << "\x1b[2J\x1b[?25l"; 

    while (true) {
        projectAndRender(shapePoints, config, alpha, beta, gamma);

        // Increment rotation angles to create the spinning animation
        alpha += 0.05f; 
        beta += 0.05f;
        //gamma +=0.05f;

        // Modern way to pause execution (Replaces the volatile loop)
        // Pauses for 30 milliseconds (~33 frames per second)
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    return 0;
}
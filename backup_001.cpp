#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "AE2D.h"

class Vec3 {
public:
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float xyz) : x(xyz), y(xyz), z(xyz) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    float getLength() const {
        return sqrtf((x * x) + (y * y) + (z * z));
    }
    void normalize() {
        float s = 1.0f / getLength();
        x = x * s;
        y = y * s;
        z = z * s;
    }
    float dotProduct(const Vec3 &other) const{
        return other.x*x + other.y*y + other.z*z;
    }

    Vec3 operator +(const Vec3 &other) const {
        return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
    }
    Vec3 operator -(const Vec3 &other) const {
        return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
    }
    Vec3 operator *(float scale) const {
        return Vec3(this->x * scale, this->y * scale, this->z * scale);
    }
    Vec3 operator -() const {
        return Vec3(-this->x, -this->y, -this->z);
    }
    friend Vec3 operator *(float scale, const Vec3 &other) {
        return Vec3(other.x * scale, other.y * scale, other.z * scale);
    }
    Vec3 copy() const {
        return *this;
    }
};

class Ray {
private:
    Vec3 pos;
    Vec3 dir;
public:
    Ray() : pos(Vec3()), dir(Vec3()) {}
    Ray(Vec3 pos, Vec3 dir) : pos(pos), dir(dir) {}

    Vec3 getPos() const{
        return pos;
    }
    Vec3 getDir() const{
        return dir;
    }
};

class Material {
private:
    Vec3 color;
    float roughness;
public:
    Material(Vec3 color, float roughness) : color(color), roughness(roughness) {}

    float getRoughness() {
        return roughness;
    }
    Vec3 &getColor() {
        return color;
    }
};

class Ball {
private:
    Vec3 pos;
    Material material;
    float radius;
public:
    Ball(Vec3 pos, Material material, float radius) :
        pos(pos), material(material), radius(radius) {}
    
    Vec3 &getPos() {
        return pos;
    }
    float getRadius() {
        return radius;
    }
    Material &getMaterial(){
        return material;
    }
    bool intersect(const Ray &in, float &distance) {
        Vec3 L = in.getPos() - pos;
        Vec3 dir = in.getDir().copy();
        dir.normalize();
        float a = dir.dotProduct(dir);
        float b = 2.0*dir.dotProduct(L);
        float c = L.dotProduct(L) - radius*radius;

        float discriminant = b*b - 4*a*c;
        if(discriminant <= 0.0f) return false;

        float root = sqrt(discriminant);
        float t0 = (-b + root)/(2*a);
        float t1 = (-b - root)/(2*a);
        float t = (t0 < t1) ? t0 : t1;

        if(t<0) return false;
        distance = t;
        return true;
    }
    Vec3 getNormal(const Vec3 &point) {
        Vec3 normal = point - pos;
        normal.normalize();
        return normal;
    }
    Vec3 getMirrored(const Vec3 &dir, const Vec3 &point) {
        Vec3 in = (-1)*(dir.copy());
        in.normalize();
        Vec3 normal = this->getNormal(point);
        Vec3 projection = normal.dotProduct(in)*normal;
        Vec3 mirrored = projection + projection - in;
        return mirrored;
    }
};

class Light {
private:
    Vec3 pos;
    Vec3 color;
    float brightness;
public:
    Light(Vec3 pos) :
        pos(pos), color(Vec3(1.0f)), brightness(1.0f) {}
    Light(Vec3 pos, Vec3 color, float brightness) :
        pos(pos), color(color), brightness(brightness) {}

    const Vec3 &getPos() const{
        return pos;
    }
    const Vec3 &getColor() const{
        return color;
    }
    void rotate() {
        // Rotation matrix
        float cosalpha = cosf(0.05f);
        float sinalpha = sinf(0.05f);
        float temp_x = pos.x;
        float temp_z = pos.z;
        pos.x = temp_x*cosalpha + temp_z*sinalpha;
        pos.z = -temp_x*sinalpha + temp_z*cosalpha;
    }
};

class Camera {
private:
    Vec3 pos;
    Vec3 dir;
    float fov;
public:
    Camera(Vec3 pos, Vec3 dir, float fov) :
        pos(pos), dir(dir), fov(fov) {}

    const Vec3 &getPos() const{
        return pos;
    }
    Vec3 &getDir() {
        return dir;
    }
    float getFov() {
        return fov;
    }
    void move(Vec3 amount) {
        return;
        this->pos = this->pos - amount;
    }
};

class Scene {
private:
    std::vector<Ball> balls;
    std::vector<Light> lights;
    Camera camera;
public:
    Scene(Camera camera) : camera(camera) {}

    const std::vector<Ball> &getBalls() const {
        return balls;
    }
    void addBall(Ball ball) {
        balls.push_back(ball);
    }
    const std::vector<Light> &getLights() const {
        return lights;
    }
    void addLight(Light light) {
        lights.push_back(light);
    }
    const Camera &getCamera() const {
        return camera;
    }
    void update() {
        for(Light &light : lights) {
            light.rotate();
        }
        this->camera.move(Vec3(0.0f, 0.0f, 0.05f));
    }
};

Scene setupScene() {
    float fov = 45.0f;
    int ballsmax = 10;
    Camera camera = Camera(Vec3(),Vec3(0.0f, 0.0f, 1.0f),fov);
    Scene scene = Scene(camera);

    // Create balls
    float k = 1.0f / (static_cast <float> (RAND_MAX));
    float x, y, z, r, g, b, radius;
    for(int i = 0; i < ballsmax; i++) {
        x = (static_cast <float> (rand()) * k * 20) - 10;
        y = (static_cast <float> (rand()) * k * 5) - 2.5;
        z = (static_cast <float> (rand()) * k * 20) - 10;
        r = (static_cast <float> (rand()) * k);
        g = (static_cast <float> (rand()) * k);
        b = (static_cast <float> (rand()) * k);
        radius = (static_cast <float> (rand()) * k * 2);

        Vec3 color = Vec3(r, g, b);
        Material material = Material(color, 1.0f);
        Vec3 pos = Vec3(x, y, z);
        Ball ball = Ball(pos,material,radius);
        scene.addBall(ball);
    }

    // Red ball
    Vec3 ball_color = Vec3(0.9f, 0.2f, 0.2f);
    Material material = Material(ball_color, 1.0f);
    Vec3 ball_pos = Vec3(4.0f, 1.0f, 8.0f);
    Ball ball = Ball(ball_pos,material,1.0f);
    scene.addBall(ball);

    // Green ball
    ball_color = Vec3(0.3f, 0.9f, 0.4f);
    material = Material(ball_color, 1.0f);
    ball_pos = Vec3(7.0f, 4.0f, 21.0f);
    ball = Ball(ball_pos,material,10.0f);
    scene.addBall(ball);

    // Blue ball
    ball_color = Vec3(0.2f, 0.2f, 0.9f);
    material = Material(ball_color, 1.0f);
    ball_pos = Vec3(50.0f, -1.0f, 00.0f);
    ball = Ball(ball_pos,material,4.0f);
    scene.addBall(ball);

    // Create lighting
    Vec3 pos = Vec3(100.0f, 110.0f, 200.0f);
    Vec3 color = Vec3(1.0f);
    float brightness = 1.0f;
    Light light = Light(pos, color, brightness);
    scene.addLight(light);

    return scene;
}

std::vector<Vec3> computeRayDirs(int w, int h, Camera camera) {
    std::vector<Vec3> ray_dirs;
    float fov = camera.getFov();
    Vec3 dir = camera.getDir();
    float z = h/tanf(fov/180*M_PI)*0.5f;

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            Vec3 dir = Vec3((float)x+0.5f-w*0.5f, -((float)y+0.5f-h*0.5f), z);
            dir.normalize();

            Vec3 ray_dir = dir;
            ray_dirs.push_back(ray_dir);
        }
    }
    return ray_dirs;
}

void rotateRayDirections(std::vector<Vec3> &ray_dirs, float theta) {
    float cosalpha = cosf(theta);
    float sinalpha = sinf(theta);
    for(Vec3 &dir : ray_dirs) {
        float temp_x = dir.x;
        float temp_z = dir.z;
        float x = temp_x*cosalpha + temp_z*sinalpha;
        float z = -temp_x*sinalpha + temp_z*cosalpha;
        float y = dir.y;
        dir = Vec3(x, y, z);
    }
}

Vec3 computeBackground(const Ray &ray, const Scene &scene) {
    Vec3 bg = Vec3(0.05f);
    Vec3 bg_light = Vec3(0.0f); 
    Vec3 dir = ray.getDir().copy();
    dir.normalize();
    float dot = 0.0f;
    
    std::vector<Light> lights = scene.getLights();
    for(Light &light : lights) {
        Vec3 light_dir = ray.getPos() - light.getPos();
        light_dir.normalize();
        float dot_product = light_dir.dotProduct(-dir);
        if(dot_product > dot) 
            dot = dot_product;
    }

    if(dot > 0.991f) {
        dot = (dot - 0.991)*130;
        dot = powf(dot,8);
        bg_light = Vec3(fmin(dot,1.0f),fmin(dot,1.0f),fmin(dot,1.0f));
    }
    return Vec3(fmin(bg.x+bg_light.x, 1.0f));
    
}

bool checkShadow(const Scene &scene, const Light &light, const Vec3 &pos, Ball* ball){
    //Look for a shadow made by other balls
    std::vector<Ball> balls = scene.getBalls();
    for(Ball &b : balls){
        float distance;
        Vec3 dir = light.getPos() - pos;

        /*
        //Optimization Huom: ota huomioon säteen lähtöpiste!
        float radius = b.getRadius();
        Vec3 center = ball->getPos();
        Vec3 c = b.getPos();
        //Decrease amount of the balls by checking the coordinates
        if(dir.y > 0 && (c.y + radius - center.y < 0)) 
            continue; //The ball is below the point and sun is up
        if(dir.y < 0 && (c.y - radius - center.y > 0)) 
            continue; //The ball is on top of the point and sun is down
        if(dir.x > 0 && (c.x + radius - center.x < 0)) 
            continue;
        if(dir.x < 0 && (c.x - radius - center.x > 0)) 
            continue;
        if(dir.z > 0 && (c.z + radius - center.z < 0)) 
            continue;
        if(dir.z < 0 && (c.z - radius - center.z > 0)) 
            continue;
        //Optimization ends 
        */

        //A ray from the ball to the light
        if(b.intersect(Ray(pos, dir), distance)) {
            return true;
        }
    }
    return false;
}


void computeBrightness(const Ray &ray, const Scene &scene, const Ray &normal_ray, Ball* ball, float &specular, float& diffuce) {
    Vec3 normal     = normal_ray.getDir();
    Vec3 pos        = normal_ray.getPos();
    Vec3 dir        = ray.getDir();
    Vec3 mirrored   = ball->getMirrored(dir, pos);
    
    diffuce = 0.0f;
    specular = 0.0f;
    
    std::vector<Light> lights = scene.getLights();
    for(Light &light : lights) {
        // Shadow
        if(checkShadow(scene, light, pos, ball)) continue;

        // Diffuce light
        Vec3 light_dir = light.getPos() - pos;
        light_dir.normalize();
        diffuce =+ normal.dotProduct(light_dir);

        // Specular light
        specular =+ mirrored.dotProduct(light_dir);
    }
}

Vec3 trace(const Ray &ray, const Scene &scene, int bounces) {
    Ball* ball = NULL;
    Ray normal_ray;

    // Find closest intersecting ball
    float closest_distance = -1;
    std::vector<Ball> balls = scene.getBalls();
    for(Ball& b : balls) {
        float distance;
        if(b.intersect(ray, distance)) {
            if(distance < closest_distance || ball == NULL) {
                closest_distance = distance;
                ball = &b;
                Vec3 point = distance*(ray.getDir()) + ray.getPos();
                normal_ray = Ray(point, ball->getNormal(point));
            }
        }
    }

    // No ball was found -> Draw background
    if(ball == NULL) {
        return computeBackground(ray, scene);
    }

    float specular, diffuce;
    computeBrightness(ray, scene, normal_ray, ball, specular, diffuce);
    // Draw color of the point
    Vec3 ball_color = 0.25f*(ball->getMaterial().getColor());

    Vec3 pixel = ball_color + ball_color*fmax(diffuce,0.0f) + ball_color*fmax(powf(specular,15), 0.0f);

    if(bounces < 2) {
        bounces++;
        Ray new_ray = Ray(normal_ray.getPos(), (ball->getMirrored(ray.getDir(), normal_ray.getPos())));
        pixel = 0.5f*pixel + 0.5f*trace(new_ray, scene, bounces);
    }
    return pixel;
}


int main(int argc, char* argv[]) {
    int width = 640;
    int height = 480;

    // Give seed for the randomizer
    if(argc > 1)
        srand(atoi(argv[1]));
    else
        srand(time(NULL));

    // Create display
    AE_Display* display = new AE_Display();
    if(!display->createWindow("Raytracer",width,height))
        return -1;

    Scene scene = setupScene();
    std::vector<Vec3> ray_dirs = computeRayDirs(width, height, scene.getCamera());

    while(!display->closeRequested()) {
        display->pollEvents();
        Vec3 camera_pos = scene.getCamera().getPos();
        for(int x = 0; x < width; x++) {
            for(int y = 0; y < height; y++) {
                Ray ray = Ray(camera_pos, ray_dirs[y*width + x]);//).getDir(), scene.getCamera().getPos());
                Vec3 c = trace(ray, scene, 0);
                uint32_t color = (uint8_t)(c.x*255.0f) << 16 | (uint8_t)(c.y*255.0f) << 8 | (uint8_t)(c.z*255.0f);
                display->setPixel(x, y, color);
            }
        }
        display->update();
        rotateRayDirections(ray_dirs, -0.01f);                
        scene.update();
    }
    display->closeWindow();
    return 0;
}
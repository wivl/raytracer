#include <_types/_uint8_t.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>
#include <chrono>
#include <cmath>

#include <ppmpp.hpp>
#include <tiny_obj_loader.h>
#include <Eigen/Dense>
#include "camera.hpp"
#include "src/Core/Matrix.h"

#include "rtmath.hpp"
#include "hittable.hpp"
#include "sphere.hpp"
#include "log.hpp"
#include "ray.hpp"
#include "antialiasing.hpp"

using namespace ppm;

Vector3f ray_color(const Ray &r, const Hittable &world);
Color color_intensity(Vector3f intensity);

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int SPP = 50;

int main() {
    // image
    float aspect = float(WIDTH) / float(HEIGHT);
    Image image(WIDTH, HEIGHT);

    // world
    HittableList world;
    world.add(std::make_shared<Sphere>(Vector3f(0, 0, -1), 0.5));
    world.add(std::make_shared<Sphere>(Vector3f(0, -100.5, -1), 100));
    
    // camera
    Camera camera;


    // render
    for (int h = 0; h < HEIGHT; h++) {
        for (int w = 0; w < WIDTH; w++) {
            Vector3f intensity(0, 0, 0);
            // anti-aliasing
            for (int s = 0; s < SPP; s++) {
                float u = float(w + random_double()) / (WIDTH - 1);
                float v = float(h + random_double()) / (HEIGHT - 1);
                // origin, at
                Ray r = camera.get_ray(u, v);
                Vector3f sample = ray_color(r, world);
                sample.x() = clamp(sample.x(), 0, 1);
                sample.y() = clamp(sample.y(), 0, 1);
                sample.z() = clamp(sample.z(), 0, 1);
                intensity += sample;
            }
            muliple_samples(intensity, SPP);
            // std::cout << intensity << std::endl;
            image.set(w, h, color_intensity(intensity));
            print_progress(h*WIDTH+w, WIDTH*HEIGHT, 40);
        }
    }
    print_progress(WIDTH*HEIGHT, WIDTH*HEIGHT, 40);
    std::cout << std::endl << "Done." << std::endl;

    image.vflip();
    image.save("../image.ppm");

    return 0;
}

double hit_sphere(const Vector3f &center, float radius, const Ray &r) {
    Vector3f oc = r.origin() - center;
    auto a = r.direction().dot(r.direction());
    auto b = 2.0 * oc.dot(r.direction());
    auto c = oc.dot(oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discriminant) ) / (2.0*a);
    }
}

Vector3f ray_color(const Ray &r, const Hittable &world) {
    HitRecord rec;
    if (world.hit(r, 0, infinity, rec)) {
        // Vector3f N = (r.at(t) - Vector3f(0, 0, -1)).normalized();
        // Color color(
                // (rec.normal.x() + 1.0) * 255.0 / 2.0,
                // (rec.normal.y() + 1.0) * 255.0 / 2.0,
                // (rec.normal.z() + 1.0) * 255.0 / 2.0
                // );
        // return color;
        return 0.5 * (rec.normal + Vector3f(1,1,1));
        // return rec.normal;
    }

    Vector3f unit_direction = r.direction().normalized();
    auto t = 0.5 * (unit_direction.y() + 1.0);
    // auto c1 = Color(255, 255, 255);
    auto c1 = Vector3f(1.0, 1.0, 1.0) * (1-t);
    // auto c2 = Color(127, 178, 255);
    auto c2 = Vector3f(0.5, 0.7, 1.0) * t;
    return c1 + c2;
}

Color color_intensity(Vector3f intensity) {
    intensity.x() = clamp(intensity.x(), 0, 1);
    intensity.y() = clamp(intensity.y(), 0, 1);
    intensity.z() = clamp(intensity.z(), 0, 1);
    return Color(
            static_cast<uint8_t>(255 * intensity.x()),
            static_cast<uint8_t>(255 * intensity.y()),
            static_cast<uint8_t>(255 * intensity.z())
        );
}

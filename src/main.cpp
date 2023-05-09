#include <cstdint>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <omp.h>

#include <ppmpp.hpp>
#include <tiny_obj_loader.h>
#include <Eigen/Dense>
#include "src/Core/Matrix.h"
#include <omp.h>

#include "camera.hpp"
#include "rtmath.hpp"
#include "hittable.hpp"
#include "sphere.hpp"
#include "log.hpp"
#include "ray.hpp"
#include "antialiasing.hpp"
#include "material.hpp"

using namespace ppm;

std::string current_date();
Colorf ray_color(const Ray &r, const Hittable &world, int depth);
Color color_intensity(Vector3f intensity);
HittableList random_scene();

#define WIDTH 960
#define HEIGHT 540
#define SPP 50
#define MAX_RECURSION_DEPTH 50

int main() {
    // image
    float aspect = float(WIDTH) / float(HEIGHT);
    Image image(WIDTH, HEIGHT);

    // world
    HittableList world = random_scene();


    
    // camera
    Vector3f eye(13, 2, 3);
    Vector3f lookat(0, 0, 0);
    Vector3f up(0, 1, 0);
    auto dist_to_focus = 10;
    float aperture = 0.1;

    Camera camera(eye, lookat, up, 20, aspect, aperture, dist_to_focus);

    // openmp
    #pragma omp parallel
    {
        // omp_set_dynamic(0);
        // omp_set_num_threads(4);
        int nthreads = omp_get_num_threads();
        print_log("LOG", "omp", (std::string("thread: ") + std::to_string(nthreads)).c_str());
    }
    unsigned long long completed = 0;
    unsigned long long total = WIDTH*HEIGHT;

    // render
#pragma omp parallel for
    for (int h = 0; h < HEIGHT; h++) {
#pragma omp parallel for
        for (int w = 0; w < WIDTH; w++) {
            Vector3f intensity(0, 0, 0); // anti-aliasing
            for (int s = 0; s < SPP; s++) {
                float u = float(w + random_float()) / (WIDTH - 1);
                float v = float(h + random_float()) / (HEIGHT - 1);
                // origin, at
                Ray r = camera.get_ray(u, v);
                Colorf sample = ray_color(r, world, MAX_RECURSION_DEPTH);
                sample.x() = clamp(sample.x(), 0, 1);
                sample.y() = clamp(sample.y(), 0, 1);
                sample.z() = clamp(sample.z(), 0, 1);
                intensity += sample;
            }
            muliple_samples(intensity, SPP);
            // std::cout << intensity << std::endl;
            image.set(w, h, color_intensity(intensity));
            #pragma omp critical
            {
                completed ++;
                print_progress(completed, total, 40);
            }
        }
    }
    print_progress(WIDTH*HEIGHT, WIDTH*HEIGHT, 40);
    std::cout << std::endl << "Done." << std::endl;

    image.vflip();
    image.save(("../"+current_date()+".ppm").c_str());

    return 0;
}

std::string current_date() {
  std::time_t now = std::time(nullptr);
  std::tm tm = *std::localtime(&now);

  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d-%H-%M");
  return ss.str();
}

Colorf ray_color(const Ray &r, const Hittable &world, int depth) {
    HitRecord rec;
    if (depth <= 0) {
        // std::cout << "dyeing black" << std::endl;
        return Colorf(0, 0, 0);
    }
    if (world.hit(r, 0.001, infinity, rec)) { // rec is over written by hit function
        // The ray after reflection
        Ray scattered;
        // 颜色衰减程度
        Colorf attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) { // read only rec
            return attenuation.cwiseProduct(ray_color(scattered, world, depth-1));
        }
        return Colorf(0, 0, 0);
    }

    // background color
    Vector3f unit_direction = r.direction().normalized();
    auto t = 0.5 * (unit_direction.y() + 1.0);
    auto c1 = Colorf(1.0, 1.0, 1.0) * (1-t);
    auto c2 = Colorf(0.5, 0.7, 1.0) * t;
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

HittableList random_scene() {
    HittableList world;
    auto ground_material = std::make_shared<Lambertian>(Colorf(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(Vector3f(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = random_float();
            Vector3f center(a + 0.9*random_float(), 0.2, b+0.9*random_float());
            if ((center - Vector3f(4, 0.2, 0)).norm() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Colorf(
                            random_float(0, 1),
                            random_float(0, 1),
                            random_float(0, 1)
                            );
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Colorf(
                            random_float(0.5, 1),
                            random_float(0.5, 1),
                            random_float(0.5, 1)
                            );
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Vector3f(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Colorf(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Vector3f(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Colorf(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Vector3f(4, 1, 0), 1.0, material3));

    return world;
}

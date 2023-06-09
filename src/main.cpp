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

#include "camera.hpp"
#include "rtmath.hpp"
#include "hittable.hpp"
#include "sphere.hpp"
#include "log.hpp"
#include "ray.hpp"
#include "antialiasing.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "rect.hpp"
#include "box.hpp"
#include "constant_medium.hpp"
#include "bvh.hpp"

using namespace ppm;

std::string current_date();
Colorf ray_color(const Ray &r, const Colorf &background, const Hittable &world, int depth);
Color color_intensity(Vector3f intensity);
HittableList random_scene();
HittableList two_perlin_spheres();
HittableList earth();
HittableList simple_light();
HittableList cornell_box();
HittableList cornell_smoke();
HittableList final_scene();

#define WIDTH 800
#define HEIGHT 800
#define SPP 1000
#define MAX_RECURSION_DEPTH 50

int main() {
    // image
    float aspect = float(WIDTH) / float(HEIGHT);
    Image image(WIDTH, HEIGHT);

    // world
    // HittableList world = random_scene();
    HittableList world = final_scene();


    
    // camera
    Vector3f eye(478, 278, -600);
    Vector3f lookat(278, 278, 0);
    Vector3f up(0, 1, 0);
    auto dist_to_focus = 10;
    float aperture = 0.1;

    Camera camera(eye, lookat, up, 40, aspect, aperture, dist_to_focus, 0.0, 1.0);

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
                // FIX: Colorf not correct
                Colorf sample = ray_color(r, Vector3f(0, 0, 0), world, MAX_RECURSION_DEPTH);
                intensity += sample;
            }
            muliple_samples(intensity, SPP);
            // std::cout << intensity << std::endl;
            intensity.x() = clamp(intensity.x(), 0, 1);
            intensity.y() = clamp(intensity.y(), 0, 1);
            intensity.z() = clamp(intensity.z(), 0, 1);
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

Colorf ray_color(const Ray &r, const Colorf &background, const Hittable &world, int depth) {
    HitRecord rec;
    
    // hit MAX_RECURSION_DEPTH
    if (depth <= 0) {
        return Colorf(0, 0, 0);
    }

    // no hit
    if (!world.hit(r, 0.001, infinity, rec)) {
        return background;
    }

    Ray scattered;
    Colorf attenuation;
    Colorf emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    // no scatter, means it emits light
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered)) { // read only rec
        return emitted;
    }

    // scattered
    return emitted + attenuation.cwiseProduct(ray_color(scattered, background, world, depth-1));

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

    auto checker = std::make_shared<Checker>(Colorf(0.2, 0.3, 0.1), Colorf(0.9, 0.9, 0.9));
    world.add(std::make_shared<Sphere>(Vector3f(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

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
                    auto center2 = center + Vector3f(0, random_float(0, 0.5), 0);
                    world.add(std::make_shared<SphereMoving>(center, center2, 0.0, 1.0, 0.2, sphere_material));
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

HittableList two_perlin_spheres() {
    HittableList objects;

    auto pertext = std::make_shared<Noise>(4);
    objects.add(std::make_shared<Sphere>(Vector3f(0,-1000,0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.add(std::make_shared<Sphere>(Vector3f(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    return objects;
}

HittableList earth() {
    auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
    auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    auto globe = std::make_shared<Sphere>(Vector3f(0,0,0), 2, earth_surface);

    return HittableList(globe);
}

HittableList simple_light() {
    HittableList objects;

    auto pertext = std::make_shared<Noise>(4);
    objects.add(std::make_shared<Sphere>(Vector3f(0,-1000,0), 1000, make_shared<Lambertian>(pertext)));
    objects.add(std::make_shared<Sphere>(Vector3f(0,2,0), 2, make_shared<Lambertian>(pertext)));

    auto difflight = std::make_shared<DiffuseLight>(Colorf(4,4,4));
    objects.add(std::make_shared<XYRect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

HittableList cornell_box() {
    HittableList objects;

    auto red   = std::make_shared<Lambertian>(Colorf(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Colorf(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Colorf(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Colorf(15, 15, 15));

    objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
    objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<XZRect>(213, 343, 227, 332, 554, light));
    objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));


    std::shared_ptr<Hittable> box1 = make_shared<Box>(Vector3f(0, 0, 0), Vector3f(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vector3f(265,0,295));
    objects.add(box1);

    std::shared_ptr<Hittable> box2 = make_shared<Box>(Vector3f(0,0,0), Vector3f(165,165,165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vector3f(130,0,65));
    objects.add(box2);

    return objects;
}

HittableList cornell_smoke() {
    HittableList objects;

    auto red   = std::make_shared<Lambertian>(Colorf(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Colorf(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Colorf(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Colorf(7, 7, 7));

    objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
    objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<XZRect>(113, 443, 127, 432, 554, light));
    objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));

    std::shared_ptr<Hittable> box1 = make_shared<Box>(Vector3f(0,0,0), Vector3f(165,330,165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vector3f(265,0,295));

    std::shared_ptr<Hittable> box2 = make_shared<Box>(Vector3f(0,0,0), Vector3f(165,165,165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vector3f(130,0,65));

    objects.add(make_shared<ConstantMedium>(box1, 0.01, Colorf(0,0,0)));
    objects.add(make_shared<ConstantMedium>(box2, 0.01, Colorf(1,1,1)));

    return objects;
}

HittableList final_scene() {
    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Colorf(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_float(1,101);
            auto z1 = z0 + w;

            boxes1.add(std::make_shared<Box>(Vector3f(x0,y0,z0), Vector3f(x1,y1,z1), ground));
        }
    }

    HittableList objects;

    objects.add(std::make_shared<BVHNode>(boxes1, 0, 1));

    auto light = std::make_shared<DiffuseLight>(Colorf(7, 7, 7));
    objects.add(make_shared<XZRect>(123, 423, 147, 412, 554, light));

    auto center1 = Vector3f(400, 400, 200);
    auto center2 = center1 + Vector3f(30,0,0);
    auto moving_sphere_material = std::make_shared<Lambertian>(Colorf(0.7, 0.3, 0.1));
    objects.add(std::make_shared<SphereMoving>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(std::make_shared<Sphere>(Vector3f(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
    objects.add(std::make_shared<Sphere>(
        Vector3f(0, 150, 145), 50, std::make_shared<Metal>(Colorf(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = std::make_shared<Sphere>(Vector3f(360,150,145), 70, std::make_shared<Dielectric>(1.5));
    objects.add(boundary);
    objects.add(std::make_shared<ConstantMedium>(boundary, 0.2, Colorf(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Sphere>(Vector3f(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
    objects.add(std::make_shared<ConstantMedium>(boundary, .0001, Colorf(1,1,1)));

    auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"));
    objects.add(std::make_shared<Sphere>(Vector3f(400,200,400), 100, emat));
    auto pertext = std::make_shared<Noise>(0.1);
    objects.add(std::make_shared<Sphere>(Vector3f(220,280,300), 80, std::make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(Colorf(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<Sphere>(v3f_random(0,165), 10, white));
    }

    objects.add(std::make_shared<Translate>(
        std::make_shared<RotateY>(
            std::make_shared<BVHNode>(boxes2, 0.0, 1.0), 15),
            Vector3f(-100,270,395)
        )
    );

    return objects;
}

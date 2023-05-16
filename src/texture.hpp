#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <Eigen/Dense>
#include <stb/stb_image.h>

#include <memory>
#include <iostream>

#include "rtmath.hpp"
#include "src/Core/Matrix.h"
#include "perlin.hpp"

class Texture {
    public:
        virtual Colorf value(float u, float v, const Eigen::Vector3f &p) const = 0;
};

class Solid: public Texture {
    private:
        Colorf color_value;
    public:
        Solid() {}
        Solid(Colorf c): color_value(c) {}
        Solid(float red, float green, float blue): color_value(Colorf(red, green, blue)) {}

        virtual Colorf value(float u, float v, const Eigen::Vector3f &p) const override {
            return color_value;
        }
};

class Checker: public Texture {
    private:
        std::shared_ptr<Texture> odd;
        std::shared_ptr<Texture> even;

    public:
        Checker() {}
        Checker(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
        : even(_even), odd(_odd) {}

        Checker(Colorf c1, Colorf c2) 
        : even(std::make_shared<Solid>(c1)), odd(std::make_shared<Solid>(c2)) {}

        virtual Colorf value(float u, float v, const Eigen::Vector3f &p) const override {
            auto sines = sin(10*p.x()) * sin(10*p.y()) * sin(10*p.z());
            if (sines < 0) {
                return odd->value(u, v, p);
            } else {
                return even->value(u, v, p);
            }
        }
};

class Noise: public Texture {
    private:
        Perlin noise;
        float scale;

    public:
        Noise() {}
        Noise(float sc): scale(sc) {}

        virtual Colorf value(float u, float v, const Eigen::Vector3f &p) const override {
            return Colorf(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10*noise.turb(p)));
        }
};

class ImageTexture: public Texture {
    private:
        unsigned char *data;
        int width, height;
        int bytes_per_scanline;

    public:
        const static int bytes_per_pixel = 3;

        ImageTexture(): data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

        ImageTexture(const char *filename) {
            auto components_per_pixel = bytes_per_pixel;
            data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
            if (!data) {
                std::cerr << "ERROR: could not load texture image file '" << filename << "'" << std::endl;
                width = height = 0;
            }

            bytes_per_scanline = bytes_per_pixel * width;

        }

        ~ImageTexture() {
            delete data;
        }

        virtual Colorf value(float u, float v, const Eigen::Vector3f &p) const override {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (data == nullptr) {
                return Colorf(0,1,1);
            }
            u = clamp(u, 0.0, 1.0);
            // flip
            v = 1.0 - clamp(v, 0.0, 1.0);

            auto i = static_cast<int>(u * width);
            auto j = static_cast<int>(v * height);

            if (i >= width) { i = width-1; }
            if (j >= height) { j = height-1; }

            const auto color_scale = 1.0 / 255.0;
            auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;

            return Colorf(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
        }


};

#endif // !_TEXTURE_H_

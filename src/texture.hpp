#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <Eigen/Dense>
#include <memory>

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

#endif // !_TEXTURE_H_

# Raytracer

A toy level raytracer written in C++, following [Ray tracing in one weekend](https://raytracing.github.io/).

## demo

![](./assets/image.png)
![](./assets/2023-05-08-22-19.png)
![](./assets/2023-05-09-22-32.png)
![](./assets/2023-05-12-11-26.png)
![](./assets/2023-05-15-23-58.png)
![](./assets/2023-05-16-10-08.png)
![](./assets/2023-05-18-19-19.png)

## build

Require <code>cmake</code>, <code>OpenMP</code> (Windows).

Dependencies are already included in the <code>dependencies</code> directory.

```
git clone --recurse-submodules https://github.com/wivl/raytracer.git
cd raytracer
mkdir build && cd build
cmake ..
make
```

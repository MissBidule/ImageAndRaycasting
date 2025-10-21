#ifndef _RAY_CIRCLE_HPP_
#define _RAY_CIRCLE_HPP_

#include "Utils.hpp"
#include "Image.hpp"

//Declaration

template<typename T>
void drawCircleRaytracing(Circle& c, Image<T>& img, T color, ViewType view, uint16_t fov = 0);

double isIntersect(Circle& c, ViewType view, uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t fov = 0);

template<typename T>
T depthColor(double distance, T color, Circle& c, ViewType view, uint16_t x0, uint16_t y0, uint16_t fov = 0);

//Definition

template<typename T>
void drawCircleRaytracing(Circle& c, Image<T>& img, T color, ViewType view, uint16_t fov) {
    
    uint16_t x0 = img.width/2;
    uint16_t y0 = img.height/2;
    
    for (uint16_t j = 0; j < img.height; j++) {
        for (uint16_t i = 0; i < img.width; i++) {
            double distance = isIntersect(c, view, i, j, x0, y0, fov);
            if (distance >= 0) {
                img.pixel(i, j) = depthColor(distance, color, c, view, x0, y0, fov);
            }
        }
    }
}

double isIntersect(Circle& circle, ViewType view, uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t fov) {
    Vec3f dir;
    Vec3f OC;
    if (view == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
        OC = Vec3f{circle.position.x - x, circle.position.y - y, circle.position.z};
    }
    else {
        dir = Vec3i{x - x0, y - y0, 0 + fov}.normalize();
        OC = Vec3f{circle.position.x - x0, circle.position.y - y0, circle.position.z - fov};
    }
    double a = dir.norm2(); //usually 1
    double b = - 2 * OC.dot(dir);
    double c = - std::pow(circle.r, 2) + OC.norm2();
    float delta = std::pow(b, 2) - 4 * a *  c;
    if (delta < 0) return -1;
    if (delta == 0) return (- b / (2 * a));
    double t1 = ((- b - std::sqrt(delta)) / (2 * a));
    double t2 = ((- b + std::sqrt(delta)) / (2 * a));
    if (t1 < t2) return t1;
    else return t2;
}

template<typename T>
T depthColor(double distance, T color, Circle& c, ViewType view, uint16_t x0, uint16_t y0, uint16_t fov) {
    float OC;
    if (view == ViewType::ORTHO) {
        OC = c.position.z;
    }
    else {
        OC = Vec3f{c.position.x - x0, c.position.y - y0, c.position.z - fov}.norm();
    }
    
    return color * ((distance - OC) / - c.r);
}

#endif //_RAY_CIRCLE_HPP_

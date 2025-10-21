#include "Circle.hpp"

Circle::Circle(Vec3f pos, float _radius, Material mat) : Primitive(pos, mat), radius(_radius) {}

double Circle::isIntersect(Vec3f rayPos, uint16_t x, uint16_t y, Camera cam, Light light) const {
    Vec3f dir;
    Vec3f OC;
    if (cam.viewType == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
        OC = Vec3f{pos.x - x, pos.y - y, pos.z};
    }
    else {
        dir = Vec3f{x - cam.viewPos.x, y - cam.viewPos.y, cam.FOV - cam.viewPos.z}.normalize();
        OC = Vec3f{pos.x - cam.viewPos.x, pos.y - cam.viewPos.y, pos.z - cam.viewPos.z};
    }
    double a = dir.norm2(); //usually 1
    double b = - 2 * OC.dot(dir);
    double c = - std::pow(radius, 2) + OC.norm2();
    float delta = std::pow(b, 2) - 4 * a *  c;
    if (delta < 0) return -1;
    if (delta == 0) return (- b / (2 * a));
    double t1 = ((- b - std::sqrt(delta)) / (2 * a));
    double t2 = ((- b + std::sqrt(delta)) / (2 * a));
    if (t1 < t2) return t1;
    else return t2;
}

Color Circle::definitiveColor(Light light, Vec3f viewPos, Vec3f normal, Vec3f fragPos) const {
    return Color(0, 0, 0);
}

float Circle::depthValue(double distance, Camera cam) const {
    return -1;
}
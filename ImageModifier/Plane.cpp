#include "Plane.hpp"

Plane::Plane(Vec3f pos, Vec3f _normal, Material mat) : Primitive(pos, mat), normal(_normal.normalize()) {}

double Plane::isViewIntersect(uint16_t x, uint16_t y, Camera cam) const {
    Vec3f dir;
    
    if (cam.viewType == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
    }
    else {
        dir = Vec3f{x - cam.viewPos.x, y - cam.viewPos.y, cam.FOV - cam.viewPos.z}.normalize();
    }

    return intersection(dir, cam.viewPos);
}

float Plane::raytrace(Vec3f rayPos, Vec3f fragPos) const {
    Vec3f dir = (fragPos - rayPos).normalize();
    
    return intersection(dir, rayPos);
}

double Plane::intersection(Vec3f dir, Vec3f rayPos) const {
    double a = normal.x;
    double b = normal.y;
    double c = normal.z;
    double d = a * (rayPos.x - pos.x) + b * (rayPos.y - pos.y) + c * (rayPos.z - pos.z);
    double delta = a * dir.x + b * dir.y + c * dir.z;
    if (abs(delta) < 0.001) return - 1;
    double t = - d / delta;
    return t > 0 ? t : -1 ;
}

Vec3f Plane::normalAtPoint(Vec3f fragPos) const {
    return normal;
}

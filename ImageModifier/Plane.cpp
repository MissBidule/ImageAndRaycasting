#include "Plane.hpp"

Plane::Plane(Vec3f pos, Vec3f _normal, Material mat) : Primitive(pos, mat), normal(_normal.normalize()) {}

double Plane::isViewIntersect(float x, float y, Camera cam) const {
    Vec3f dir;
    
    if (cam.viewType == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
    }
    else {
        dir = Vec3f{x - (float)cam.width/2, - y + (float)cam.height/2, (float)cam.FOV}.normalize();
    }

    return intersection(dir, cam.viewPos);
}

float Plane::raytrace(Vec3f rayPos, Vec3f dir) const {
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

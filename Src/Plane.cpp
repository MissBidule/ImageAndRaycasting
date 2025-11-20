#include "Plane.hpp"

Plane::Plane(Vec3f pos, Vec3f _normal, Material& mat) : Primitive(pos, mat), normal(_normal.normalize()) {}

double Plane::raytrace(Ray& ray) const {
    return intersection(ray.rayPos + ray.rayDir * offset, ray.rayDir);
}

double Plane::intersection(Vec3f rayPos, Vec3f dir) const {
    double a = normal.x;
    double b = normal.y;
    double c = normal.z;
    double d = a * (rayPos.x - pos.x) + b * (rayPos.y - pos.y) + c * (rayPos.z - pos.z);
    double delta = a * dir.x + b * dir.y + c * dir.z;
    if (abs(delta) < 0.001) return - 1;
    double t = - d / delta;
    return t > 0 ? t : -1 ;
}

Vec3f Plane::normalAtPoint(Vec3f fragPos, const Ray& ray) const {
    return normal * (2 *(retrieveNormalDir(normal, ray.rayDir)) - 1);
}

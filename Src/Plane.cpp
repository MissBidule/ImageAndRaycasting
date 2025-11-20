#include "Plane.hpp"

Plane::Plane(Vec3f pos, Vec3f _normal, Material& mat, bool isPartOfObj) : Primitive(pos, mat, isPartOfObj), normal(_normal.normalize()) {}

double Plane::raytrace(Ray& ray, Hit& hit) {
    double d = intersection(ray.rayPos + ray.rayDir * offset, ray.rayDir);
    Vec3f fragPos = ray.rayPos + ray.rayDir * (float)d;

    hit.object = this;
    hit.fragPos = fragPos;
    hit.normal = normalAtPoint(fragPos, ray);
    
    return d;
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

void Plane::setScale(float newScale) {
    return; //useless
}

void Plane::setTranslate(Vec3f newTranslate) {
    pos = pos + newTranslate;
}
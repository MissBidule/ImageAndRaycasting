#include "Circle.hpp"

Circle::Circle(Vec3f pos, float _radius, Material* mat, bool isPartOfObj) : Primitive(pos, mat, isPartOfObj), radius(_radius) {}

double Circle::raytrace(Ray& ray, Hit& hit) {
    ++raysLocal;
    Vec3f OC = pos - (ray.rayPos + ray.rayDir * offset);
    double d = intersection(ray.rayDir, OC);
    Vec3f fragPos = ray.rayPos + ray.rayDir * (float)d;

    hit.object = this;
    hit.fragPos = fragPos;
    hit.normal = normalAtPoint(fragPos, ray);
    
    return d;
}

double Circle::intersection(Vec3f dir, Vec3f OC) const {
    double a = dir.norm2(); //usually 1
    double b = - 2 * OC.dot(dir);
    double c = - std::pow(radius, 2) + OC.norm2();
    float delta = std::pow(b, 2) - 4 * a *  c;
    if (delta < 0) return -1;
    double t1 = ((- b - std::sqrt(delta)) / (2 * a));
    double t2 = ((- b + std::sqrt(delta)) / (2 * a));
    if (t1 > offset) return t1;
    else if (t2 > offset) return t2;
    return -1;
}

Vec3f Circle::normalAtPoint(Vec3f fragPos, const Ray& ray) const {
    Vec3f outwardNormal = (fragPos - pos).normalize();
    int face = retrieveNormalDir(outwardNormal, ray.rayDir) ? 1 : -1;
    return outwardNormal * face;
}

float Circle::depthValue(double distance, Camera cam) const {
    float OC;
    if (cam.viewType == ViewType::ORTHO) {
        OC = pos.z;
    }
    else {
        OC = Vec3f{pos.x - cam.viewPos.x, pos.y - cam.viewPos.y, pos.z - cam.viewPos.z}.norm();
    }
    
    return ((distance - OC) / - radius);
}

void Circle::setScale(float newScale) {
    radius *= newScale;
}

void Circle::setTranslate(Vec3f newTranslate) {
    pos = pos + newTranslate;
}

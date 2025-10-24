#include "Circle.hpp"

Circle::Circle(Vec3f pos, float _radius, Material mat) : Primitive(pos, mat), radius(_radius) {}

double Circle::raytrace(Vec3f rayPos, Vec3f dir) const {
    Vec3f OC = pos - (rayPos + dir * offset);
    
    return intersection(dir, OC);
}

double Circle::intersection(Vec3f dir, Vec3f OC) const {
    double a = dir.norm2(); //usually 1
    double b = - 2 * OC.dot(dir);
    double c = - std::pow(radius, 2) + OC.norm2();
    float delta = std::pow(b, 2) - 4 * a *  c;
    if (delta < 0) return -1;
    double t1 = ((- b - std::sqrt(delta)) / (2 * a));
    double t2 = ((- b + std::sqrt(delta)) / (2 * a));
    if (t1 > 0) return t1;
    else if (t2 > 0) return t2;
    return -1;
}

Vec3f Circle::normalAtPoint(Vec3f fragPos, Vec3f rayDir) const {
    Vec3f outwardNormal = (fragPos - pos).normalize();
    return outwardNormal * (2 *(retrieveNormalDir(outwardNormal, rayDir)) - 1);
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

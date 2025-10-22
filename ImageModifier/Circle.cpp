#include "Circle.hpp"

Circle::Circle(Vec3f pos, float _radius, Material mat) : Primitive(pos, mat), radius(_radius) {}

double Circle::isIntersect(Vec3f rayPos, uint16_t x, uint16_t y, Camera cam) const {
    Vec3f dir;
    Vec3f OC;
    if (cam.viewType == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
        OC = Vec3f{pos.x - x, pos.y - y, pos.z};
    }
    else {
        dir = Vec3f{x - cam.viewPos.x, y - cam.viewPos.y, cam.FOV - cam.viewPos.z}.normalize();
        OC = pos - cam.viewPos;
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

Color Circle::definitiveColor(double distance, Light light, Camera cam, uint16_t x, uint16_t y) const {
    Vec3f dir;
    Vec3f fragPos;
    if (cam.viewType == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
        fragPos = Vec3f{(float)x, (float)y, 0} + dir * (float)distance;
    }
    else {
        dir = Vec3f{x - cam.viewPos.x, y - cam.viewPos.y, cam.FOV - cam.viewPos.z}.normalize();
        fragPos = cam.viewPos + dir * (float)distance;
    }
    
    //TEST LIGHT RAYTRACE
    
    //This is the only thing that circle has in particular must change
    Vec3f normal = (fragPos - pos).normalize();
    //
    Vec3f lightColor = light.color;
    Vec3f matAmbient = mat.ambient;
    Vec3f matDiffuse = mat.diffuse;
    Vec3f matSpecular = mat.specular;

    //ambient
    Vec3f ambient = lightColor * matAmbient;
    
    //diffuse
    Vec3f lightDir = (light.pos - fragPos).normalize();
    float diff = std::max(normal.dot(lightDir), 0.0f);
    Vec3f diffuse = lightColor * matDiffuse * diff;
    
    //specular
    Vec3f viewDir = (cam.viewPos - fragPos).normalize();
    Vec3f reflectDir = (-lightDir).reflect(normal);
    float spec = std::pow(std::max(viewDir.dot(reflectDir), 0.0f), mat.shininess);
    Vec3f specular = matSpecular * lightColor * spec;
    
    //result
    return Color::colorFromVec3f(ambient + diffuse + specular);
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

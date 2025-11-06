#ifndef _MISC_HPP_
#define _MISC_HPP_

#include "Color.hpp"

enum ViewType {
    ORTHO,
    PERSP
};

enum LightType {
    POINT,
    DIR
};

enum MaterialType {
    DIFFUSE,
    TRANSPARENT,
    REFLECTIVE
};

struct Camera {
    Vec3f viewPos;
    ViewType viewType;
    int width;
    int height;
    int FOV = 1000;
};

struct Light {
    Vec3f pos; //is dir when dirLight
    Color color;
    LightType type;
    float linear;
    float quadratic;
    float constant;
    
    static std::vector<Light*> lightList;
    
    inline Light(Vec3f _pos, Color _color, LightType _type, float _linear = 0.0014f, float _quadratic = 0.000007f, float _constant = 1.0f) : pos(_pos), color(_color), type(_type), linear(_linear), quadratic(_quadratic), constant(_constant) {
        lightList.emplace_back(this);
    }
};

struct Material {
    Color ambient = Color();
    Color diffuse = Color();
    Color specular = Color();
    float shininess = 0;
    float alpha = 1; //only used with transparent
    float ior = 1;//only used with transparent

    MaterialType type = MaterialType::DIFFUSE;

    static float reflectance(double cosine, double refractionIndex) {
        float r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 *= r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

#endif // _MISC_HPP_ miscellaneous

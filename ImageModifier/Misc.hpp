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
    Color ambient;
    Color diffuse;
    Color specular;
    float shininess;
};

#endif // _MISC_HPP_ miscellaneous

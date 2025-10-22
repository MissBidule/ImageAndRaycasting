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
    Vec3f pos; //used only for pointlight
    Color color;
    LightType type;
    //float constant;
//    float linear;
//    float quadratic;
    Vec3f dir; //used only for directional
    
    static std::vector<Light*> lightList;
    
    inline Light(Vec3f _pos, Color _color, LightType _type, Vec3f _dir = Vec3f{}) : pos(_pos), color(_color), type(_type), dir(_dir) {
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

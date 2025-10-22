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
    Vec3f pos;
    Color color;
    LightType type;
    Vec3f dir; //used only for directional
};

struct Material {
    Color ambient;
    Color diffuse;
    Color specular;
    float shininess;
};

#endif // _MISC_HPP_ miscellaneous

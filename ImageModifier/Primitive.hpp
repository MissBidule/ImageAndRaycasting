#ifndef _PRIMITIVE_HPP_
#define _PRIMITIVE_HPP_

#include <vector>
#include "ColorImage.hpp"
#include "Utils.hpp"

class Primitive {
    public:
        static std::vector<Primitive*> objectList;

        //will call all primitives to draw
        static ColorImage* draw(const ColorImage& img, Camera cam, Light light); /////////////////////////////////////MULTIPLE LIGHTS !!!!!

    protected:
        virtual double isIntersect(Vec3f rayPos, uint16_t x, uint16_t y, Camera cam) const = 0;
        virtual Color definitiveColor(double distance, Light light, Camera cam, uint16_t x, uint16_t y) const = 0;
        virtual float depthValue(double distance, Camera cam) const = 0;
        
        friend bool sortByDepth(const Primitive& a, const Primitive& b);

        Primitive(Vec3f _pos, Material _mat);

        Vec3f pos;
        Material mat;
};

#endif //_PRIMITIVE_HPP_

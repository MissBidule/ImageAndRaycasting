#ifndef _PRIMITIVE_HPP_
#define _PRIMITIVE_HPP_

#include <vector>
#include "ColorImage.hpp"
#include "Misc.hpp"

class Primitive {
    public:
        static std::vector<Primitive*> objectList;

        //will call all primitives to draw
        static ColorImage* draw(const ColorImage& img, Camera cam);

    protected:
        Vec3f definitiveColor(double distance, Camera cam, float x, float y) const;
        virtual Vec3f normalAtPoint(Vec3f fragPos) const = 0;
        virtual double isViewIntersect(float x, float y, Camera cam) const = 0;
        virtual float raytrace(Vec3f rayPos, Vec3f dir) const = 0;
        
        friend bool sortByDepth(const Primitive& a, const Primitive& b);

        Primitive(Vec3f _pos, Material _mat);

        Vec3f pos;
        Material mat;

        const float offset = 0.1f;
};

#endif //_PRIMITIVE_HPP_

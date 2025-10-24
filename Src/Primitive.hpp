#ifndef _PRIMITIVE_HPP_
#define _PRIMITIVE_HPP_

#include <vector>
#include "ColorImage.hpp"
#include "Misc.hpp"

class Primitive {
    public:
        static std::vector<Primitive*> objectList;
        static Vec3f defaultColor;

        //will call all primitives to draw
        static ColorImage* draw(const ColorImage& img, Camera cam, int samples = 4);

    protected:
        static Vec3f definitiveColor(Vec3f orig, Vec3f dir, Vec3f camPos, uint8_t depth = 5);
        Vec3f diffuseCalculation(Vec3f fragPos, Vec3f camPos, double distance);
        Vec3f phongColor(Vec3f fragPos, Vec3f lightColor, Vec3f lightDir, Vec3f camPos, float attenuation);
        bool retrieveNormalDir(Vec3f normal, Vec3f rayDir) const;
        virtual Vec3f normalAtPoint(Vec3f fragPos, Vec3f rayDir = Vec3f{0, 0, 0}) const = 0;
        virtual double raytrace(Vec3f rayPos, Vec3f dir) const = 0;
        
        friend bool sortByDepth(const Primitive& a, const Primitive& b);

        Primitive(Vec3f _pos, Material _mat);

        Vec3f pos;
        Material mat;

        const float offset = 0.1f;
};

#endif //_PRIMITIVE_HPP_

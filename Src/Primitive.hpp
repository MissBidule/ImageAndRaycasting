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
        static Vec3f definitiveColor(Ray& ray, Vec3f camPos, int depth = 5);
        Vec3f transparentCalculation(Vec3f fragPos, Vec3f camPos, const Ray &ray, int depth) const;
        Vec3f diffuseCalculation(Vec3f fragPos, Vec3f camPos, double distance, const Ray& ray) const;
        Vec3f phongColor(Vec3f fragPos, Vec3f lightColor, Vec3f lightDir, Vec3f camPos, const Ray& ray, float attenuation) const;
        bool retrieveNormalDir(Vec3f normal, Vec3f rayDir) const;
        virtual Vec3f normalAtPoint(Vec3f fragPos, const Ray& ray) const = 0;
        virtual double raytrace(Ray& ray) const = 0;
        
        friend bool sortByDepth(const Primitive& a, const Primitive& b);

        Primitive(Vec3f _pos, Material& _mat);

        Vec3f pos;
        Material* mat;

        const float offset = 0.1f;
};

#endif //_PRIMITIVE_HPP_

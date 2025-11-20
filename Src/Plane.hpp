#ifndef _PLANE_HPP_
#define _PLANE_HPP_

#include "Primitive.hpp"

class Plane : public Primitive {
    public:
        Plane(Vec3f pos, Vec3f normal, Material* mat, bool isPartOfObj = false);
        void setScale(float newScale) override;
        void setTranslate(Vec3f newTranslate) override;

    private:
        Vec3f normalAtPoint(Vec3f fragPos, const Ray& ray) const;
        double raytrace(Ray& ray, Hit& hit) override;
        double intersection(Vec3f rayPos, Vec3f dir) const;

        Vec3f normal;
};

#endif //_PLANE_HPP_

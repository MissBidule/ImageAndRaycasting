#ifndef _CIRCLE_HPP_
#define _CIRCLE_HPP_

#include "Primitive.hpp"

class Circle : public Primitive {
    public:
        Circle(Vec3f pos, float _radius, Material* mat, bool isPartOfObj = false);
        void setScale(float newScale) override;
        void setTranslate(Vec3f newTranslate) override;
        double raytrace(Ray& ray, Hit& hit) override;

    private:
        Vec3f normalAtPoint(Vec3f fragPos, const Ray& ray) const;
        double intersection(Vec3f dir, Vec3f OC) const;
        float depthValue(double distance, Camera cam) const;

        float radius;
};

#endif //_CIRCLE_HPP_

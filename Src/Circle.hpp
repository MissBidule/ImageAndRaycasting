#ifndef _CIRCLE_HPP_
#define _CIRCLE_HPP_

#include "Primitive.hpp"

class Circle : public Primitive {
    public:
        Circle(Vec3f pos, float _radius, Material mat);

    private:
        Vec3f normalAtPoint(Vec3f fragPos, Vec3f rayDir) const override;
        double raytrace(Vec3f rayPos, Vec3f dir) const override;
        double intersection(Vec3f dir, Vec3f OC) const;
        float depthValue(double distance, Camera cam) const;

        float radius;
};

#endif //_CIRCLE_HPP_

#ifndef _PLANE_HPP_
#define _PLANE_HPP_

#include "Primitive.hpp"

class Plane : public Primitive {
    public:
        Plane(Vec3f pos, Vec3f normal, Material mat);

    private:
        Vec3f normalAtPoint(Vec3f fragPos) const override;
        double isViewIntersect(float x, float y, Camera cam) const override;
        float raytrace(Vec3f rayPos, Vec3f dir) const override;
        double intersection(Vec3f rayPos, Vec3f dir) const;

        Vec3f normal;
};

#endif //_PLANE_HPP_

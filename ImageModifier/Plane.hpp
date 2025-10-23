#ifndef _PLANE_HPP_
#define _PLANE_HPP_

#include "Primitive.hpp"

class Plane : public Primitive {
    public:
        Plane(Vec3f pos, Vec3f normal, Material mat);

    private:
        Vec3f normalAtPoint(Vec3f fragPos) const override;
        double isViewIntersect( uint16_t x, uint16_t y, Camera cam) const override;
        float raytrace(Vec3f rayPos, Vec3f dir) const override;
        double intersection(Vec3f dir, Vec3f rayPos) const;

        Vec3f normal;
};

#endif //_PLANE_HPP_

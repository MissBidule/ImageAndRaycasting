#ifndef _TRIANGLE_HPP_
#define _TRIANGLE_HPP_

#include "Primitive.hpp"

class Triangle : public Primitive {
    public:
        Triangle(Vec3f vert1, Vec3f vert2, Vec3f vert3, Material mat);

    private:
        Vec3f normalAtPoint(Vec3f fragPos, Vec3f rayDir) const override;
        double raytrace(Vec3f rayPos, Vec3f dir) const override;
        double intersection(Vec3f rayPos, Vec3f dir) const;

        Vec3f normal;
        Vec3f vertex0;
        Vec3f vertex1;
        Vec3f vertex2;
};

#endif //_TRIANGLE_HPP_

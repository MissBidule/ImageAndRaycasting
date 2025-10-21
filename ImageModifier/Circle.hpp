#ifndef _CIRCLE_HPP_
#define _CIRCLE_HPP_

#include "Primitive.hpp"

class Circle : public Primitive {
    public:
        Circle(Vec3f pos, float _radius, Material mat);

    private:
        double isIntersect(Vec3f rayPos, uint16_t x, uint16_t y, Camera cam) const override;
        Color definitiveColor(double distance, Light light, Camera cam, uint16_t x, uint16_t y) const override;
        float depthValue(double distance, Camera cam) const override;

        float radius;
};

#endif //_CIRCLE_HPP_
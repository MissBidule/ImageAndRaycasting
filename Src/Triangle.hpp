#ifndef _TRIANGLE_HPP_
#define _TRIANGLE_HPP_

#include "Primitive.hpp"

class Triangle : public Primitive {
    public:
        Triangle(Vec3f vert0, Vec3f vert1, Vec3f vert2, Material* mat, bool isPartOfObj = false);
        void setNormalByVertex(Vec3f norm0, Vec3f norm1, Vec3f norm2);
        void setScale(float newScale) override;
        void setTranslate(Vec3f newTranslate) override;

    private:
        Vec3f normalAtPoint(float u, float v, const Ray& ray) const;
        double raytrace(Ray& ray, Hit& hit) override;
        double intersection(Vec3f rayPos, Vec3f dir, float& u, float& v) const;

        Vec3f defaultNormal;
        Vec3f vertex0;
        Vec3f vertex1;
        Vec3f vertex2;
        std::vector<Vec3f> normalByVertex;
};

#endif //_TRIANGLE_HPP_

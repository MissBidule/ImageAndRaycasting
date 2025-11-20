#ifndef _MULTI_MESH_HPP_
#define _MULTI_MESH_HPP_

#include "Triangle.hpp"

class MultiMesh : public Primitive {
    public:
        MultiMesh(bool isPartOfObj = false);
        void addTriangleMesh(Triangle triangle);
        void setScale(float newScale) override;
        void setTranslate(Vec3f newTranslate) override;
        std::vector<Triangle>& getMeshes() const override { return meshes;};

    private:
        double raytrace(Ray& ray, Hit& hit) override;
        bool intersection(Vec3f rayPos, Vec3f dir, float& u, float& v) const;

        Vec3f min;
        Vec3f max;
};

#endif //_MULTI_MESH_HPP_

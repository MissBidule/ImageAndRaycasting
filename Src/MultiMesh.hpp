#ifndef _MULTI_MESH_HPP_
#define _MULTI_MESH_HPP_

#include "Triangle.hpp"

class MultiMesh : public Primitive {
    public:
        MultiMesh(const MultiMesh& mm, Material* mat);
        MultiMesh(std::string objFileName, Material* mat);
        void addTriangleMesh(Triangle* triangle);
        void setScale(float newScale) override;
        void setTranslate(Vec3f newTranslate) override;
        const std::vector<Primitive*>& getMeshes() const override { return meshes;};
        double shadowRaytrace(Ray& ray, Hit& hit, Material& shadowMat, float maxDist = -1) override;
        double raytrace(Ray& ray, Hit& hit) override;

    private:
        void loadObjtriangles(std::string objFileName, Material* mat);
        bool intersection(Vec3f rayPos, Vec3f dir) const;

        Vec3f min;
        Vec3f max;
        std::vector<Primitive*> meshes;
};

#endif //_MULTI_MESH_HPP_

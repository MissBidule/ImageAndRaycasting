#ifndef _MULTI_MESH_HPP_
#define _MULTI_MESH_HPP_

#include "Triangle.hpp"

struct BBox {
    Vec3f* min;
    Vec3f* max;
    Triangle* mesh = nullptr;
    BBox* left = nullptr;
    BBox* right = nullptr;
};

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
        void initBoundingBoxes(int start, int end, BBox& currentBox);

        std::vector<Primitive*> meshes;
        BBox mainBBox;
};

#endif //_MULTI_MESH_HPP_

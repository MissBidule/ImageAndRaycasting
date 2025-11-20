#ifndef _MULTI_MESH_HPP_
#define _MULTI_MESH_HPP_

#include "Triangle.hpp"

class MultiMesh : public Primitive {
    public:
        MultiMesh(std::string objFileName, Material& mat);
        void addTriangleMesh(Triangle* triangle);
        void setScale(float newScale) override;
        void setTranslate(Vec3f newTranslate) override;
        const std::vector<Primitive*>& getMeshes() override { return meshes;};

    private:
        void loadObjtriangles(std::string objFileName, Material& mat);
    
        double raytrace(Ray& ray, Hit& hit) override;
        bool intersection(Vec3f rayPos, Vec3f dir) const;

        Vec3f min;
        Vec3f max;
        std::vector<Primitive*> meshes;
};

#endif //_MULTI_MESH_HPP_

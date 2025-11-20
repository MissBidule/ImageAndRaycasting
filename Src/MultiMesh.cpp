#include "MultiMesh.hpp"

MultiMesh::MultiMesh(Vec3f _min, Vec3f _max, bool isPartOfObj) : Primitive(Vec3f{0,0,0}, mat{}, isPartOfObj), min(_min), max(_max) {
    multiMesh = true;
}

void MultiMesh::addTriangleMesh(Triangle& triangle) {
    meshes.emplace_back(triangle);
}

double MultiMesh::raytrace(Ray& ray, Hit& hit) {
    if (intersection(ray.rayPos, ray.rayDir)) return 0;
    return -1;
}

bool MultiMesh::intersection(Vec3f rayPos, Vec3f dir) const {
    Vec3f invdir = dir.invdir();

    if (invdir.x >= 0) {
        float tmin = (min.x - rayPos.x) * invdir.x; 
        float tmax = (max.x - rayPos.x) * invdir.x; 
    }
    else {
        float tmax = (min.x - rayPos.x) * invdir.x; 
        float tmin = (max.x - rayPos.x) * invdir.x; 
    }

    if (invdir.y >= 0) {
        float tymin = (min.y - rayPos.y) * invdir.y; 
        float tymax = (max.y - rayPos.y) * invdir.y; 
    }
    else {
        float tymax = (min.y - rayPos.y) * invdir.y; 
        float tymin = (max.y - rayPos.y) * invdir.y; 
    }

    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 

    if (tymin > tmin) tmin = tymin; 
    if (tymax < tmax) tmax = tymax; 

    if (invdir.z >= 0) {
        float tzmin = (min.z - rayPos.z) * invdir.z; 
        float tzmax = (max.z - rayPos.z) * invdir.z; 
    }
    else {
        float tzmax = (min.z - rayPos.z) * invdir.z; 
        float tzmin = (max.z - rayPos.z) * invdir.z; 
    }

    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 

    return true; 
}

void MultiMesh::setScale(float newScale) {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i].setScale(newScale);
    }
}

void MultiMesh::setTranslate(Vec3f newTranslate) {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i].setTranslate(newTranslate);
    }
}
#define TINYOBJLOADER_IMPLEMENTATION
#include "MultiMesh.hpp"
#include "tiny_obj_loader.h"
#include <memory>

MultiMesh::MultiMesh(const MultiMesh& mm, Material* mat) : Primitive(Vec3f{0,0,0}, nullptr, false) {
    mainBBox.min = new Vec3f(*mm.mainBBox.min);
    mainBBox.max = new Vec3f(*mm.mainBBox.max);
    std::vector<Primitive*> meshes = mm.getMeshes();
    for (size_t i = 0; i < meshes.size(); i++) {
        const Triangle* mesh = dynamic_cast<Triangle*>(meshes[i]);
        Triangle* newMesh = new Triangle(mesh, mat, true);
        addTriangleMesh(newMesh);
    }
}

MultiMesh::MultiMesh(std::string objFileName, Material* mat) : Primitive(Vec3f{0,0,0}, nullptr, false) {
    loadObjtriangles(objFileName, mat);
}

void MultiMesh::addTriangleMesh(Triangle* triangle) {
    meshes.emplace_back(triangle);
}

double MultiMesh::raytrace(Ray& ray, Hit& hit) {
    if (intersection(ray.rayPos, ray.rayDir)) {
        Hit currentHit;
        double distance = -1;
        double temp;
        const std::vector<Primitive*> triangleMeshes = getMeshes();
        for (size_t j = 0; j < triangleMeshes.size(); j ++) {
            temp = triangleMeshes[j]->raytrace(ray, currentHit);
            if ((temp != -1 && temp < distance) || distance == -1) {
                distance = temp;
                hit = currentHit;
            }
        }
        return distance;
    }
    return -1;
}

double MultiMesh::shadowRaytrace(Ray& ray, Hit& hit, Material& shadowMat, float maxDist){
    shadowMat.ignoreShadow = true;
    shadowMat.diffuse = Color(255,255,255);
    shadowMat.alpha = 0;
    Material currentMat;
    double lastHit = -1;
    const std::vector<Primitive*> triangleMeshes = getMeshes();
    for (size_t k = 0; k < triangleMeshes.size(); k ++) {
        double distance = triangleMeshes[k]->shadowRaytrace(ray, hit, currentMat);
        if (distance != -1 && ((distance - maxDist) < offset || maxDist == -1)) {
            lastHit = distance;
            shadowMat.ignoreShadow = false;
            if (currentMat.type == MaterialType::TRANSPARENT) {
                shadowMat.type = MaterialType::TRANSPARENT;
                shadowMat.alpha += currentMat.alpha;
                if (shadowMat.alpha >= 1) {
                    shadowMat.alpha = 1;
                    break;
                }
                shadowMat.diffuse = shadowMat.diffuse&currentMat.diffuse;
            }
            else break;
        }
    }
    return lastHit;
}

bool MultiMesh::intersection(Vec3f rayPos, Vec3f dir) const {
    Vec3f invdir = dir.invdir();
    
    Vec3f min = *mainBBox.min;
    Vec3f max = *mainBBox.max;
    float tmin, tmax;
    if (invdir.x >= 0) {
        tmin = (min.x - rayPos.x) * invdir.x;
        tmax = (max.x - rayPos.x) * invdir.x;
    }
    else {
        tmax = (min.x - rayPos.x) * invdir.x;
        tmin = (max.x - rayPos.x) * invdir.x;
    }

    float tymin, tymax;
    if (invdir.y >= 0) {
        tymin = (min.y - rayPos.y) * invdir.y;
        tymax = (max.y - rayPos.y) * invdir.y;
    }
    else {
        tymax = (min.y - rayPos.y) * invdir.y;
        tymin = (max.y - rayPos.y) * invdir.y;
    }

    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 

    if (tymin > tmin) tmin = tymin; 
    if (tymax < tmax) tmax = tymax; 

    float tzmin, tzmax;
    if (invdir.z >= 0) {
        tzmin = (min.z - rayPos.z) * invdir.z;
        tzmax = (max.z - rayPos.z) * invdir.z;
    }
    else {
        tzmax = (min.z - rayPos.z) * invdir.z;
        tzmin = (max.z - rayPos.z) * invdir.z;
    }

    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 

    return true; 
}

void MultiMesh::setScale(float newScale) {
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->setScale(newScale);
    }
    *mainBBox.min = *mainBBox.min * newScale;
    *mainBBox.max = *mainBBox.max * newScale;
}

void MultiMesh::setTranslate(Vec3f newTranslate) {
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->setTranslate(newTranslate);
    }
    *mainBBox.min = *mainBBox.min + newTranslate;
    *mainBBox.max = *mainBBox.max + newTranslate;
}

void MultiMesh::loadObjtriangles(std::string objFileName, Material* mat) {

    std::string inputfile = "objFiles/" + objFileName + ".obj";
    tinyobj::ObjReaderConfig reader_config;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "1.TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "2.TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    Vec3f _min = Vec3f{};
    Vec3f _max = Vec3f{};
    std::vector<Vec3f> trianglesVertex;
    std::vector<Vec3f> normals;

    // Loop over shapes
    bool first = true;
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = -attrib.vertices[3*size_t(idx.vertex_index)+2];
                
                Vec3f vertex{vx, vy, vz};
                trianglesVertex.emplace_back(vertex);
                
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = -attrib.normals[3*size_t(idx.normal_index)+2];
                    
                    Vec3f normal{nx, ny, nz};
                    normals.emplace_back(normal);
                  }

                if (first) {
                    _min = vertex;
                    _max = vertex;
                    first = false;
                }
                else {
                    if (vx < _min.x) _min.x = vx;
                    else if (vx > _max.x) _max.x = vx;
                    if (vy < _min.y) _min.y = vy;
                    else if (vy > _max.y) _max.y = vy;
                    if (vz < _min.z) _min.z = vz;
                    else if (vz > _max.z) _max.z = vz;
                }
            }
            index_offset += fv;
        }
    }
    
    float minCoord = _min.x < _min.y ? (_min.z < _min.x ? _min.z : _min.x) : (_min.z < _min.y ? _min.z : _min.y);
    float maxCoord = _max.x > _max.y ? (_max.z > _max.x ? _max.z : _max.x) : (_max.z > _max.y ? _max.z : _max.y);
    float actualAmplitude = maxCoord - minCoord;
    if (actualAmplitude == 0) return;
    float factor = 2 / actualAmplitude;
    _min = _min * factor;
    _max = _max * factor;
    Vec3f rebaseVec = Vec3f{-1, -1, -1} - _min;
    std::cout << minCoord << " " << maxCoord << std::endl;
    std::cout << factor << std::endl;
    std::vector<std::unique_ptr<Triangle>> triangleList;
    for (size_t i = 0; i < trianglesVertex.size(); i+=3) {
        trianglesVertex[i] = (trianglesVertex[i] * factor) + rebaseVec;
        trianglesVertex[i + 1] = (trianglesVertex[i + 1] * factor) + rebaseVec;
        trianglesVertex[i + 2] = (trianglesVertex[i + 2] * factor) + rebaseVec;

        Triangle* newTriangle = new Triangle(
            trianglesVertex[i + 0],
            trianglesVertex[i + 1],
            trianglesVertex[i + 2],
            mat,
            true
        );
        if (normals.size() == trianglesVertex.size()) {
            
            newTriangle->setNormalByVertex(normals[i], normals[i + 1], normals[i + 2]);
        }
        addTriangleMesh(newTriangle);
    }
    mainBBox.min = new Vec3f(_min + rebaseVec);
    mainBBox.max = new Vec3f(_max + rebaseVec);
    
    if (meshes.size() == 0) return;
    initBoundingBoxes(0, (int)meshes.size() - 1, mainBBox);
}

void MultiMesh::initBoundingBoxes(int start, int end, BBox& currentBox) {
    if (end == start) {
        //make triangle in bbox
        return;
    }
    Vec3f extents = *currentBox.max - *currentBox.min;
    if ((extents.x > extents.y) && (extents.x > extents.z))
        std::sort(Primitive::objectList.begin() + start, Primitive::objectList.begin() + end, Primitive::sortByPosX);
    if ((extents.y > extents.z) && (extents.y > extents.x))
        std::sort(Primitive::objectList.begin() + start, Primitive::objectList.begin() + end, Primitive::sortByPosY);
    if ((extents.z > extents.y) && (extents.z > extents.x))
        std::sort(Primitive::objectList.begin() + start, Primitive::objectList.begin() + end, Primitive::sortByPosX);
    
    //make bbox and iterate
    //half in each
    //keep min, keep max
}

#define TINYOBJLOADER_IMPLEMENTATION
#include "MultiMesh.hpp"
#include "tiny_obj_loader.h"
#include <memory>
#include <algorithm>

MultiMesh::MultiMesh(const MultiMesh& mm, bool objMat, Material* mat) : Primitive(Vec3f{0,0,0}, nullptr, false) {
    mainBBox.min = mm.mainBBox.min;
    mainBBox.max = mm.mainBBox.max;
    std::vector<Primitive*> meshes = mm.getMeshes();
    for (size_t i = 0; i < meshes.size(); i++) {
        const Triangle* mesh = dynamic_cast<Triangle*>(meshes[i]);
        Triangle* newMesh;
        if (objMat) newMesh = new Triangle(mesh, meshes[i]->mat, true);
        else newMesh = new Triangle(mesh, mat, true);
        addTriangleMesh(newMesh);
    }
    if (meshes.size() == 0) return;
    initBoundingBoxes(0, (int)meshes.size() - 1, mainBBox);
}

MultiMesh::MultiMesh(std::string objFileName, bool objMat, Material* mat) : Primitive(Vec3f{0,0,0}, nullptr, false) {
    loadObjtriangles(objFileName, objMat, mat);
}

void MultiMesh::addTriangleMesh(Triangle* triangle) {
    meshes.emplace_back(triangle);
}

double MultiMesh::raytrace(Ray& ray, Hit& hit) {
    if (meshes.size() == 0) return -1;
    double final = raytraceRecursion(ray, hit, mainBBox);
    return final;
}

double MultiMesh::raytraceRecursion(Ray& ray, Hit& hit, const BBox& currentBox) {
    if (intersection(ray.rayPos + ray.rayDir * offset, ray.rayDir, currentBox)) {
        if (currentBox.mesh != nullptr) {
            return currentBox.mesh->raytrace(ray, hit);
        }
        Hit leftHit;
        Hit rightHit;
        double leftDistance;
        double rightDistance;
        leftDistance = raytraceRecursion(ray, leftHit, *currentBox.left);
        rightDistance = raytraceRecursion(ray, rightHit, *currentBox.right);
        if (leftDistance < rightDistance) {
            if (leftDistance != -1) {
                hit = leftHit;
                return leftDistance;
            }
            else if (rightDistance != -1) {
                hit = rightHit;
                return rightDistance;
            }
        }
        else if (rightDistance < leftDistance) {
            if (rightDistance != -1) {
                hit = rightHit;
                return rightDistance;
            }
            else if (leftDistance != -1) {
                hit = leftHit;
                return leftDistance;
            }
        }
    }
    return -1;
}

double MultiMesh::shadowRaytrace(Ray& ray, Hit& hit, Material& shadowMat, float maxDist){
    shadowMat.ignoreShadow = true;
    shadowMat.diffuse = Color(255,255,255);
    shadowMat.alpha = 0;
    Material currentMat;
    double lastHit = 0;
    double distance = 0;
    while (distance != -1 && shadowMat.alpha < 1) {
        distance = -1;
        lastHit = raytrace(ray, hit);
        if (lastHit != -1 && ((lastHit - maxDist) < offset || maxDist == -1)) {
            hit.object->shadowRaytrace(ray, hit, currentMat);
            distance = lastHit;
            shadowMat.ignoreShadow = false;
            if (currentMat.type == MaterialType::TRANSPARENT) {
                shadowMat.type = MaterialType::TRANSPARENT;
                shadowMat.alpha += currentMat.alpha;
                if (shadowMat.alpha >= 1) {
                    shadowMat.alpha = 1;
                    break;
                }
                shadowMat.diffuse = shadowMat.diffuse&currentMat.diffuse;
                ray.rayPos = hit.fragPos;
            }
            else {
                shadowMat.alpha = 1;
                break;
            }
        }
    }
    return lastHit;
}

bool MultiMesh::intersection(Vec3f rayPos, Vec3f dir, const BBox& currentBox) const {
    Vec3f invdir = dir.invdir();
    
    Vec3f min = currentBox.min;
    Vec3f max = currentBox.max;
    min = min - Vec3f{offset, offset, offset};
    max = max + Vec3f{offset, offset, offset};

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

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmax >= 0; 
}

void MultiMesh::setScale(float newScale) {
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->setScale(newScale);
    }
    reScaleBVH(newScale, mainBBox);
}

void MultiMesh::setTranslate(Vec3f newTranslate) {
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->setTranslate(newTranslate);
    }
    reTranslateBVH(newTranslate, mainBBox);
}

void MultiMesh::reScaleBVH(float newScale, BBox& currentBox) {
    currentBox.min = currentBox.min * newScale;
    currentBox.max = currentBox.max * newScale;
    if (currentBox.mesh == nullptr) {
        reScaleBVH(newScale, *currentBox.left);
        reScaleBVH(newScale, *currentBox.right);
    }
}
void MultiMesh::reTranslateBVH(Vec3f newTranslate, BBox& currentBox) {
    currentBox.min = currentBox.min + newTranslate;
    currentBox.max = currentBox.max + newTranslate;
    if (currentBox.mesh == nullptr) {
        reTranslateBVH(newTranslate, *currentBox.left);
        reTranslateBVH(newTranslate, *currentBox.right);
    }
}

void MultiMesh::loadObjtriangles(std::string objFileName, bool objMat, Material* mat) {

    std::string inputfile = "objFiles/" + objFileName + ".obj";
    tinyobj::ObjReaderConfig reader_config;
    if (objMat) reader_config.mtl_search_path = "objFiles/"; // Path to material files

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
                    _min.x = std::min(vx, _min.x);
                    _max.x = std::max(vx, _max.x);
                    _min.y = std::min(vy, _min.y);
                    _max.y = std::max(vy, _max.y);
                    _min.z = std::min(vz, _min.z);
                    _max.z = std::max(vz, _max.z);
                }
            }
            index_offset += fv;
        }
    }
    
    float minCoord;
    minCoord = std::min(_min.x, _min.y);
    minCoord = std::min(_min.z, minCoord);
    float maxCoord;
    maxCoord = std::min(_max.x, _max.y);
    maxCoord = std::min(_max.z, maxCoord);
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
    
    if (meshes.size() == 0) return;
    initBoundingBoxes(0, (int)meshes.size() - 1, mainBBox);
}

void MultiMesh::initBoundingBoxes(int start, int end, BBox& currentBox) {
    currentBox.min = dynamic_cast<Triangle*>(meshes[start])->min;
    currentBox.max = dynamic_cast<Triangle*>(meshes[start])->max;

    for (int i = start+1; i <= end; i++) {
        const Vec3f& tmin = dynamic_cast<Triangle*>(meshes[i])->min;
        const Vec3f& tmax = dynamic_cast<Triangle*>(meshes[i])->max;

        currentBox.min.x = std::min(currentBox.min.x, tmin.x);
        currentBox.min.y = std::min(currentBox.min.y, tmin.y);
        currentBox.min.z = std::min(currentBox.min.z, tmin.z);

        currentBox.max.x = std::max(currentBox.max.x, tmax.x);
        currentBox.max.y = std::max(currentBox.max.y, tmax.y);
        currentBox.max.z = std::max(currentBox.max.z, tmax.z);
    }

    if (end == start) {
        currentBox.mesh = meshes[start];
        return;
    }
    Vec3f extents = currentBox.max - currentBox.min;
    if ((extents.x > extents.y) && (extents.x > extents.z))
        std::sort(meshes.begin() + start, meshes.begin() + end, Primitive::sortByPosX);
    if ((extents.y > extents.z) && (extents.y > extents.x))
        std::sort(meshes.begin() + start, meshes.begin() + end, Primitive::sortByPosY);
    if ((extents.z > extents.y) && (extents.z > extents.x))
        std::sort(meshes.begin() + start, meshes.begin() + end, Primitive::sortByPosZ);
    
    int midArray = ((end - start) / 2) + start;
    currentBox.left = new BBox();
    initBoundingBoxes(start, midArray, *currentBox.left);
    currentBox.right = new BBox();
    initBoundingBoxes(midArray+1, end, *currentBox.right);
}

#include "Triangle.hpp"

void setMinMax(float value0, float value1, float value2, float& min, float& max);

Triangle::Triangle(const Triangle* t, Material* mat, bool isPartOfObj) : Primitive(t->pos, mat, isPartOfObj), min(t->min), max(t->max), defaultNormal(t->defaultNormal), vertex0(t->vertex0), vertex1(t->vertex1), vertex2(t->vertex2), normalByVertex(t->normalByVertex) {}

Triangle::Triangle(Vec3f vert0, Vec3f vert1, Vec3f vert2, Material* mat, bool isPartOfObj) : Primitive(Vec3f{0,0,0}, mat, isPartOfObj), vertex0(vert0), vertex1(vert1), vertex2(vert2) {
    pos = Vec3f{(vert0.x + vert1.x + vert2.x) / 3.0f, (vert0.y + vert1.y + vert2.y) / 3.0f, (vert0.z + vert1.z + vert2.z) / 3.0f};
    Vec3f edge1 = vert2 - vert0;
    Vec3f edge2 = vert1 - vert0;
    
    //min max of coord
    setMinMax(vert0.x, vert1.x, vert2.x, min.x, max.x);
    setMinMax(vert0.y, vert1.y, vert2.y, min.y, max.y);
    setMinMax(vert0.z, vert1.z, vert2.z, min.z, max.z);
    
    defaultNormal = edge1.cross(edge2).normalize();
}

void Triangle::setNormalByVertex(Vec3f norm0, Vec3f norm1, Vec3f norm2) {
    normalByVertex.push_back(norm0);
    normalByVertex.push_back(norm1);
    normalByVertex.push_back(norm2);
}

double Triangle::raytrace(Ray& ray, Hit& hit) {
    float u = 0;
    float v = 0;
    double d = intersection(ray.rayPos + ray.rayDir * offset, ray.rayDir, u, v);
    Vec3f fragPos = ray.rayPos + ray.rayDir * (float)d;

    hit.object = this;
    hit.fragPos = fragPos;
    hit.normal = normalAtPoint(u, v, ray);
    
    return d;
}

double Triangle::intersection(Vec3f rayPos, Vec3f dir, float& u, float& v) const {
    Vec3f edge1, edge2, h, s, q;
    float a,f;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = dir.cross(edge2);
    a = edge1.dot(h);
    if (a > -offset && a < offset)
        return -1;    // Le rayon est parallèle au triangle.

    f = 1.0/a;
    s = rayPos - vertex0;
    u = f * (s.dot(h));
    if (u < 0.0 || u > 1.0)
        return -1;
    q = s.cross(edge1);
    v = f * dir.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return -1;

    // On calcule t pour savoir ou le point d'intersection se situe sur la ligne.
    float t = f * edge2.dot(q);
    if (t > offset) // Intersection avec le rayon
    {
        return t;
    }
    else // On a bien une intersection de droite, mais pas de rayon.
        return -1;
}

Vec3f Triangle::normalAtPoint(float u, float v, const Ray& ray) const {
    Vec3f normal = defaultNormal;
    if (normalByVertex.size() == 3) {

        float w = 1 - u - v;
        normal = (normalByVertex[0] * w + normalByVertex[1] * u + normalByVertex[2] * v);
    }
    
    return normal * (2 *(retrieveNormalDir(defaultNormal, ray.rayDir)) - 1);
}

void Triangle::setScale(float newScale) {
    vertex0 = vertex0*newScale;
    vertex1 = vertex1*newScale;
    vertex2 = vertex2*newScale;
    
    min = min*newScale;
    max = max*newScale;
}

void Triangle::setTranslate(Vec3f newTranslate) {
    vertex0 = vertex0+newTranslate;
    vertex1 = vertex1+newTranslate;
    vertex2 = vertex2+newTranslate;
    
    min = min+newTranslate;
    max = max+newTranslate;

    pos = pos + newTranslate;
}

void setMinMax(float value0, float value1, float value2, float& min, float& max) {
    if (value0 < value1) {// 0 < 1
        min = value0;
        max = value1;
        if (value0 < value2) {// 0 < 1 && 2
            if (value1 < value2) {// 0 < 1 < 2
                max = value2;
            }
            else ;// 0 < 2 < 1
        }
        else {// 2 < 0 < 1
            min = value2;
        }
    }
    else {// 1 < 0
        min = value1;
        max = value0;
        if (value1 < value2) {// 1 < 0 && 2
            if (value0 < value2) {// 1 < 0 < 2
                max = value2;
            }
            else ;// 1 < 2 < 0
        }
        else {// 2 < 1 < 0
            min = value2;
        }
    }
}

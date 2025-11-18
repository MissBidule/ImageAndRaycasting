#include "Triangle.hpp"

Triangle::Triangle(Vec3f vert0, Vec3f vert1, Vec3f vert2, Material mat) : Primitive(Vec3f{0,0,0}, mat), vertex0(vert0), vertex1(vert1), vertex2(vert2) {
    pos = Vec3f{(vert0.x + vert1.x + vert2.x) / 3.0f, (vert0.y + vert1.y + vert2.y) / 3.0f, (vert0.z + vert1.z + vert2.z) / 3.0f};
    Vec3f edge1 = vert2 - vert0;
    Vec3f edge2 = vert1 - vert0;
    normal = edge1.cross(edge2).normalize();
}

double Triangle::raytrace(Vec3f rayPos, Vec3f dir) const {
    return intersection(rayPos + dir * offset, dir);
}

double Triangle::intersection(Vec3f rayPos, Vec3f dir) const {
    Vec3f edge1, edge2, h, s, q;
    float a,f,u,v;
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

Vec3f Triangle::normalAtPoint(Vec3f fragPos, Vec3f rayDir) const {
    return normal * (2 *(retrieveNormalDir(normal, rayDir)) - 1);
}
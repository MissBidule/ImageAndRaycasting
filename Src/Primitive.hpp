#ifndef _PRIMITIVE_HPP_
#define _PRIMITIVE_HPP_

#include <vector>
#include <atomic>
#include "ColorImage.hpp"
#include "Misc.hpp"

class Primitive;
class Triangle;

extern thread_local uint64_t raysLocal;

struct Hit {
    Primitive* object = nullptr;
    Vec3f fragPos;
    Vec3f normal;
};

class Primitive {
    public:
        static std::vector<Primitive*> objectList;
        static Vec3f defaultColor;
        static bool sortByPosX(const Primitive* a, const Primitive* b);
        static bool sortByPosY(const Primitive* a, const Primitive* b);
        static bool sortByPosZ(const Primitive* a, const Primitive* b);

        //will call all primitives to draw
        static ColorImage* draw(const ColorImage& img, Camera cam, int samples = 4);
        virtual void setScale(float newScale) = 0;
        virtual void setTranslate(Vec3f newTranslate) = 0;
        virtual const std::vector<Primitive*>& getMeshes() const; //used for multiMesh
        virtual double shadowRaytrace(Ray& ray, Hit& hit, Material& shadowMat, float maxDist = -1);
        virtual double raytrace(Ray& ray, Hit& hit) = 0;
    
        static std::atomic<uint64_t> rays;

    protected:
        static Vec3f definitiveColor(Ray& ray, Vec3f camPos, int depth = 5);
        Vec3f transparentCalculation(Vec3f fragPos, Vec3f normal, Vec3f camPos, const Ray &ray, int depth) const;
        Vec3f diffuseCalculation(Vec3f fragPos, Vec3f normal, Vec3f camPos, double distance, const Ray& ray) const;
        Vec3f phongColor(Vec3f fragPos, Vec3f normal, Vec3f lightColor, Vec3f lightDir, Vec3f camPos, const Ray& ray, float attenuation) const;
        bool retrieveNormalDir(Vec3f normal, Vec3f rayDir) const;

        Primitive(Vec3f _pos, Material* _mat, bool isPartOfObj = false);

        Vec3f pos;
        Material* mat;

        const float offset = 0.01f;
        const Vec3f globalAmbientLight = Vec3f{0.05f, 0.05f, 0.05f};
};

#endif //_PRIMITIVE_HPP_

#include "Primitive.hpp"
#include <time.h>
#include <omp.h>
#include <random>

std::vector<Primitive*> Primitive::objectList = std::vector<Primitive*>();
std::vector<Light*> Light::lightList = std::vector<Light*>();
Vec3f Primitive::defaultColor = Vec3f{0.2f, 0.2f, 0.9f};

//maybe too costly in time
bool sortByDepth(const Primitive& a, const Primitive& b)
{
    return a.pos.z < b.pos.z;
}
//std::sort(Primitive::ObjectList.begin(), Primitive::ObjectList.end(), sortByDepth);

Primitive::Primitive(Vec3f _pos, Material& _mat) : pos(_pos), mat(&_mat) {
    objectList.emplace_back(this);
}

ColorImage* Primitive::draw(const ColorImage& img, Camera cam, int samples) {
    //bilinear scale ensures the picture is the size of the output camera
    ColorImage* returnImg = img.bilinearScale(cam.width, cam.height);

    //raytrace the shapes for each pixel
    #pragma omp parallel for collapse(2) schedule(dynamic, 8)
    for (int j = 0; j < cam.height; j++) {
        for (int i = 0; i < cam.width; i++) {
            Vec3f colorMix{0, 0, 0};
            
            static thread_local std::mt19937 generator;
            std::uniform_real_distribution<float> distribution(0,1);
            
            for (int k = 0; k < samples; k++)
            {
                float jRand = distribution(generator);
                float iRand = distribution(generator);
                Ray ray;
                if (cam.viewType == ViewType::ORTHO) {
                    ray.rayPos = Vec3f{(cam.viewPos.x - cam.width/2 + i + iRand), - (cam.viewPos.y - cam.height/2 + j + jRand), (cam.viewPos.z)};
                    ray.rayDir = Vec3f{0, 0, 1};
                }
                else {
                    ray.rayPos = cam.viewPos;
                    ray.rayDir = Vec3f{i + iRand - (float)cam.width/2, - (j + jRand) + (float)cam.height/2, (float)cam.FOV}.normalize();
                }
                colorMix = colorMix + Primitive::definitiveColor(ray, cam.viewPos);
            }
            returnImg->pixel(i, j) = Color::colorFromVec3f(colorMix / samples);
        }
    }

    return returnImg;
}

Vec3f Primitive::definitiveColor(Ray& ray, Vec3f camPos, int depth) {
    if (depth <= 0) {
        return defaultColor;
    }
    
    size_t index = 0;
    double distance = -1;
    double temp;
    index = 0;
    distance = -1;
    for (size_t u = 0; u < objectList.size(); u ++) {
        temp = objectList[u]->raytrace(ray);
        if ((temp != -1 && temp < distance) || distance == -1) {
            distance = temp;
            index = u;
        }
    }
    
    if (distance <= 0) {
        return defaultColor;
    }
    
    Vec3f fragPos = ray.rayPos + ray.rayDir * (float)distance;
    Vec3f reflectionDir;
    Vec3f colorMultiply;

    switch (objectList[index]->mat->type)
    {
        case MaterialType::DIFFUSE:
            return objectList[index]->diffuseCalculation(fragPos, camPos, distance, ray);
            
            break;
        case MaterialType::TRANSPARENT:
            colorMultiply = (Vec3f)objectList[index]->mat->ambient * objectList[index]->mat->alpha + Vec3f{1, 1, 1} * (1 - objectList[index]->mat->alpha);
            return objectList[index]->transparentCalculation(fragPos, camPos, ray, depth) * colorMultiply;

            break;
        case MaterialType::REFLECTIVE:
            colorMultiply = (Vec3f)objectList[index]->mat->ambient;
            reflectionDir = (ray.rayDir.reflect(objectList[index]->normalAtPoint(fragPos, ray))).normalize();
            ray.rayPos = fragPos;
            ray.rayDir = reflectionDir;
            return definitiveColor(ray, camPos, --depth) * colorMultiply;

            break;
        default:
            break;
    }

    return defaultColor;
}

Vec3f Primitive::transparentCalculation(Vec3f fragPos, Vec3f camPos, const Ray &ray, int depth) const {

    Vec3f normal = normalAtPoint(fragPos, ray);
    double ri = retrieveNormalDir(normal, ray.rayDir) ? 1.0 / mat->ior : mat->ior;
    double cosTheta = (-ray.rayDir).dot(normal);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    float kr = Material::reflectance(cosTheta, ri);

    Vec3f color;
    Vec3f reflectColor{0,0,0};
    Vec3f refractColor{0,0,0};
    depth -= 1;

    bool cannotRefract = ri * sinTheta > 1.0;


    Vec3f reflectDir = (ray.rayDir.reflect(normal)).normalize();
    Ray reflectRay = {
        fragPos,
        reflectDir
    };
    reflectColor = definitiveColor(reflectRay, camPos, depth);
    if (!cannotRefract) {
        Vec3f refractDir = ray.rayDir.refract(normal, ri);
        Ray refractRay = {
            fragPos,
            refractDir
        };
        refractColor = definitiveColor(refractRay, camPos, depth);
    }
    else kr = 1;

    color = reflectColor * kr + refractColor * (1.0f - kr);

    return color;
}

Vec3f Primitive::diffuseCalculation(Vec3f fragPos, Vec3f camPos, double distance, const Ray& ray) const {    
    //test shadow or light
    Vec3f finalColor{0, 0, 0};
   for (size_t i = 0; i < Light::lightList.size(); i++) {
        // static thread_local std::mt19937 generator;
        // std::uniform_int_distribution<int> distribution(0,(int)Light::lightList.size()-1);
        double temp;
        // const Light currentLight = *Light::lightList[distribution(generator)];
        const Light currentLight = *Light::lightList[i];
        float attenuation = 1.0f;
        Vec3f lightColor = currentLight.color;
        Vec3f lightDir;
        float shadowAlpha = 0;

        if (currentLight.type == LightType::POINT) {
            Vec3f ray = currentLight.pos - fragPos;
            float distance = ray.norm();
            attenuation = 1.0f / (currentLight.constant + currentLight.linear * distance + currentLight.quadratic * (distance * distance));
            for (size_t u = 0; u < objectList.size(); u ++) {
                Ray lightRay = {
                    fragPos,
                    ray.normalize()
                };
                temp = objectList[u]->raytrace(lightRay);
                if (temp != -1 && (temp - distance) < offset) {
                    if (objectList[u]->mat->type == MaterialType::TRANSPARENT) {
                        shadowAlpha += objectList[u]->mat->alpha;
                        if (shadowAlpha >= 1) {
                            shadowAlpha = 1;
                            break;
                        }
                        lightColor = lightColor - ((Vec3f() - (Vec3f)objectList[u]->mat->ambient) * objectList[u]->mat->alpha);
                    }
                    else {
                        shadowAlpha = 1;
                        break;
                    }
                }
            }
            lightDir = ray.normalize();
        }
        else if (currentLight.type == LightType::DIR) {
            lightDir = (- currentLight.pos).normalize();
            for (size_t u = 0; u < objectList.size(); u ++) {
                Ray lightRay = {
                    fragPos,
                    lightDir
                };
                temp = objectList[u]->raytrace(lightRay);
                if (temp != -1) {
                    if (objectList[u]->mat->type == MaterialType::TRANSPARENT) {
                        shadowAlpha += objectList[u]->mat->alpha;
                        if (shadowAlpha >= 1) {
                            shadowAlpha = 1;
                            break;
                        }
                        lightColor = lightColor - ((Vec3f() - (Vec3f)objectList[u]->mat->ambient) * objectList[u]->mat->alpha);
                    }
                    else {
                        shadowAlpha = 1;
                        break;
                    }
                }
            }
        }

        //final color
        finalColor = finalColor + phongColor(fragPos, lightColor, lightDir, camPos, ray, attenuation) * (1 - shadowAlpha) + lightColor * (Vec3f)mat->ambient * shadowAlpha * attenuation;
        
        // float pdf = 1.0f / Light::lightList.size();
    }
    // return finalColor / pdf;
    return finalColor;
}

bool Primitive::retrieveNormalDir(Vec3f normal, Vec3f rayDir) const {
    return rayDir.dot(normal) <= 0;
}

Vec3f Primitive::phongColor(Vec3f fragPos, Vec3f lightColor, Vec3f lightDir, Vec3f camPos, const Ray& ray, float attenuation) const {
    Vec3f normal = normalAtPoint(fragPos, ray);
    Vec3f matAmbient = mat->ambient;
    Vec3f matDiffuse = mat->diffuse;
    Vec3f matSpecular = mat->specular;
    
    //ambient
    Vec3f ambient = lightColor * matAmbient * attenuation;
    
    //diffuse
    float diff = std::max(normal.dot(lightDir), 0.0f);
    Vec3f diffuse = lightColor * matDiffuse * diff * attenuation;
    
    //specular
    Vec3f viewDir = (camPos - fragPos).normalize();
    Vec3f reflectDir = (-lightDir).reflect(normal);
    float spec = std::pow(std::max(viewDir.dot(reflectDir), 0.0f), mat->shininess * 128);
    Vec3f specular = matSpecular * lightColor * spec * attenuation;

    return ambient + diffuse + specular;
}

#include "Primitive.hpp"
#include <time.h>
#include <omp.h>

std::vector<Primitive*> Primitive::objectList = std::vector<Primitive*>();
std::vector<Light*> Light::lightList = std::vector<Light*>();
Vec3f Primitive::defaultColor = Vec3f{1, 0, 1};

//maybe too costly in time
bool sortByDepth(const Primitive& a, const Primitive& b)
{
    return a.pos.z < b.pos.z;
}
//std::sort(Primitive::ObjectList.begin(), Primitive::ObjectList.end(), sortByDepth);

Primitive::Primitive(Vec3f _pos, Material _mat) : pos(_pos), mat(_mat) {
    objectList.emplace_back(this);
}

ColorImage* Primitive::draw(const ColorImage& img, Camera cam, int samples) {
    //bilinear scale ensures the picture is the size of the output camera
    ColorImage* returnImg = img.bilinearScale(cam.width, cam.height);

    //raytrace the shapes for each pixel    
    #pragma omp declare reduction(vec3f_plus : Vec3f : omp_out = omp_out + omp_in) initializer(omp_priv = Vec3f())
    #pragma omp parallel for collapse(2) schedule(dynamic, 8)
    for (int j = 0; j < cam.height; j++) {
        for (int i = 0; i < cam.width; i++) {
            Vec3f colorMix{0, 0, 0};
            float hit = 0;
            
            unsigned int seed = omp_get_thread_num() + time(nullptr);
            
            for (int k = 0; k < samples; k++)
            {
                float jRand = static_cast<float>(rand_r(&seed)) / static_cast<float>(RAND_MAX);
                float iRand = static_cast<float>(rand_r(&seed)) / static_cast<float>(RAND_MAX);
                if (cam.viewType == ViewType::ORTHO) {
                    Vec3f rayOrig{(cam.viewPos.x - cam.width/2 + i + iRand), - (cam.viewPos.y - cam.height/2 + j + jRand), (cam.viewPos.z)};
                    colorMix = colorMix + Primitive::definitiveColor(rayOrig, Vec3f{0, 0, 1}, cam.viewPos);
                }
                else {
                    Vec3f rayDir = Vec3f{i + iRand - (float)cam.width/2, - (j + jRand) + (float)cam.height/2, (float)cam.FOV}.normalize();
                    colorMix = colorMix + Primitive::definitiveColor(cam.viewPos, rayDir, cam.viewPos);
                }
                hit++;
            }
            returnImg->pixel(i, j) = Color::colorFromVec3f(colorMix / (hit > 0.0f ? hit : 1.0f));
        }
    }

    return returnImg;
}

Vec3f Primitive::definitiveColor(Vec3f orig, Vec3f dir, Vec3f camPos, int depth) {
    if (depth <= 0) {
        return defaultColor;
    }
    
    size_t index = 0;
    double distance = -1;
    double temp;
    index = 0;
    distance = -1;
    for (size_t u = 0; u < objectList.size(); u ++) {
        temp = objectList[u]->raytrace(orig, dir);
        if ((temp != -1 && temp < distance) || distance == -1) {
            distance = temp;
            index = u;
        }
    }
    
    if (distance <= 0) {
        return defaultColor;
    }
    
    Vec3f fragPos = orig + dir * (float)distance;
    Vec3f reflectionDir;
    Vec3f colorMultiply;

    switch (objectList[index]->mat.type)
    {
        case MaterialType::DIFFUSE:
            return objectList[index]->diffuseCalculation(fragPos, camPos, distance);
            
            break;
        case MaterialType::TRANSPARENT:
            colorMultiply = (Vec3f)objectList[index]->mat.ambient * objectList[index]->mat.alpha + Vec3f{1, 1, 1} * (1 - objectList[index]->mat.alpha);
            return objectList[index]->transparentCalculation(fragPos, camPos, dir, depth) * colorMultiply;

            break;
        case MaterialType::REFLECTIVE:
            colorMultiply = (Vec3f)objectList[index]->mat.ambient;
            reflectionDir = (dir.reflect(objectList[index]->normalAtPoint(fragPos, dir))).normalize();
            return definitiveColor(fragPos, reflectionDir, camPos, --depth) * colorMultiply;

            break;
        default:
            break;
    }

    return defaultColor;
}

Vec3f Primitive::transparentCalculation(Vec3f fragPos, Vec3f camPos, Vec3f dir, int depth) {

    Vec3f normal = normalAtPoint(fragPos, dir);
    double ri = retrieveNormalDir(normal, dir) ? 1.0 / mat.ior : mat.ior;
    double cosTheta = (-dir).dot(normal);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    float kr = Material::reflectance(cosTheta, ri);

    Vec3f color;
    Vec3f reflectColor{0,0,0};
    Vec3f refractColor{0,0,0};
    depth -= 1;

    bool cannotRefract = ri * sinTheta > 1.0;


    Vec3f reflectDir = (dir.reflect(normal)).normalize();
    reflectColor = definitiveColor(fragPos, reflectDir, camPos, depth);
    if (!cannotRefract) {
        Vec3f refractDir = dir.refract(normal, ri);
        refractColor = definitiveColor(fragPos, refractDir, camPos, depth);
    }
    else kr = 1;

    color = reflectColor * kr + refractColor * (1.0f - kr);

    return color;
}

Vec3f Primitive::diffuseCalculation(Vec3f fragPos, Vec3f camPos, double distance) {    
    //test shadow or light
    Vec3f finalColor = Vec3f{0, 0, 0};
    for (size_t i = 0; i < Light::lightList.size(); i++) {
        double temp;
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
                temp = objectList[u]->raytrace(fragPos, ray.normalize());
                if (temp != -1 && (temp - distance) < offset) {
                    if (objectList[u]->mat.type == MaterialType::TRANSPARENT) {
                        shadowAlpha += objectList[u]->mat.alpha;
                        if (shadowAlpha >= 1) {
                            shadowAlpha = 1;
                            break;
                        }
                        lightColor = lightColor - ((Vec3f() - (Vec3f)objectList[u]->mat.ambient) * objectList[u]->mat.alpha);
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
                temp = objectList[u]->raytrace(fragPos, lightDir);
                if (temp != -1) {
                    if (objectList[u]->mat.type == MaterialType::TRANSPARENT) {
                        shadowAlpha += objectList[u]->mat.alpha;
                        if (shadowAlpha >= 1) {
                            shadowAlpha = 1;
                            break;
                        }
                        lightColor = lightColor - ((Vec3f() - (Vec3f)objectList[u]->mat.ambient) * objectList[u]->mat.alpha);
                    }
                    else {
                        shadowAlpha = 1;
                        break;
                    }
                }
            }
        }

        //final color
        finalColor = finalColor + phongColor(fragPos, lightColor, lightDir, camPos, attenuation) * (1 - shadowAlpha) + lightColor * (Vec3f)mat.ambient * shadowAlpha * attenuation;
    }

    return finalColor;
}

bool Primitive::retrieveNormalDir(Vec3f normal, Vec3f rayDir) const {
    return rayDir.dot(normal) <= 0;
}

Vec3f Primitive::phongColor(Vec3f fragPos, Vec3f lightColor, Vec3f lightDir, Vec3f camPos, float attenuation) {
    Vec3f normal = normalAtPoint(fragPos);
    Vec3f matAmbient = mat.ambient;
    Vec3f matDiffuse = mat.diffuse;
    Vec3f matSpecular = mat.specular;
    
    //ambient
    Vec3f ambient = lightColor * matAmbient * attenuation;
    
    //diffuse
    float diff = std::max(normal.dot(lightDir), 0.0f);
    Vec3f diffuse = lightColor * matDiffuse * diff * attenuation;
    
    //specular
    Vec3f viewDir = (camPos - fragPos).normalize();
    Vec3f reflectDir = (-lightDir).reflect(normal);
    float spec = std::pow(std::max(viewDir.dot(reflectDir), 0.0f), mat.shininess * 128);
    Vec3f specular = matSpecular * lightColor * spec * attenuation;

    return ambient + diffuse + specular;
}
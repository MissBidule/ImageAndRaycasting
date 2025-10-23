#include "Primitive.hpp"

std::vector<Primitive*> Primitive::objectList = std::vector<Primitive*>();
std::vector<Light*> Light::lightList = std::vector<Light*>();

//maybe too costly in time
bool sortByDepth(const Primitive& a, const Primitive& b)
{
    return a.pos.z < b.pos.z;
}
//std::sort(Primitive::ObjectList.begin(), Primitive::ObjectList.end(), sortByDepth);

Primitive::Primitive(Vec3f _pos, Material _mat) : pos(_pos), mat(_mat) {
    objectList.emplace_back(this);
}

ColorImage* Primitive::draw(const ColorImage& img, Camera cam) {
    //bilinear scale ensures the picture is the size of the output camera
    ColorImage* returnImg = img.bilinearScale(cam.width, cam.height);

    //raytrace the shapes for each pixel    
    size_t index = 0;
    double distance = -1;
    double temp;
    for (uint16_t j = 0; j < cam.height; j++) {
        for (uint16_t i = 0; i < cam.width; i++) {
            index = 0;
            distance = -1;
            for (size_t u = 0; u < objectList.size(); u ++) {
                temp = objectList[u]->isViewIntersect(i, j, cam);
                if ((temp != -1 && temp < distance) || distance == -1) {
                    distance = temp;
                    index = u;
                }
            }
            if (distance >= 0) {
                returnImg->pixel(i, j) = Color::colorFromVec3f(objectList[index]->definitiveColor(distance, cam, i, j));
            }
        }
    }

    return returnImg;
}

Vec3f Primitive::definitiveColor(double distance, Camera cam, uint16_t x, uint16_t y) const {
    Vec3f dir;
    Vec3f fragPos;
    if (cam.viewType == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
        fragPos = Vec3f{cam.viewPos.x - cam.width/2 + x, -(cam.viewPos.y - cam.height/2 + y), cam.viewPos.z} + dir * (float)distance;
    }
    else {
        dir = Vec3f{x - (float)cam.width/2, - y + (float)cam.height/2, (float)cam.FOV}.normalize();
        fragPos = cam.viewPos + dir * (float)distance;
    }
    
    //test shadow or light
    Vec3f finalColor = Vec3f{0, 0, 0};
    for (size_t i = 0; i < Light::lightList.size(); i++) {
        double lightDistance = -1;
        double temp;
        const Light currentLight = *Light::lightList[i];
        Vec3f lightColor = currentLight.color;
        Vec3f matAmbient = mat.ambient;
        Vec3f lightDir;
        float attenuation = 1.0f;

        if (currentLight.type == LightType::POINT) {
            Vec3f ray = (fragPos - currentLight.pos);
            float distance = (-ray).norm();
            attenuation = 1.0f / (currentLight.constant + currentLight.linear * distance + currentLight.quadratic * (distance * distance)); 
            for (size_t u = 0; u < objectList.size(); u ++) {
                temp = objectList[u]->raytrace(currentLight.pos, ray.normalize());
                if ((temp != -1 && temp < lightDistance) || lightDistance == -1) {
                    lightDistance = temp;
                }
            }

            Vec3f lightBlock = currentLight.pos + ray.normalize() * (float)lightDistance;
            if ((fragPos - lightBlock).norm() > offset) {
                finalColor = finalColor + lightColor * matAmbient * attenuation;
                continue;
            }
            lightDir = (-ray).normalize();
        }
        else if (currentLight.type == LightType::DIR) {
            lightDir = (- currentLight.pos).normalize();
            bool obstacle = false;
            for (size_t u = 0; u < objectList.size(); u ++) {
                temp = objectList[u]->raytrace(fragPos + lightDir * offset, lightDir);
                if (temp != -1) {
                    obstacle = true;
                    break;
                }
            }
            if (obstacle) {
                finalColor = finalColor + lightColor * matAmbient;
                continue;
            }
        }
        
        Vec3f normal = normalAtPoint(fragPos);
        Vec3f matDiffuse = mat.diffuse;
        Vec3f matSpecular = mat.specular;
        
        //ambient
        Vec3f ambient = lightColor * matAmbient * attenuation;
        
        //diffuse
        float diff = std::max(normal.dot(lightDir), 0.0f);
        Vec3f diffuse = lightColor * matDiffuse * diff * attenuation;
        
        //specular
        Vec3f viewDir = (cam.viewPos - fragPos).normalize();
        Vec3f reflectDir = (-lightDir).reflect(normal);
        float spec = std::pow(std::max(viewDir.dot(reflectDir), 0.0f), mat.shininess * 128);
        Vec3f specular = matSpecular * lightColor * spec * attenuation;
        //final color
        finalColor = finalColor + ambient + diffuse + specular;
    }
    //result
    return finalColor;
}

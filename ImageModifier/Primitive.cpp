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
                returnImg->pixel(i, j) = objectList[index]->definitiveColor(distance, cam, i, j);
            }
        }
    }

    return returnImg;
}

Color Primitive::definitiveColor(double distance, Camera cam, uint16_t x, uint16_t y) const {
    Vec3f dir;
    Vec3f fragPos;
    if (cam.viewType == ViewType::ORTHO) {
        dir = Vec3f{0, 0, 1};
        fragPos = Vec3f{(float)x, (float)y, 0} + dir * (float)distance;
    }
    else {
        dir = Vec3f{x - cam.viewPos.x, y - cam.viewPos.y, cam.FOV - cam.viewPos.z}.normalize();
        fragPos = cam.viewPos + dir * (float)distance;
    }
    
    //test shadow or light
    Vec3f finalColor = Vec3f{0, 0, 0};
    for (size_t i = 0; i < Light::lightList.size(); i++) {
        double lightDistance = -1;
        double temp;
        const Light currentLight = *Light::lightList[i];
        for (size_t u = 0; u < objectList.size(); u ++) {
            temp = objectList[u]->raytrace(currentLight.pos, fragPos);
            if ((temp != -1 && temp < lightDistance) || lightDistance == -1) {
                lightDistance = temp;
            }
        }
        Vec3f lightColor = currentLight.color;
        Vec3f matAmbient = mat.ambient;
        Vec3f lightBlock = currentLight.pos + (fragPos - currentLight.pos).normalize() * (float)lightDistance;
        if ((fragPos - lightBlock).norm() > 0.1f) {
            finalColor = finalColor + lightColor * matAmbient;
            continue;
        }
        
        Vec3f normal = normalAtPoint(fragPos);
        Vec3f matDiffuse = mat.diffuse;
        Vec3f matSpecular = mat.specular;
        
        //ambient
        Vec3f ambient = lightColor * matAmbient;
        
        //diffuse
        Vec3f lightDir = (currentLight.pos - fragPos).normalize();
        float diff = std::max(normal.dot(lightDir), 0.0f);
        Vec3f diffuse = lightColor * matDiffuse * diff;
        
        //specular
        Vec3f viewDir = (cam.viewPos - fragPos).normalize();
        Vec3f reflectDir = (-lightDir).reflect(normal);
        float spec = std::pow(std::max(viewDir.dot(reflectDir), 0.0f), mat.shininess * 128);
        Vec3f specular = matSpecular * lightColor * spec;
        
        //final color
        finalColor = finalColor + ambient + diffuse + specular;
    }
    //result
    return Color::colorFromVec3f(finalColor);
}

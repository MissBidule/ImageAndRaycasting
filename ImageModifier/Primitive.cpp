#include "Primitive.hpp"

std::vector<Primitive*> Primitive::objectList = std::vector<Primitive*>();

//maybe too costly in time
bool sortByDepth(const Primitive& a, const Primitive& b)
{
    return a.pos.z < b.pos.z;
}
//std::sort(Primitive::ObjectList.begin(), Primitive::ObjectList.end(), sortByDepth);

Primitive::Primitive(Vec3f _pos, Material _mat) : pos(_pos), mat(_mat) {
    objectList.emplace_back(this);
};

ColorImage* Primitive::draw(const ColorImage& img, Camera cam, Light light) { /////////////////////////////////////MULTIPLE LIGHTS !!!!!
    //bilinear scale ensures the picture is the size of the output camera
    ColorImage* returnImg = img.bilinearScale(cam.width, cam.height);

    //raytrace the shapes for each pixel    
    int index = 0;
    double distance = -1;
    double temp;
    for (uint16_t j = 0; j < cam.height; j++) {
        for (uint16_t i = 0; i < cam.width; i++) {
            index = 0;
            distance = -1;
            for (size_t u = 0; u < objectList.size(); u ++) {
                temp = objectList[u]->isIntersect(cam.viewPos, i, j, cam);
                if ((temp != -1 && temp < distance) || distance == -1) {
                    distance = temp;
                    index = u;
                }
            }
            if (distance >= 0) {
                returnImg->pixel(i, j) = objectList[index]->definitiveColor(distance, light, cam, i, j);
            }
        }
    }

    return returnImg;
}
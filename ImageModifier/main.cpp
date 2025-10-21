// main.cpp
#include <iostream>
#include <fstream>

#include "Circle.hpp"
#include "GreyImage.hpp"

int main(int argc, char* argv [])
{

    //Exercises with shapes
    {
        //This is our base which shows if something is missing
        ColorImage* img = ColorImage::readJPEG("ifpicture/cropPic.jpg");

        //we create light(s)
        Light pointLight {
            Vec3f{700, 700, 1500},
            Color(150, 150, 150),
            LightType::POINT
        };
        
        //define our primitives and materials
        Material Mc1 {
            Color::colorFromFloat(0.1745, 0.0215, 0.0215),
            Color::colorFromFloat(0.61424, 0.07568, 0.07568),
            Color::colorFromFloat(0.727811, 0.633, 0.633),
            0.6
        };
        Circle c1(Vec3f{400, 400, 2500}, 300, Mc1);

        Material Mc2 {
            Color::colorFromFloat(0.0215, 0.1745, 0.0215),
            Color::colorFromFloat(0.07568, 0.61424, 0.07568),
            Color::colorFromFloat(0.633, 0.727811, 0.633),
            0.6
        };
        Circle c2(Vec3f{600, 650, 2200}, 200, Mc2);

        Material Mc3 {
            Color::colorFromFloat(0.0215, 0.0215, 0.1745),
            Color::colorFromFloat(0.07568, 0.07568, 0.61424),
            Color::colorFromFloat(0.633, 0.633, 0.727811),
            0.6
        };
        Circle c3(Vec3f{200, 350, 2000}, 100, Mc3);
    
        //our camera defines our final view
        Camera cam {
            Vec3f{400, 400, 0},
            ViewType::ORTHO,
            800,
            800,
            1500
        };

        //draw our first picture in orthogonal view
        ColorImage* orthoImg = Primitive::draw(*img, cam, pointLight);
        orthoImg->writeJPEG("ofpicture/cropPicWithCirclesOrtho.jpg", 100);
        
        //change and draw in perspective
        cam.viewType = ViewType::PERSP;
        
        ColorImage* perspImg = Primitive::draw(*img, cam, pointLight);
        perspImg->writeJPEG("ofpicture/cropPicWithCirclesPersp.jpg", 100);
        
        delete img;
        delete orthoImg;
        delete perspImg;
    }
    
    /*{
        //this will copy a colored picture
        std::ifstream fi;
        fi.open("ifpicture/chat.ppm", std::ios::binary);
        ColorImage* img = ColorImage::readPPM(fi);
        std::ofstream fo;
        fo.open("ofpicture/simpleCopyChat.ppm", std::ios::binary);
        img->writePPM(fo);
        //conversion to grey scale
        fo.close();
        GreyImage* imgG = new GreyImage(*img);
        fo.open("ofpicture/colorToGreyChat.pgm", std::ios::binary);
        imgG->writePGM(fo);
        
        delete img;
        delete imgG;
    }
    
    {
        //this uses the TGA format
        std::ifstream fi;
        fi.open("ifpicture/TGAorigChat.tga", std::ios::binary);
        ColorImage* img = ColorImage::readTGA(fi);
        std::ofstream fo;
        fo.open("ofpicture/TGAformatChat.tga", std::ios::binary);
        img->writeTGA(fo);
        
        delete img;
    }
    
    {
        //this will copy a grey picture
        std::ifstream fi;
        fi.open("ifpicture/chat.pgm", std::ios::binary);
        GreyImage* img = GreyImage::readPGM(fi);
        std::ofstream fo;
        fo.open("ofpicture/simpleCopyChat.pgm", std::ios::binary);
        img->writePGM(fo);
        
        //conversion to RGB, useless as is but can be useful to add colors
        fo.close();
        ColorImage* imgC = new ColorImage(*img);
        fo.open("ofpicture/greyToColorChat.ppm", std::ios::binary);
        imgC->writePPM(fo);
        
        delete img;
        delete imgC;
    }
    
    {
        //this will draw on a picture
        std::ifstream fi;
        fi.open("ifpicture/greyChat.ppm", std::ios::binary);
        ColorImage* img = ColorImage::readPPM(fi);
        img->fillRectangle(200, 30, 50, 190, Color(57, 100, 90));
        img->rectangle(120, 140, 100, 50, Color(200, 30, 159));
        std::ofstream fo;
        fo.open("ofpicture/drawnChat.ppm", std::ios::binary);
        img->writePPM(fo);
        
        delete img;
    }
    
    {
        //this will draw on an empty picture
        GreyImage* img = new GreyImage(320, 240);
        img->clear(200);
        img->rectangle(120, 140, 100,50, 0);
        img->line(0, 180, 300, 0, 159);
        std::ofstream fo;
        fo.open("ofpicture/drawnCanvas.pgm", std::ios::binary);
        img->writePGM(fo);
    }
    
    {
        //this will scale using two different methods
        std::vector<GreyImage*> greyImg;
        std::vector<ColorImage*> colorImg;
        
        std::ifstream fi;
        fi.open("ifpicture/dessin1.pgm", std::ios::binary);
        greyImg.emplace_back(GreyImage::readPGM(fi));
        fi.close();
        fi.open("ifpicture/dessin2.pgm", std::ios::binary);
        greyImg.emplace_back(GreyImage::readPGM(fi));
        fi.close();
        fi.open("ifpicture/dessin3.pgm", std::ios::binary);
        greyImg.emplace_back(GreyImage::readPGM(fi));
        fi.close();
        fi.open("ifpicture/dessin1.ppm", std::ios::binary);
        colorImg.emplace_back(ColorImage::readPPM(fi));
        fi.close();
        fi.open("ifpicture/dessin2.ppm", std::ios::binary);
        colorImg.emplace_back(ColorImage::readPPM(fi));
        fi.close();
        fi.open("ifpicture/dessin3.ppm", std::ios::binary);
        colorImg.emplace_back(ColorImage::readPPM(fi));
        
        greyImg.emplace_back(greyImg[0]->simpleScale(800,700));
        greyImg.emplace_back(greyImg[1]->simpleScale(800,700));
        greyImg.emplace_back(greyImg[2]->bilinearScale(800,700));
        colorImg.emplace_back(colorImg[0]->simpleScale(800,700));
        colorImg.emplace_back(colorImg[1]->simpleScale(800,700));
        colorImg.emplace_back(colorImg[2]->bilinearScale(800,700));

        std::ofstream fo;
        fo.open("ofpicture/dessin1.pgm", std::ios::binary);
        greyImg[3]->writePGM(fo);
        fo.close();
        fo.open("ofpicture/dessin2.pgm", std::ios::binary);
        greyImg[4]->writePGM(fo);
        fo.close();
        fo.open("ofpicture/dessin3.pgm", std::ios::binary);
        greyImg[5]->writePGM(fo);
        fo.close();
        fo.open("ofpicture/dessin1.ppm", std::ios::binary);
        colorImg[3]->writePPM(fo);
        fo.close();
        fo.open("ofpicture/dessin2.ppm", std::ios::binary);
        colorImg[4]->writePPM(fo);
        fo.close();
        fo.open("ofpicture/dessin3.ppm", std::ios::binary);
        colorImg[5]->writePPM(fo);
        fo.close();
        
        for (GreyImage* img : greyImg) {
            delete img;
        }
        for (ColorImage* img : colorImg) {
            delete img;
        }
    }
	
    {
        //this uses the jpeg format, scales it and compressed it
        ColorImage* img = ColorImage::readJPEG("ifpicture/cropPic.jpg");
        ColorImage* scaleImg = img->bilinearScale(1920, 1080);
        scaleImg->writeJPEG("ofpicture/cropPic75quality.jpg", 75);
        
        delete img;
        delete scaleImg;
    }*/

	return 0;
}

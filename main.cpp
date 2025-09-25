// main.cpp
#include <iostream>
#include <fstream>

#include "Image.hpp"

int main()
{/////////////////////////VERIFIER LES ALLOCATIONS
	/*std::ifstream fi;
	fi.open("chat.pgm", std::ios::binary);
	GrayImage* img=GrayImage::readPGM(fi);
	fi.close();
	GrayImage* img=new GrayImage(320, 240);
	img->rectangle(120, 140, 100,50, 0);
	std::ofstream fo;
	fo.open("ImageTest.PGM", std::ios::binary);
	img->writePGM(fo);
	fo.close();
	return 0;*/

	/*std::ifstream fi;
	fi.open("chat.ppm", std::ios::binary);
	ColorImage* img=ColorImage::readPPM(fi);
	fi.close();
	ColorImage* img=new ColorImage(320,240);
	img->rectangle(120, 140, 100, 50, Color(200, 30, 159));
	img->fillRectangle(200, 30, 50, 190, Color(57, 100, 90));
	std::ofstream fo;
	fo.open("ImageTest.PPM", std::ios::binary);
	img->writePPM(fo);
	fo.close();
	return 0;*/

	/*std::ifstream fi;
	fi.open("chat.pgm", std::ios::binary);
	GrayImage* img=GrayImage::readPGM(fi);
	fi.close();
	GrayImage* img=new GrayImage(320, 240);
	img->clear(200);
	img->rectangle(120, 140, 100,50, 0);
	std::ofstream fo;
	fo.open("ImageTest2.PGM", std::ios::binary);
	img->writePGM(fo);
	fo.close();
	return 0;*/

	/*std::ifstream fi;
	fi.open("Dessin1.pgm", std::ios::binary);
	GrayImage* img1=GrayImage::readPGM(fi);
	fi.close();
	fi.open("Dessin2.pgm", std::ios::binary);
	GrayImage* img2=GrayImage::readPGM(fi);
	fi.close();
	fi.open("Dessin3.pgm", std::ios::binary);
	GrayImage* img3=GrayImage::readPGM(fi);
	fi.close();
	fi.open("Dessin1.ppm", std::ios::binary);
	ColorImage* img4=ColorImage::readPPM(fi);
	fi.close();
	fi.open("Dessin2.ppm", std::ios::binary);
	ColorImage* img5=ColorImage::readPPM(fi);
	fi.close();
	fi.open("Dessin3.ppm", std::ios::binary);
	ColorImage* img6=ColorImage::readPPM(fi);
	fi.close();
	
	GrayImage* img7=img1->simpleScale(500,500);
	GrayImage* img8=img2->simpleScale(500,500);
	GrayImage* img9=img3->simpleScale(500,500);
	ColorImage* img10=img4->simpleScale(500,500);
	ColorImage* img11=img5->simpleScale(500,500);
	ColorImage* img12=img6->simpleScale(500,500);

	std::ofstream fo;
	fo.open("Dessin1.pgm", std::ios::binary);
	img7->writePGM(fo);
	fo.close();
	fo.open("Dessin2.pgm", std::ios::binary);
	img8->writePGM(fo);
	fo.close();
	fo.open("Dessin3.pgm", std::ios::binary);
	img9->writePGM(fo);
	fo.close();
	fo.open("Dessin1.ppm", std::ios::binary);
	img10->writePPM(fo);
	fo.close();
	fo.open("Dessin2.ppm", std::ios::binary);
	img11->writePPM(fo);
	fo.close();
	fo.open("Dessin3.ppm", std::ios::binary);
	img12->writePPM(fo);
	fo.close();*/
	
	std::ifstream fi;
	fi.open("a", std::ios::binary);
	ColorImage* img1=ColorImage::readPGM(fi);
	fi.close();
	img1->writeJPEG("b.jpg");

	return 0;
}

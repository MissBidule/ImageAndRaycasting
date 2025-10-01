#ifndef _COLOR_IMAGE_HPP_
#define _COLOR_IMAGE_HPP_

#include "Image.hpp"

class GreyImage;

//Colored picture class
class ColorImage : public Image<Color> {

    //Same as GreyImage
    private :
        ColorImage(Image<Color>);
            
    public :
        //same as Image
        ColorImage& operator=(const ColorImage&)=delete;
        ColorImage(uint16_t w, uint16_t h);
        ColorImage(const ColorImage& orig);
        ColorImage(const GreyImage& orig);
        ~ColorImage();
                
        //Reading/writing PPM
        void writePPM(std::ostream& os) const;
        static ColorImage* readPPM(std::istream& is);
        
        //Reading/writing TGA
        void writeTGA(std::ostream& os, bool rle=true) const;
        static ColorImage* readTGA(std::istream& is);

        //Reading/writing JPEG
        void writeJPEG(const char* fname, unsigned int quality=75) const;
        static ColorImage* readJPEG(const char* fname);

        //Same as GreyImage
        ColorImage* simpleScale(uint16_t w, uint16_t h) const;
        ColorImage* bilinearScale(uint16_t w, uint16_t h) const;
};

#endif // _COLOR_IMAGE_HPP_

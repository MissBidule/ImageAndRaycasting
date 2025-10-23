#ifndef _GREY_IMAGE_HPP_
#define _GREY_IMAGE_HPP_

#include "Image.hpp"

class ColorImage;

//Classe d'une image en Noir et Blanc
class GreyImage : public Image<uint8_t> {
    
    //Constructeur réservé à la classe
    private :
        GreyImage(Image<uint8_t>);
        
    public :
        //pareil que Image
        GreyImage& operator=(const GreyImage&)=delete;
        GreyImage(uint16_t w, uint16_t h);
        GreyImage(const GreyImage& orig);
        GreyImage(const ColorImage& orig);
        ~GreyImage();
        
        //Lecture/écriture d'un PGM
        void writePGM(std::ostream& os) const;
        static GreyImage* readPGM(std::istream& is);

        //Créent une instance de GreyImage à partir d'un image<T> modifié
        GreyImage* simpleScale(uint16_t w, uint16_t h) const;
        GreyImage* bilinearScale(uint16_t w, uint16_t h) const;
};

#endif // _GREY_IMAGE_HPP_

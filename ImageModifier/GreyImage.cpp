#include "GreyImage.hpp"
#include "ColorImage.hpp"

GreyImage::GreyImage(Image<uint8_t> temp) :
Image<uint8_t>(temp.width, temp.height)
{
    array=temp.array;
}

GreyImage::GreyImage(uint16_t w, uint16_t h) :
Image<uint8_t>(w, h)
{}

GreyImage::GreyImage(const GreyImage& orig) :
Image<uint8_t>(orig)
{}

GreyImage::GreyImage(const ColorImage& orig) :
Image<uint8_t>(orig.width, orig.height)
{
    for (int x = 0; x < orig.width * orig.height; x++) {
        array[x] = Color::toGreyScale(orig.array[x]);
    }
}

GreyImage::~GreyImage()
{}

//PGM

void GreyImage::writePGM(std::ostream& os) const
{
    os << "P5\n";
    os << "#image sauvegardée avec l'outil de synthèse d'image.\n";
    os << width << ' ' << height << '\n';
    os << "255\n";
    os.write((const char*) &(array[0]), width*height);
}

GreyImage* GreyImage::readPGM(std::istream& is)
{
    char c1, c2;
    is.get(c1);
    is.get(c2);
    //Ici on accepte deux formats de fichiers PGM
    if (!(c1=='P' && (c2=='5' || c2=='2')))
        throw std::runtime_error("Ce n'est pas un fichier PGM.");
    skip_line(is);
    skip_comments(is);
    uint16_t w, h;
    is >> w >> h;
    skip_line(is);
    skip_comments(is);
    uint32_t max;
    is >> max;
    //On n'autorise les images codées uniquement sur un octet de couleur
    if (max!=255)
        throw std::runtime_error("La profondeur n'est pas celle attendue.");
    skip_line(is);
    GreyImage* img=new GreyImage(w,h);
    //Un P5 est codé en octet
    if (c2=='5')
        is.read((char*) &(img->array[0]), img->width*img->height);
    //Un P2 est codé en ASCII
    else
    {
        uint8_t _g;
        for (int64_t i=0; i<img->width*img->height; i++)
        {
            _g=img->Ascii2Uint8(is);
            img->array[i]=(_g);
        }
    }
    
    return img;
}

//SCALE

GreyImage* GreyImage::simpleScale(uint16_t w, uint16_t h) const
{
    //On appelle le constructeur privé de l'image avec le patron modifié
    Image<uint8_t>* temp=Image<uint8_t>::simpleScale(w,h);
    GreyImage* scale=new GreyImage(*temp);
    delete temp;
    return scale;
}

GreyImage* GreyImage::bilinearScale(uint16_t w, uint16_t h) const
{
    //pareil que simpleScale
    Image<uint8_t>* temp=Image<uint8_t>::bilinearScale(w,h);
    GreyImage* scale=new GreyImage(*temp);
    delete temp;
    return scale;
}

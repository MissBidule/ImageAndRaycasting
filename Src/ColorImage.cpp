#include "GreyImage.hpp"
#include "ColorImage.hpp"

ColorImage::ColorImage(Image<Color> temp) :
Image<Color>(temp.width, temp.height)
{
    array=temp.array;
}

ColorImage::ColorImage(uint16_t w, uint16_t h) :
Image<Color>(w, h)
{}

ColorImage::ColorImage(const ColorImage& orig) :
Image<Color>(orig)
{}

ColorImage::ColorImage(const GreyImage& orig) :
Image<Color>(orig.width, orig.height)
{
    for (int x = 0; x < orig.width * orig.height; x++) {
        array[x] = Color::toRGB(orig.array[x]);
    }
}


ColorImage::~ColorImage()
{}

//PPM

void ColorImage::writePPM(std::ostream& os) const
{
    os << "P6\n";
    os << "#image sauvegardée avec l'outil de synthèse d'image.\n";
    os << width << ' ' << height << '\n';
    os << "255\n";
    os.write((const char*) &array[0], width*height*3);
}

ColorImage* ColorImage::readPPM(std::istream& is)
{
    char c1, c2;
    is.get(c1);
    is.get(c2);
    //Ici on accepte deux formats de fichiers PPM
    if (!(c1=='P' && (c2=='6' || c2=='3')))
        throw std::runtime_error("Ce n'est pas un fichier PPM.");
    skip_line(is);
    skip_comments(is);
    uint16_t w, h;
    is >> w >> h;
    skip_line(is);
    skip_comments(is);
    uint32_t max;
    is >> max;
    //Only 1 byte color (3 times here)
    if (max!=255)
        throw std::runtime_error("La profondeur n'est pas celle attendue.");
    skip_line(is);
    ColorImage* img=new ColorImage(w,h);
    //P6 is byte coded
    if (c2=='6')
        is.read((char*) &(img->array[0]), img->width*img->height*3);
    //P3 is ASCII coded
    else
    {
        uint8_t _r, _g, _b;
        for (int64_t i=0; i<img->width*img->height; i++)
        {
            //a color is made of 3 bytes
            _r=img->Ascii2Uint8(is);
            _g=img->Ascii2Uint8(is);
            _b=img->Ascii2Uint8(is);
            img->array[i]=(Color(_r,_g,_b));
        }
    }
    return img;
}

//TGA

void ColorImage::writeTGA(std::ostream& os, bool rle) const
{
    //Data needed at the start of the TGA
    uint8_t entete[]={0,0,2,0,0,0,0,0,0,0,0,0};
    //Tells if the file is compressed
    if (rle)
        entete[2]=10;
    os.write((const char*) entete, 12);
    const char* ptr1=(char*) &width;
    const char* ptr2=(char*) &height;
    os.write(ptr1,2);
    os.write(ptr2,2);
    //RGB
    os.put(24);
    //Picture starts on the top left (easier)
    os.put(32);
    Color* couleurs=new Color[width*height];
    for (int64_t i=0; i<width*height; i++)
    {
        couleurs[i]=array[i];
        //Swap colors from RGB to BGR
        swapBytes(couleurs[i]);
    }
    if (!rle)
        //Uncompressed writing
        os.write((const char*)couleurs, width*height*3);
    else
    {
        //Vector that holds colors not repeated
        //Raw packets
        std::vector<Color>* vectC=new std::vector<Color>;
        //Last color
        Color temp=couleurs[0];
        //Counting iterations of identical color
        uint8_t nb=1;
        for (int64_t i=1; i<width*height; i++)
        {
            //Identical colors less than 128
            if ((couleurs[i]==temp && nb<128) && vectC->empty())
                nb++;
            //First colors not repeated
            else if ((temp!=couleurs[i] && vectC->empty()) && nb==1)
                {
                    //We add temp and the color in the vector
                    vectC->push_back(temp);
                    vectC->push_back(couleurs[i]);
                    temp=couleurs[i];
                }
            //Different colors not repeated (less than 128)
            else if ((temp!=couleurs[i] && vectC->size()<128) && !vectC->empty())
                {
                    //We add the new color in the vector
                    vectC->push_back(couleurs[i]);
                    temp=couleurs[i];
                }
            //Maximum of not repeated colors
            else if (temp!=couleurs[i] && vectC->size()==128)
            {
                    //We write the maximum
                    os.put(127);
                    Color* tabtemp=new Color[128];
                    //We change the type of the array and write it
                    for (uint8_t j=0; j<128; j++)
                        tabtemp[j]=vectC->at(j);
                    os.write((const char*)tabtemp, 128*3);
                    delete [] tabtemp;
                    //We empty the vector
                    vectC->clear();
                    //We add the last color read
                    vectC->push_back(couleurs[i]);
                    temp=couleurs[i];
            }
            //Identical colors but we started a list of not repeated colors
            else if (temp==couleurs[i] && !vectC->empty())
            {
                //As we do not write the last color there's no use for the size 1 vector
                if (vectC->size()>1)
                {
                    //We remove the last color as it is repeated
                    uint8_t n=vectC->size()-1;
                    os.put(n-1);
                    Color* tabtemp=new Color[n];
                    //Same as before
                    for (uint8_t j=0; j<n; j++)
                        tabtemp[j]=vectC->at(j);
                    os.write((const char*)tabtemp, n*3);
                    delete [] tabtemp;
                }
                vectC->clear();
                //We then switch to identical colors
                nb++;
            }
            //This else is for :
            //-128 identical colors
            //-Different colors and nb > 1
            else
            {
                nb+=127;
                os.put(nb);
                char* ptr_temp=(char*) &temp;
                os.write(ptr_temp, 3);
                nb=1;
                temp=couleurs[i];
            }
        }
        //We write the last pixels that only got saved
        if (vectC->size()>0)
        {
            uint8_t n=vectC->size();
            os.put(n-1);
            Color* tabtemp=new Color[n];
            for (uint8_t j=0; j<n; j++)
                tabtemp[j]=vectC->at(j);
            os.write((const char*)tabtemp, n*3);
            delete [] tabtemp;
        }
        else
        {
            nb+=127;
            os.put(nb);
            char* ptr_temp=(char*) &temp;
            os.write(ptr_temp, 3);
        }
        delete vectC;
    }
    delete [] couleurs;
}

ColorImage* ColorImage::readTGA(std::istream& is)
{
    //All variables on a byte that we need
    uint8_t comm, map, type, map_size, pxl_size, start;
    char* ptr_comm=(char*) &comm;
    char* ptr_map=(char*) &map;
    char* ptr_type=(char*) &type;
    char* ptr_map_size=(char*) &map_size;
    char* ptr_pxl_size=(char*) &pxl_size;
    char* ptr_start=(char*) &start;
    
    //All variables on two bytes that we need
    uint16_t map_lgt, w, h;
    char* ptr_map_lgt=(char *) &map_lgt;
    char* ptr_w=(char *) &w;
    char* ptr_h=(char *) &h;
    
    is.get(*ptr_comm);
    is.get(*ptr_map);
    is.get(*ptr_type);
    //We only read TGA of type 1, 2, 9, 10
    if (((type!=1 && type!=2) && type!=9 ) && type!=10)
        throw std::runtime_error("Ce type de données TGA n'est pas pris en charge.");
    //We ignore the map origin
    skip_byte(is, 2);
    is.read(ptr_map_lgt,2);
    is.get(*ptr_map_size);
    //If we have a color map specification, we need the colors to be in 3 bytes
    if (map==1 && map_size!=24)
        throw std::runtime_error("Ce n'est pas une image RGB.");
    //We ignore picture origin
    skip_byte(is, 4);
    is.read(ptr_w,2);
    is.read(ptr_h,2);
    is.get(*ptr_pxl_size);
    //If we don't need map specification, we check colors to be in 3 bytes
    if (map!=1 && pxl_size!=24)
        throw std::runtime_error("Ce n'est pas une image RGB.");
    //If this value is equal to 32, then the picture starts top left, otherwise bottom left
    is.get(*ptr_start);
    skip_byte(is, comm);
    
    ColorImage* img=new ColorImage(w,h);
    
    //Reading by map specification
    if (map==1)
    {
        //Creation of the map
        Color* couleurs=new Color[map_lgt];
        is.read((char*)couleurs, 3*map_lgt);
        //If the color index is on a byte
        if (pxl_size==8)
        {
            uint8_t* index=new uint8_t[w*h];
            //If it's a type one we just read the index
            if (type!=9)
                is.read((char*)index, w*h);
            //If it's a type 9 we take the compression into account
            else
            {
                //Counter of saved colors
                uint32_t n=0;
                uint8_t t;
                char* ptr_t=(char*) &t;
                uint8_t temp;
                char* ptr_temp=(char*) &temp;
                do
                {
                    is.get(*ptr_t);
                    //If it is non repetitive colors
                    if (t<128)
                    {
                        //We take the number of colors
                        t+=1;
                        //Read the index
                        uint8_t* tabtmp=new uint8_t[t];
                        is.read((char*)tabtmp, t);
                        for (uint8_t i=0; i<t; i++)
                            //Write the indexes from n
                            index[i+n]=tabtmp[i];
                        //Update counter
                        n+=t;
                        delete [] tabtmp;
                    }
                    //If it is repetitive
                    else
                    {
                        //We take the number of colors minus 1 byte
                        t-=127;
                        //Read the color index
                        is.get(*ptr_temp);
                        for (uint8_t i=0; i<t; i++)
                            //Write the index t from n
                            index[i+n]=temp;
                        n+=t;
                    }
                } while ((int64_t)n<w*h);
            }
            
            for (int64_t i=0; i<w*h; i++)
                //We retrieve the colors from the saved indexes
                img->array[i]=couleurs[index[i]];
            delete [] index;
        }
        //Same but with 2 bytes indexes
        else if (pxl_size==16)
                {
                    uint16_t* index=new uint16_t[w*h];
                    if (type!=9)
                        is.read((char*)index, w*h*2);
                    else
                    {
                        uint32_t n=0;
                        uint8_t t;
                        char* ptr_t=(char*) &t;
                        uint16_t temp;
                        char* ptr_temp=(char*) &temp;
                        do
                        {
                            is.get(*ptr_t);
                            if (t<128)
                            {
                                t+=1;
                                uint16_t* tabtmp=new uint16_t[t];
                                is.read((char*)tabtmp, t*2);
                                for (uint8_t i=0; i<t; i++)
                                    index[i+n]=tabtmp[i];
                                n+=t;
                                delete [] tabtmp;
                            }
                            else
                            {
                                t-=127;
                                is.read(ptr_temp,2);
                                for (uint8_t i=0; i<t; i++)
                                    index[i+n]=temp;
                                n+=t;
                            }
                        } while ((int64_t)n<w*h);
                    }
                    for (int64_t i=0; i<w*h; i++)
                        img->array[i]=couleurs[index[i]];
                    delete [] index;
                }
        delete [] couleurs;
    }
    //If we do not have map specification
    //If it's a type 2, we simply read the colors
    else if (type!=10)
            is.read((char*) &(img->array[0]), img->width*img->height*3);
        //Whereas with type 10, the compression is taken into account
        else
        {
            uint32_t n=0;
            uint8_t t;
            char* ptr_t=(char*) &t;
            Color temp;
            char* ptr_temp=(char*) &temp;
            do
            {
                is.get(*ptr_t);
                if (t<128)
                {
                    t+=1;
                    Color* tabtmp=new Color[t];
                    is.read((char*)tabtmp, t*3);
                    for (uint8_t i=0; i<t; i++)
                        img->array[i+n]=tabtmp[i];
                    n+=t;
                    delete [] tabtmp;
                }
                else
                {
                    t-=127;
                    is.read(ptr_temp,3);
                    for (uint8_t i=0; i<t; i++)
                        img->array[i+n]=temp;
                    n+=t;
                }
            } while ((int64_t)n<w*h);
        }
        
    //We swap the colors from BGR to RGB
    for (int64_t i=0; i<w*h; i++)
        swapBytes(img->array[i]);
    
    //Modify the array if it's a picture from bottom left
    if (start!=32)
    {
        std::vector<Color> arraytemp(w*h);
        uint32_t u=0;
        for (int32_t i=h-1; i>=0; i--)
            for (uint16_t j=0; j<w; j++)
                arraytemp[u++]=img->array[w*i+j];
        img->array.clear();
        img->array=arraytemp;
    }

    return img;
}

//JPEG

//Cette structure permet de ne pas lancer de exit() en passant par
//la fonction my_error_exit()
struct my_error_mgr {
    jpeg_error_mgr pub;

    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

static void my_error_exit (j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    //Contourne le exit() ce qui permet de gérer l'erreur nous-mêmes
    longjmp(myerr->setjmp_buffer, 1);
}

void ColorImage::writeJPEG(const char* fname, unsigned int quality) const
{
    //gestionnaire d'erreurs
    my_error_mgr jerr;
    //En passant par le nom de fichier on doit créer un FILE
    FILE * outfile;
    //Création du fichier à compresser
    jpeg_compress_struct cinfo;
    //Variable qui va récupérer les parties compressées
    JSAMPROW row_pointer[1];
    //Quantité sauvegardée à chaque itération
    int row_stride;
    //Début de la compression
    jpeg_create_compress(&cinfo);
    if ((outfile = fopen(fname, "wb")) == NULL) {
        throw std::runtime_error("Impossible d'ouvrir le fichier.");
    }
    //On configure les erreurs pour lancer un throw runtime_error
    cinfo.err = jpeg_std_error(&jerr.pub);
      jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
        throw std::runtime_error("Erreur pendant l'écriture du fichier.");
    }
    //On indique la sortie du fichier
    jpeg_stdio_dest(&cinfo, outfile);
    //Définition des paramètres du fichier
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    //Remplissage automatique du reste des paramètres
    jpeg_set_defaults(&cinfo);
    //Niveau de compression du JPEG
    jpeg_set_quality(&cinfo, quality, TRUE);

    //On indique le tableau où sont stockées les données des pixels
    JSAMPLE * ptr_array=(unsigned char*) &array[0];

    //Lancement de la compression
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = width * 3;
    while (cinfo.next_scanline < height) {
        //Prend une partie du tableau pour l'écrire
        row_pointer[0] = & ptr_array[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    //Fin de la compression
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    //Libération de l'espace alloué
    jpeg_destroy_compress(&cinfo);
}

ColorImage* ColorImage::readJPEG(const char* fname)
{
    //pareil que pour l'écriture en grande partie
    my_error_mgr jerr;
    FILE * infile;
    jpeg_decompress_struct cinfo;
    int row_stride;
    if ((infile = fopen(fname, "rb")) == NULL) {
        throw std::runtime_error("Impossible d'ouvrir le fichier.");
    }
    cinfo.err = jpeg_std_error(&jerr.pub);
      jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        throw std::runtime_error("Erreur pendant la lecture du fichier.");
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    ColorImage* img=new ColorImage(cinfo.image_width,cinfo.image_height);
    jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;
    //On indique où l'on souhaite stocker les pixels
    JSAMPLE * ptr_array=(unsigned char*) &(img->array[0]);
    //C'est ici que vont être enregistré chaque ligne
    JSAMPROW* buffer=new JSAMPROW[1];
    while (cinfo.output_scanline < cinfo.output_height) {
        buffer[0]=&ptr_array[cinfo.output_scanline * row_stride];
        jpeg_read_scanlines(&cinfo, buffer, 1);
    }
    jpeg_finish_decompress(&cinfo);
    fclose(infile);
    jpeg_destroy_decompress(&cinfo);
    return img;
}

/////////SCALE

ColorImage* ColorImage::simpleScale(uint16_t w, uint16_t h) const
{
    //pareil que GreyImage
    ColorImage* scale=new ColorImage(*Image<Color>::simpleScale(w,h));
    return scale;
}

ColorImage* ColorImage::bilinearScale(uint16_t w, uint16_t h) const
{
    //pareil que GreyImage
    ColorImage* scale=new ColorImage(*Image<Color>::bilinearScale(w,h));
    return scale;
}



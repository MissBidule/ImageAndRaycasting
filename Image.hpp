#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <vector>
#include <math.h>
#include <setjmp.h>
extern "C" {
 #include <jpeglib.h> //Pour moi : -I/usr/local/include ; -L/usr/local/lib
} //C:\GnuWin32\include ; C:\GnuWin32\lib

#define CORR_PGMASCII
#define CORR_PPMASCII
#define CORR_READCOLORJPEG
#define CORR_WRITECOLORJPEG
#define CORR_READCOLORTGARLE
#define CORR_BRESENHAM
#define CORR_TEMPLATE

const char * const identifier="alidor_l";

const char * const informations=
 "Le test d'ouverture du fichier (fopen) ne se fait pas via write et read sauf pour jpeg.\n"
 "L'écriture et la lecture du fichier JPEG nécessitent le nom suivi de l'extension du fichier.\n"
 "Bibliothèques incluses : <iostream> <vector> <math.h> <setjmp.h> <jpeglib.h>\n"
 "Merci.\n"
;

typedef short unsigned int ushort;

///////////////CLASSES

//Patron des Classes d'images
template <typename T>
class Image {
	public :
		const	uint16_t width, height;
				T *array;
	public :
		//On interdit la création d'une image sans dimension
		//Pour éviter un tableau dynamique vide
		Image()=delete;
		//On interdit l'affectation d'une image dans une autre
		Image& operator=(const Image&)=delete;
		Image(uint16_t w, uint16_t h);
		Image(const Image& orig);
		~Image();

		//Consultation de la largeur ou de la hauteur
		inline  const  uint16_t& getWidth()const{return  width;}
		inline  const  uint16_t& getHeight()const{return  height;}

		//Accesseurs d'un pixel
		T& pixel(uint16_t x, uint16_t y);
		T pixel(uint16_t x, uint16_t y) const;
		
		//Inverse les octets d'une variable
		template<typename U> friend void swapBytes(U& type);
		
		//Transforme un ensemble de caractères en nombre
		uint8_t Ascii2Uint8(std::istream &is);

		//Change la taille d'une image
		Image* simpleScale(uint16_t w, uint16_t h) const;
		Image* bilinearScale(uint16_t w, uint16_t h) const;

		//Dessine un segment
		void line(ushort x1,ushort y1,ushort x2,ushort y2,const T pixel_value);

		//Remplit/vide une image à l'aide d'une couleur
		void clear(T color);

		//Dessine des rectangles
		void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, T color);
		void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, T color);
};

//Classe d'une image en Noir et Blanc
class GrayImage : public Image<uint8_t> {
	
	//Constructeur réservé à la classe
	private :
		GrayImage(Image<uint8_t>);
		
	public :
		//pareil que Image
		GrayImage()=delete;
		GrayImage& operator=(const GrayImage&)=delete;
		GrayImage(uint16_t w, uint16_t h);
		GrayImage(const GrayImage& orig);
		~GrayImage();
		
		//Lecture/écriture d'un PGM
		void writePGM(std::ostream& os) const;		
		static GrayImage* readPGM(std::istream& is);

		//Créent une instance de GrayImage à partir d'un image<T> modifié
		GrayImage* simpleScale(uint16_t w, uint16_t h) const;
		GrayImage* bilinearScale(uint16_t w, uint16_t h) const;
};

//Classe d'une couleur RGB
class Color {
	public :
		uint8_t r, g, b;
		inline Color(uint8_t _r=0, uint8_t _g=0, uint8_t _b=0) :
		r(_r), g(_g), b(_b) {};
		
		//Surcharge d'opérateurs
		friend Color operator*(double alpha, const Color& color);
		friend Color operator+(const Color& c1, const Color& c2);
		friend bool operator==(const Color& c1, const Color& c2);
		friend bool operator!=(const Color& c1, const Color& c2);
};

//Classe d'une Image Colorée
class ColorImage : public Image<Color> {

	//Pareil que GrayImage
	private :
		ColorImage(Image<Color>);
			
	public :
		//Pareil que Image
		ColorImage()=delete;
		ColorImage& operator=(const ColorImage&)=delete;
		ColorImage(uint16_t w, uint16_t h);
		ColorImage(const ColorImage& orig);
		~ColorImage();
				
		//Lecture/écriture d'une image PPM
		void writePPM(std::ostream& os) const;		
		static ColorImage* readPPM(std::istream& is);
		
		//Lecture/écriture d'une image TGA
		void writeTGA(std::ostream& os, bool rle=true) const;
		static ColorImage* readTGA(std::istream& is);

		//Lecture/écriture d'une image JPEG
		void writeJPEG(const char* fname, unsigned int quality=75) const;
		static ColorImage* readJPEG(const char* fname);

		//Pareil que GreyImage
		ColorImage* simpleScale(uint16_t w, uint16_t h) const;
		ColorImage* bilinearScale(uint16_t w, uint16_t h) const;
};

///////////////IMAGE<T>

template<typename T>
Image<T>::Image(uint16_t w, uint16_t h) :
width(w), height(h), array(nullptr)
{
	array=new T[(uint32_t)width*height]; 
}

template<typename T>
Image<T>::Image(const Image& orig) :
width(orig.width), height(orig.height), array(nullptr)
{
	array=new T[(uint32_t)width*height]; 
	for (int64_t i=0; i<width*height; i++)
		array[i]=orig.array[i];
}

template<typename T>
Image<T>::~Image()
{
	delete [] array;
}

//////////////SCALE

template<typename T>
Image<T>* Image<T>::simpleScale(uint16_t w, uint16_t h) const
{
	//On retourne un pointeur d'Image<T> qui sera convertit en
	//une autre classe d'image directement dans les classes
	Image<T>* scale=new Image<T>(w,h);
	//Rapport entre l'ancienne et la nouvelle image
	double ratioW=((double)width)/w;
	double ratioH=((double)height)/h;
	for (uint16_t j=0; j<h; j++)
		for (uint16_t i=0; i<w; i++)
		{
			double x=i*ratioW;
			double y=j*ratioH;
			scale->pixel(i,j)=pixel(uint16_t(x), uint16_t(y));
		}
	return scale;
}

template<typename T>
Image<T>* Image<T>::bilinearScale(uint16_t w, uint16_t h) const
{
	//pareil que simpleScale
	Image<T>* scale=new Image<T>(w,h);
	//Ici, j'essaie de corriger le décalage par rapport
	//à simpleScale avec une modification du calcul du rapport
	double ratioW=((double)width-0.5)/((double)w+0.5);
	double ratioH=((double)height-0.5)/((double)h+0.5);
	for (uint16_t j=0; j<h; j++)
		for (uint16_t i=0; i<w; i++)
		{
			double x=((double)i)*ratioW;
			double y=((double)j)*ratioH;
			//Définition des pixels pour le calcul de la couleur
			uint16_t xi=uint16_t(x);
			uint16_t yj=uint16_t(y);
			uint16_t xip1=(xi+1>=width?xi:xi+1);
			uint16_t yjp1=(yj+1>=height?yj:yj+1);
			double lambda=x-xi;
			double mu=y-yj;
			T p1=pixel(xi,yj);
			T p2=pixel(xip1,yj);
			T p3=pixel(xi,yjp1);
			T p4=pixel(xip1,yjp1);
			//Formule utilisée pour la couleur du pixel final
			scale->pixel(i,j)=(1-lambda)*((1-mu)*p1+mu*p3)+lambda*((1-mu)*p2+mu*p4);
		}
	return scale;
}

///////////////OUTILS DIVERS

template<typename T>
void Image<T>::clear(T color)
{
	for (uint32_t i=0; i<width*height; i++)
		array[i]=color;
}

template<typename T>
void Image<T>::rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, T color)
{
	uint32_t orig=width*y+x;
	for (uint16_t i=0; i<h; i++)
	{	
		//Ecriture des lignes horizontales du rectangle
		if (i==0 || i==h-1)
			for (uint16_t j=0; j<w; j++)
				array[orig+width*i+j]=color;
		//Ecriture des lignes verticales du rectangle
		array[orig+width*i]=color;
		array[orig+width*i+w-1]=color;
	}
}

template<typename T>
void Image<T>::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, T color)
{
	uint32_t orig=width*y+x;
	for (uint16_t i=0; i<h; i++)
		for (uint16_t j=0; j<w; j++)
			array[orig+width*i+j]=color;
}

template<typename T>
void swapBytes(T& type)
{
	char* ptr=reinterpret_cast<char*>(&type);
	char* ptr_end=ptr+sizeof(T)-1;
	//Réecrit du dernier octet au premier
	while (ptr<ptr_end)
	{
		char temp=*ptr_end;
		*ptr_end=*ptr;
		*ptr=temp;
		ptr++;
		ptr_end--;
	}
}

template<typename T>
T& Image<T>::pixel(uint16_t x, uint16_t y)
{
	return array[y*width+x];
}

template<typename T>
T Image<T>::pixel(uint16_t x, uint16_t y) const
{
	return array[y*width+x];
}

template<typename T>
uint8_t Image<T>::Ascii2Uint8(std::istream &is)
{
	std::vector<uint8_t>* color=new std::vector<uint8_t>;
	uint8_t c;
	char* ptr_c=(char *) &c;
	is.get(*ptr_c);
	//On récupère un char jusqu'au séparateur de nombre
	while (c!=10)
	{
		color->push_back(c);
		is.get(*ptr_c);
	}
	c=0;
	for (uint8_t j=0; j<color->size(); j++)
	{
		//On ajoute le chiffre sur sa base 10
		uint8_t b=(pow(10,color->size()-j-1));
		c+=((uint8_t)color->at(j)-48)*b;
	}
	delete color;
	return c;
}

template <typename T>
void Image<T>::line(ushort x1,ushort y1,ushort x2,ushort y2,const T pixel_value)
{
	ushort x=x1;
	ushort y=y1;
	int incaX, incaY;
	int longX, longY;
	if (x2-x1<0)
	{
		//longX doit rester positif, on applique le signe à IncaX
		longX=-(x2-x1);
		incaX=-1;
	}
	else
	{
		longX=(x2-x1);
		incaX=1;
	}
	if (y2-y1<0)
	{
		//longY doit rester positif, on applique le signe à IncaY
		longY=-(y2-y1);
		incaY=-1;
	}
	else
	{
		longY=(y2-y1);
		incaY=1;
	}
	int i=0;

	if (longY<longX)
	{
		const int c1=2*(longY-longX);
		const int c2=2*longY;
		int critere=c2-longX;
		//Ecriture du segment sur l'axe X
		while (i<=longX)
		{
			pixel(x,y)=pixel_value;
			if (critere>=0)
			{
				y+=incaY;
				critere+=c1;
			}
			else
				critere+=c2;
			x+=incaX;
			i++;
		}
	}
	else
	{
		const int c1=2*(longX-longY);
		const int c2=2*longX;
		int critere=c2-longY;
		//Ecriture du segment sur l'axe Y
		while (i<=longY)
		{
			pixel(x,y)=pixel_value;
			if (critere>=0)
			{
				x+=incaX;
				critere+=c1;
			}
			else
				critere+=c2;
			y+=incaY;
			i++;
		}
	}
}

#endif

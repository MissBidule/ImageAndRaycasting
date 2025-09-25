#include "Image.hpp"

//////////////////STATIC METHODS/////////////////////////////

static void skip_line(std::istream& is)
{
	char c;
	do
		is.get(c);
	//'\n' indique une fin de ligne en Ascii
	while (c!='\n');
}

static void skip_byte(std::istream& is, uint8_t n)
{
	//Ignore un certain nombre d'octets
	for (uint8_t i=0; i<n; i++)
		is.get();
}

static void skip_comments(std::istream& is)
{
	char c;
	do
	{
	is.get(c);
	//'#' indique le début d'une ligne de commentaire
	if (c=='#')
			skip_line(is);
	else
		is.putback(c);
	}
	while (c=='#');
}

////////////////GRAY IMAGE/////////////////////////////////

GrayImage::GrayImage(Image<uint8_t> temp) :
Image<uint8_t>(temp.width, temp.height)
{
	array=temp.array;
	temp.array=nullptr;
}

GrayImage::GrayImage(uint16_t w, uint16_t h) :
Image<uint8_t>(w, h)
{}

GrayImage::GrayImage(const GrayImage& orig) :
Image<uint8_t>(orig)
{}

GrayImage::~GrayImage()
{}

////////PGM

void GrayImage::writePGM(std::ostream& os) const
{
	os << "P5\n";
	os << "#image sauvegardée par Lilou ALIDOR pour le TP de Représentation et Codage des Images.\n";
	os << width << ' ' << height << '\n';
	os << "255\n";
	os.write((const char*) array, width*height);
}

GrayImage* GrayImage::readPGM(std::istream& is)
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
	GrayImage* img=new GrayImage(w,h);
	//Un P5 est codé en octet
	if (c2=='5')
		is.read((char*) img->array, img->width*img->height);
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

//////SCALE

GrayImage* GrayImage::simpleScale(uint16_t w, uint16_t h) const
{
	//On appelle le constructeur privé de l'image avec le patron modifié
	Image<uint8_t>* temp=Image<uint8_t>::simpleScale(w,h);
	GrayImage* scale=new GrayImage(*temp);
	delete temp;
	return scale;
}

GrayImage* GrayImage::bilinearScale(uint16_t w, uint16_t h) const
{	
	//pareil que simpleScale
	Image<uint8_t>* temp=Image<uint8_t>::bilinearScale(w,h);
	GrayImage* scale=new GrayImage(*temp);
	delete temp;
	return scale;
}

///////////////////COLOR IMAGE///////////////////////////////

ColorImage::ColorImage(Image<Color> temp) :
Image<Color>(temp.width, temp.height)
{
	array=temp.array;
	temp.array=nullptr;
}

ColorImage::ColorImage(uint16_t w, uint16_t h) :
Image<Color>(w, h)
{}

ColorImage::ColorImage(const ColorImage& orig) :
Image<Color>(orig)
{}

ColorImage::~ColorImage()
{}

////////////PPM

void ColorImage::writePPM(std::ostream& os) const
{
	os << "P6\n";
	os << "#image sauvegardée par Lilou ALIDOR pour le TP de Représentation et Codage des Images.\n";
	os << width << ' ' << height << '\n';
	os << "255\n";
	os.write((const char*) array, width*height*3);
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
	//On n'accepte que des couleurs codées sur un octet (3 fois un octet ici)
	if (max!=255)
		throw std::runtime_error("La profondeur n'est pas celle attendue.");
	skip_line(is);
	ColorImage* img=new ColorImage(w,h);
	//Un P6 est codé en octet
	if (c2=='6')
		is.read((char*) img->array, img->width*img->height*3);
	//Un P3 est codé en octet
	else
	{
		uint8_t _r, _g, _b;
		for (int64_t i=0; i<img->width*img->height; i++)
		{	
			//Une couleur est composée de 3 octets
			_r=img->Ascii2Uint8(is);
			_g=img->Ascii2Uint8(is);
			_b=img->Ascii2Uint8(is);
			img->array[i]=(Color(_r,_g,_b));
		}
	}
	return img;
}

///////////TGA

void ColorImage::writeTGA(std::ostream& os, bool rle) const
{
	//Données qui composent le début du TGA
	uint8_t entete[]={0,0,2,0,0,0,0,0,0,0,0,0};
	//Indique au fichier si il est compressé
	if (rle)
		entete[2]=10;
	os.write((const char*) entete, 12);
	const char* ptr1=(char*) &width;
	const char* ptr2=(char*) &height;
	os.write(ptr1,2);
	os.write(ptr2,2);
	//Image en RGB
	os.put(24);
	//L'image commence en haut à gauche (c'est plus simple)
	os.put(32);
	Color* couleurs=new Color[width*height];
	for (int64_t i=0; i<width*height; i++)
	{
		couleurs[i]=array[i];
		//On inverse les couleurs RGB en BGR
		swapBytes(couleurs[i]);
	}
	if (!rle)
		//Ecriture non compressée
		os.write((const char*)couleurs, width*height*3);
	else
	{
		//Vecteur qui contiendra les couleurs seules
		//Raw packets
		std::vector<Color>* vectC=new std::vector<Color>;
		//Couleur qui sert à comparer avec la suivante
		Color temp=couleurs[0];
		//Compteur de couleurs identiques
		uint8_t nb=1;
		for (int64_t i=1; i<width*height; i++)
		{
			//Couleurs identiques inférieures à 128
			if ((couleurs[i]==temp && nb<128) && vectC->empty())
				nb++;
			//Premières couleurs différentes seules 
			else if ((temp!=couleurs[i] && vectC->empty()) && nb==1)
				{
					//On ajoute temp et la couleur dans le vecteur
					vectC->push_back(temp);
					vectC->push_back(couleurs[i]);
					temp=couleurs[i];
				}
			//Suite des couleurs différentes seules (moins de 128)
			else if ((temp!=couleurs[i] && vectC->size()<128) && !vectC->empty())
				{
					//On ajoute la nouvelle couleur dans le vecteur
					vectC->push_back(couleurs[i]);
					temp=couleurs[i];
				}
			//Suite des couleurs différentes seules (maximum)
			else if (temp!=couleurs[i] && vectC->size()==128)
			{
					//On écrit le nombre de couleurs seules maximum
					os.put(127);
					Color* tabtemp=new Color[128];
					//On transforme le vecteur en tableau
					//Pour faire un write sans problèmes
					for (uint8_t j=0; j<128; j++)
						tabtemp[j]=vectC->at(j);
					os.write((const char*)tabtemp, 128*3);
					delete [] tabtemp;
					//On vide le vecteur de couleurs seules
					vectC->clear();
					//On y met la dernière couleur lue
					vectC->push_back(couleurs[i]);
					temp=couleurs[i];
			}
			//Couleur identique mais on avait commencé une suite
			//de couleurs seules
			else if (temp==couleurs[i] && !vectC->empty())
			{
				//Comme on n'écrit pas la dernière couleur, inutile
				//de se préoccuper du vecteur de taille 1
				if (vectC->size()>1)
				{
					//On retire la dernière couleur qui n'est pas seule
					uint8_t n=vectC->size()-1;
					os.put(n-1);
					Color* tabtemp=new Color[n];
					//pareil, on facilite l'écriture grâce au tableau
					for (uint8_t j=0; j<n; j++)
						tabtemp[j]=vectC->at(j);
					os.write((const char*)tabtemp, n*3);
					delete [] tabtemp;
				}
				vectC->clear();
				//On passe à deux couleurs identiques
				nb++;
			}
			//Ce else prend en compte les deux cas suivants :
			//-Couleurs identiques et compteur = 128
			//-Couleurs différentes et compteur > 1
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
		//On écrit les derniers pixels qui ont été seulement enregistrés
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
	//Toutes les variables sur un octet que j'ai besoin de lire
	uint8_t comm, map, type, map_size, pxl_size, start;
	char* ptr_comm=(char*) &comm;
	char* ptr_map=(char*) &map;
	char* ptr_type=(char*) &type;
	char* ptr_map_size=(char*) &map_size;
	char* ptr_pxl_size=(char*) &pxl_size;
	char* ptr_start=(char*) &start;
	
	//Toutes les variables sur deux octets que j'ai besoin de lire
	uint16_t map_lgt, w, h;
	char* ptr_map_lgt=(char *) &map_lgt;
	char* ptr_w=(char *) &w;
	char* ptr_h=(char *) &h;
	
	is.get(*ptr_comm);
	is.get(*ptr_map);
	is.get(*ptr_type);
	//Je ne lis les fichiers TGA que de type 1, 2, 9, 10
	if (((type!=1 && type!=2) && type!=9 ) && type!=10)
		throw std::runtime_error("Ce type de données TGA n'est pas pris en charge.");
	//On ignore l'origine de la map
	skip_byte(is, 2);
	is.read(ptr_map_lgt,2);
	is.get(*ptr_map_size);
	//Si on a une palette, on a besoin que les couleurs soient en 3 fois un octets
	if (map==1 && map_size!=24)
		throw std::runtime_error("Ce n'est pas une image RGB.");
	//On ignore l'origine de l'image
	skip_byte(is, 4);
	is.read(ptr_w,2);
	is.read(ptr_h,2);
	is.get(*ptr_pxl_size);
	//Seulement si on n'a pas de palette, on vérifie que les couleurs soient en 3 fois un octet
	if (map!=1 && pxl_size!=24)
		throw std::runtime_error("Ce n'est pas une image RGB.");
	//Si cette valeur est égale à 32, alors l'image commence en haut à gauche
	//Sinon en bas à gauche
	is.get(*ptr_start); 
	skip_byte(is, comm);
	
	ColorImage* img=new ColorImage(w,h);
	
	//Lecture des couleurs par palette
	if (map==1)
	{
		//Création et lecture de la palette
		Color* couleurs=new Color[map_lgt];
		is.read((char*)couleurs, 3*map_lgt);
		//Si l'index de couleur est sur un octet
		if (pxl_size==8)
		{
			uint8_t* index=new uint8_t[w*h];
			//Si c'est un type 1, on lit normalement les index
			if (type!=9)
				is.read((char*)index, w*h);
			//Si c'est un type 9, on prend en compte la compression
			else
			{
				//compteur de couleurs enregistrées
				uint32_t n=0;
				uint8_t t;
				char* ptr_t=(char*) &t;
				uint8_t temp;
				char* ptr_temp=(char*) &temp;
				do
				{
					is.get(*ptr_t);
					//Si c'est une suite de couleurs seules
					if (t<128)
					{
						//On prend le nombre de couleurs
						t+=1;
						//Lit les index de couleurs seules
						uint8_t* tabtmp=new uint8_t[t];
						is.read((char*)tabtmp, t);
						for (uint8_t i=0; i<t; i++)
							//Ecrit les index de couleurs seules à partir de n
							index[i+n]=tabtmp[i];
						//Mise à jour du compteur
						n+=t;
						delete [] tabtmp;
					}
					//Si c'est une suite de couleurs identiques
					else
					{
						//On prend le nombre de couleurs (en enlevant le byte 8)
						t-=127;
						//Lit l'index de la couleur
						is.get(*ptr_temp);
						for (uint8_t i=0; i<t; i++)
							//On écrit l'index t fois à partir de n
							index[i+n]=temp;
						n+=t;
					}
				} while ((int64_t)n<w*h);
			}
			
			for (int64_t i=0; i<w*h; i++)
				//On récupère les couleurs en prenant les index enregistrés
				img->array[i]=couleurs[index[i]];
			delete [] index;
		}
		//Pareil mais avec des index sur deux octets
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
	//Si on a pas de palette
	//Si c'est un type 2, on lit simplement les couleurs
	else if (type!=10)
			is.read((char*) img->array, img->width*img->height*3);
		//Si c'est un type 10, on prend en compte la compression de la même
		//manière qu'avec la palette
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
		
	//On inverse le tableau en BGR pour le passer en RGB
	for (int64_t i=0; i<w*h; i++)
		swapBytes(img->array[i]);
	
	//On modifie le tableau si l'image commence en bas à gauche
	if (start!=32)
	{
		Color* arraytemp=new Color[w*h];
		uint32_t u=0;
		for (int32_t i=h-1; i>=0; i--)
			for (uint16_t j=0; j<w; j++)
				arraytemp[u++]=img->array[w*i+j];
		delete [] img->array;
		img->array=arraytemp;
	}

	return img;
}

///////////JPEG

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
	JSAMPLE * ptr_array=(unsigned char*) array;

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
	JSAMPLE * ptr_array=(unsigned char*) img->array;
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
	//pareil que GrayImage
	Image<Color>* temp=Image<Color>::simpleScale(w,h);
	ColorImage* scale=new ColorImage(*temp);
	delete temp;
	return scale;
}

ColorImage* ColorImage::bilinearScale(uint16_t w, uint16_t h) const
{
	//pareil que GrayImage
	Image<Color>* temp=Image<Color>::bilinearScale(w,h);
	ColorImage* scale=new ColorImage(*temp);
	delete temp;
	return scale;
}

/////////////////SURCHARGE D'OPERATEURS//////////////////////

Color operator*(double alpha, const Color& color)
{
	return Color(color.r*alpha, color.g*alpha, color.b*alpha);
}

Color operator+(const Color& c1, const Color& c2)
{
	return Color(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b);
}

bool operator==(const Color& c1, const Color& c2)
{
	return (((c1.r==c2.r) && (c1.g==c2.g)) && (c1.b==c2.b));
}

bool operator!=(const Color& c1, const Color& c2)
{
	return (((c1.r!=c2.r) || (c1.g!=c2.g)) || (c1.b!=c2.b));
}

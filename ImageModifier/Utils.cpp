#include "Image.hpp"

//STATIC METHODS

void skip_line(std::istream& is)
{
	char c;
	do
		is.get(c);
	//'\n' indique une fin de ligne en Ascii
	while (c!='\n');
}

void skip_byte(std::istream& is, uint8_t n)
{
	//Ignore un certain nombre d'octets
	for (uint8_t i=0; i<n; i++)
		is.get();
}

void skip_comments(std::istream& is)
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

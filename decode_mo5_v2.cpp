/*
Convertisseur fichier Basic MO5 Thomson "encodé" en texte lisible Basic MO5
Copyright (C) 2017 David Chardonnet

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

/*
Plateforme de tests:
Windows 7
Cygwin 64
g++ 6.3.0

compilation: g++ 

fichier d'entrée: BU1.MO5
*/

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

unsigned char * lit_fichier_binaire(const std::string &fileName);
int decode_ligne(int i);
int decode_basic(int i);


long g_longueur_fichier;
// Buffer qui contient le fichier BASIC MO5 sous forme "compressée"
unsigned char * buffer;

//*********************************************************************************************
//
// Programme principal
//
//---------------------------------------------------------------------------------------------
int main()
{
	int i;
	int numero_ligne;
	
	std::cout << "Lecture de BU1.MO5" << std::endl;
	
	buffer = lit_fichier_binaire("BU1.MO5");
	
	std::cout << g_longueur_fichier << " octets lus" << std::endl;
	std::cout << std::endl;
	
	// On passe les 5 premiers octets dont on ne comprend pas la signification
	i=5;
	while (i < g_longueur_fichier)
	{
		i= decode_ligne(i);
		// On se positionne sur la ligne suivante
		i+=3;
	}

	delete(buffer);
	std::cout << "\n\nFin de la lecture" << std::endl;
	
	return 0;
}

//***********************************************************************************************
// Appeler cette fonction une fois qu'on est positionné sur le premier octet du numéro de ligne
// i = position dans le buffer
// Renvoie la position de la fin de ligne (position du 0 final)
//-----------------------------------------------------------------------------------------------
int decode_ligne(int i)
{
	int l_numero_ligne;
	
	// Décodage du numéro de ligne - 2 premiers octets
	l_numero_ligne = (((int)buffer[i]) << 8) + (int)buffer[i+1];
	std::cout << l_numero_ligne << " ";
	
	//------------------------
	// Décodage du code BASIC
	//------------------------
	// On se positionne sur le début du code
	i += 2;
	i = decode_basic(i);
	
	return i;
}


//***********************************************************************************************
// Appeler cette fonction une fois qu'on est positionné sur le premier octet du code basic de la 
// ligne à décoder
// i = position dans le buffer
// Renvoie la position de la fin de ligne (position du 0 final)
//-----------------------------------------------------------------------------------------------
int decode_basic(int i)
{
	unsigned char car;
	unsigned char car2;
	std::string code;
	bool flag_for = false;
	bool flag_ligne = false;

	car = buffer[i];
	
	while(car != 0x00)
	{
		if ((car >= 'a' and car <='z') or 
			(car >= 'A' and car <='Z') or 
			(car >= '0' and car <='9') or
			car == ' ' or car == '"'  or car == ':' or car == ';' or 
			car == '.' or car == '-'  or car == '$' or car == ',' or 
			car == '&' or car == '#'  or car == '(' or car == ')' or 
			car == '+' or car == '\'' or car == '?' or car == '%')
		{
			code = car;			
		}
		else if (car == 0xFF)
		{			
			// Code basic encodé sur 2 octets "FFxx"
			i++;
			car2 = buffer[i];
			     if (car2 == 0x80) code = "SGN";
			else if (car2 == 0x81) code = "INT";
			else if (car2 == 0x82) code = "ABS";
			else if (car2 == 0x8E) code = "ASC";
			else if (car2 == 0x99) code = "GR$";
			else if (car2 == 0x9C) code = "MID$";
			else if (car2 == 0x9F) code = "RND";
			else if (car2 == 0xA0) code = "INKEY$";
			else if (car2 == 0xA1) code = "INPUT";
			else if (car2 == 0xA4) code = "SCREEN";
			else if (car2 == 0xA6) code = "PTRIG";
			else
			{				
				code = "**FF Inconnu**";	
				std::cout << "[[[[ff" << std::hex << (int)car2 << "]]]]";
			}
		}
		else if (car == 0x80) code = "END";
		else if (car == 0x81) 
		{
			code = "FOR";
			flag_for = true;
		}
		else if (car == 0x82) code = "NEXT";
		else if (car == 0x84) code = "DIM";
		else if (car == 0x87)
		{			
			i++;
			car2 = buffer[i];
			     if (car2==0xbb) code = "GOTO";
			else if (car2==0xbc) code = "GOSUB";
			else
			{
				code = "**87 Inconnu**";
				std::cout << "[[[[87" << std::hex << (int)car2 <<"]]]]";

			}
		}
		else if (car == 0x89) code = "IF";
		else if (car == 0x8B) code = "RETURN";
		else if (car == 0x8F) code = "ELSE";
		else if (car == 0x93) code = "DEFINT";
		else if (car == 0x9C) code = "LOCATE";
		else if (car == 0x9D) code = "CLS";
		else if (car == 0x9E) code = "CONSOLE";
		else if (car == 0xA3) code = "BEEP";
		else if (car == 0xA4) code = "COLOR";
		else if (car == 0xA5) code = "LINE";
		else if (car == 0xA6) code = "BOX";
		else if (car == 0xA8) code = "ATTRB";
		else if (car == 0xA9) code = "DEF";
		else if (car == 0xAB) code = "PRINT";
		else if (car == 0xAE) code = "CLEAR";
		else if (car == 0xB1) code = "NEW";
		else if (car == 0xB5) code = "OPEN";
		else if (car == 0xB6) code = "CLOSE";
		else if (car == 0xB7) code = "INPEN";
		else if (car == 0xB9) code = "PLAY";
		else if (car == 0xBB)
		{
			if (flag_for == true)
			{
				code = "TO";
				flag_for = false;
			}
			else
			{
				code = "GOTO";
			}
		}
		else if (car == 0xBF) code = "USING";
		else if (car == 0xC4) code = "THEN";
		else if (car == 0xC6) code = "STEP";
		else if (car == 0xC7) code = "+";
		else if (car == 0xC8) code = "-";
		else if (car == 0xC9) code = "*";
		else if (car == 0xCA) code = "/";
		
		else if (car == 0xCC) code = "AND";
		else if (car == 0xCD) code = "OR";
		else if (car == 0xCE) code = "XOR";

		else if (car == 0xD1) code = "MOD";		
		else if (car == 0xD2) code = "@";		
		else if (car == 0xD3) code = ">";		
		else if (car == 0xD4) code = "=";
		else if (car == 0xD5) code = "<";
		//else code = car;
		else
		{	
			// Code inconnu
			code = car;
			std::cout << "[[[[" << std::hex << (int)car << " "<< car <<"]]]]";
		}
		std::cout << code;
		//std::cout << (int)car << code << std::endl;

		i++;
		car = buffer[i];
	}
	std::cout << std::endl;

	return i;	
}

//*****************************************************************************
//
// Lire un fichier binaire et mettre le résultat dans un buffer
// "C-Style"
//-----------------------------------------------------------------------------
unsigned char * lit_fichier_binaire(const std::string &fileName)
{
	FILE * fichier;
	unsigned char * buffer;
	

	// Ouvrir le fichier en mode binaire
	fichier = fopen(fileName.c_str(), "rb");  
	
	// Se positionner à la fin du fichier
	fseek(fichier, 0, SEEK_END);         
	
	// Lire la position du curseur dans le fichier -> longueur du fichier
	g_longueur_fichier = ftell(fichier);
	
	// Retourner en début de fichier
	rewind(fichier);

	// Allocation du buffer avec '\0' terminal
	buffer = (unsigned char *)malloc((g_longueur_fichier+1)*sizeof(char));
	
	// Chargement du fichier dans le buffer
	fread((char*)buffer, g_longueur_fichier, 1, fichier);
	
	// Fermeture du fichier
	fclose(fichier);
	
	return buffer;
}
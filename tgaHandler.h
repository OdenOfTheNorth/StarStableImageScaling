#pragma once
#include <fstream>

#ifndef TGA_HANDLER_H
#define TGA_HANDLER_H

//Created a header file for better organisation

struct ColourData
{
    // Image Data: Pixel RBG(A) values
    char r,g,b,a;

	ColourData(){};
	ColourData(char r, char g, char b, char a = 255) : r(r), g(g), b(b), a(a){};
	
	ColourData operator+ (const ColourData other) const {
		return ColourData(
			r + other.r,
			g + other.g,
			b + other.b,
			a + other.a
		);
	}

	ColourData operator- (const ColourData other) const {
		return ColourData(
			r - other.r,
			g - other.g,
			b - other.b,
			a - other.a
		);
	}
};

//Got header struct from http://www.paulbourke.net/dataformats/tga/
struct TGAHeader {
	char idLength;
	char colourMapType;
	char imageType;
	
    //Color Map Specification:
	short colorMapOrigin;	//word
	short colorMapLength;	//word
	char colorMapEntrySize;
	
    //image specification
	short xOrigin;		//word
	short yOrigin;		//word
	short imageWidth;	//word
	short imageHeight;	//word
	char imagePixelSize;
	char imageDescriptorByte;
};

class tgaHandler
{	
	TGAHeader tgaHeader;	
	public: 
        void CreateAndCopy(char*  INPUT_FILE, char* OUTPUT_FILE, char* ALGORITHM);
    private:
        void ReadFile(std::ifstream& tgaFile);
		void WriteFile(char* OUTPUT_FILE, ColourData* tgaData);
        void CreateFileNearestNeigbour(char* OUTPUT_FILE, ColourData* tgaData);        
		void CreateFileBilinear(char* OUTPUT_FILE, ColourData* tgaData);
        ColourData* ReadColourData(std::ifstream& tgaFile);
		int width, height;
};

#endif // TGA_HANDLER_H
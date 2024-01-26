#include "tgaHandler.h"
#include <iostream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <cmath>

//Local Math Lib

float lerp(float a, float b, float t){
    return (t * a) + (1 - t) * b;
}

ColourData lerpColour(ColourData a, ColourData b, float t){
    return ColourData(
        lerp(a.r, b.r, t), 
        lerp(a.g, b.g, t), 
        lerp(a.b, b.b, t), 
        lerp(a.a, b.a, t)
    );
}

//

void tgaHandler::ReadFile(std::ifstream& tgaFile){
    printf("\nReading File\n");
    tgaFile.seekg(0, std::ios::beg);
    //Read file the order matters
    tgaFile.read(&tgaHeader.idLength, sizeof(tgaHeader.idLength)); 
	tgaFile.read(&tgaHeader.colourMapType, sizeof(tgaHeader.colourMapType)); 
	tgaFile.read(&tgaHeader.imageType, sizeof(tgaHeader.imageType));
	tgaFile.read((char*)(&tgaHeader.colorMapOrigin), sizeof(tgaHeader.colorMapOrigin)); 
	tgaFile.read((char*)(&tgaHeader.colorMapLength), sizeof(tgaHeader.colorMapLength));
	tgaFile.read(&tgaHeader.colorMapEntrySize, sizeof(tgaHeader.colorMapEntrySize));
	tgaFile.read((char*)(&tgaHeader.xOrigin), sizeof(tgaHeader.xOrigin));
	tgaFile.read((char*)(&tgaHeader.yOrigin), sizeof(tgaHeader.yOrigin));
	tgaFile.read((char*)(&tgaHeader.imageWidth), sizeof(tgaHeader.imageWidth));
	tgaFile.read((char*)(&tgaHeader.imageHeight), sizeof(tgaHeader.imageHeight));
	tgaFile.read(&tgaHeader.imagePixelSize, sizeof(tgaHeader.imagePixelSize));
	tgaFile.read(&tgaHeader.imageDescriptorByte, sizeof(tgaHeader.imageDescriptorByte));
}

void tgaHandler::WriteFile(char* OUTPUT_FILE, ColourData* ColourData) {
    // Saving the file to outputFileName(.tga)
    printf("\nSaving '%s'...", OUTPUT_FILE);
    std::ofstream tgaNew(OUTPUT_FILE, std::ios::binary);

    if (!tgaNew.is_open()){
        printf("\nUnable to open file\n");
        return;
    }        
    else {
        // Write new header
        tgaNew.write(&tgaHeader.idLength, sizeof(tgaHeader.idLength));
        tgaNew.write(&tgaHeader.colourMapType, sizeof(tgaHeader.colourMapType));
        tgaNew.write(&tgaHeader.imageType, sizeof(tgaHeader.imageType));
        tgaNew.write((char*)(&tgaHeader.colorMapOrigin), sizeof(tgaHeader.colorMapOrigin));
        tgaNew.write((char*)(&tgaHeader.colorMapLength), sizeof(tgaHeader.colorMapLength));
        tgaNew.write(&tgaHeader.colorMapEntrySize, sizeof(tgaHeader.colorMapEntrySize));
        tgaNew.write((char*)(&tgaHeader.xOrigin), sizeof(tgaHeader.xOrigin));
        tgaNew.write((char*)(&tgaHeader.yOrigin), sizeof(tgaHeader.yOrigin));
        tgaNew.write((char*)(&tgaHeader.imageWidth), sizeof(tgaHeader.imageWidth));
        tgaNew.write((char*)(&tgaHeader.imageHeight), sizeof(tgaHeader.imageHeight));
        tgaNew.write(&tgaHeader.imagePixelSize, sizeof(tgaHeader.imagePixelSize));
        tgaNew.write(&tgaHeader.imageDescriptorByte, sizeof(tgaHeader.imageDescriptorByte));

        for (int i = 0; i < (tgaHeader.imageWidth * tgaHeader.imageHeight); i++) {
            // Write updated pixel information
            tgaNew.write(&ColourData[i].r, sizeof(ColourData[i].r));
            tgaNew.write(&ColourData[i].g, sizeof(ColourData[i].g));
            tgaNew.write(&ColourData[i].b, sizeof(ColourData[i].b));
            if (tgaHeader.imagePixelSize == 32)
                tgaNew.write(&ColourData[i].a, sizeof(ColourData[i].a));
        }
        printf("\nDone.\n");
        tgaNew.close();
    }
}

void tgaHandler::CreateFileBilinear(char* OUTPUT_FILE, ColourData* tgaData) {
    //set new dimensions
    const int newWidth = width / 2;
    const int newHeight = height / 2;

    ColourData* newData = new ColourData[newWidth * newHeight];

    printf("\n\nResizing to %d x %d from %d x %d", newWidth, newHeight, width, height);

    //nestle for loop for readability
    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            // Calculate corresponding position in the original image with scalor
            float ScaleXFactor = (width - 1) / (newWidth - 1);
            float ScaleYFactor = (height - 1) / (newHeight - 1);
            float origX = x * ScaleXFactor; 
            float origY = y * ScaleYFactor; 
            
            //conver to intigers to get a real position
            int x1 = (int)origX;
            int y1 = (int)origY;

            int x2 = std::min(x1 + 1, width - 1);
            int y2 = std::min(y1 + 1, height - 1);

            // Bilinear interpolation
            float a = origX - x1;
            float b = origY - y1;

            // Assuming 4 channels (RGBA)
            ColourData top = lerpColour(tgaData[y1 * width + x1], tgaData[y1 * width + x2], a);
            ColourData bottom = lerpColour(tgaData[y2 * width + x1], tgaData[y2 * width + x2], a);

            newData[y * newWidth + x] = lerpColour(top, bottom, b);
        }
    }

    //set the header to the new width and height
    tgaHeader.imageWidth = newWidth;
    tgaHeader.imageHeight = newHeight;    

    WriteFile(OUTPUT_FILE, newData);

    delete[] newData;
}

void tgaHandler::CreateFileNearestNeigbour(char* OUTPUT_FILE, ColourData* colourData){
    const int newWidth = width / 2;
    const int newHeight = height / 2;

    // Rescaling the image
    printf("\n\nResizing to %d x %d from %d x %d", newWidth, newHeight, width, height);
    ColourData *newData = new ColourData[newWidth * newHeight];

    //nestle for loop for readability
    for (int y = 0; y < newHeight; y++)
    {
        for (int x = 0; x < newWidth; x++)
        {
            float ScaleXFactor = (width - 1) / (newWidth - 1);
            float ScaleYFactor = (height - 1) / (newHeight - 1);
            int origX = x * ScaleXFactor;
            int origY = y * ScaleXFactor;

            int newIndex = y * newWidth + x;
            //Get colourData index
            int origIndex = origY * width + origX;

            // Ensure bounds checking
            if (origX < width && origY < height)
            {
                newData[newIndex].r = colourData[origIndex].r;
                newData[newIndex].g = colourData[origIndex].g;
                newData[newIndex].b = colourData[origIndex].b;

                if (tgaHeader.imagePixelSize == 32)
                {
                    newData[newIndex].a = colourData[origIndex].a;
                }
            }
        }
    }

    //set the header to the new width and height
    tgaHeader.imageWidth = newWidth;
    tgaHeader.imageHeight = newHeight;    

    WriteFile(OUTPUT_FILE, newData);

    delete[] newData; 
}

ColourData* tgaHandler::ReadColourData(std::ifstream& tgaFile)
{
    //Create new color data
    const int imageLength = tgaHeader.imageWidth*tgaHeader.imageHeight;		
	ColourData* colourData = new ColourData[imageLength];

    //read the tgaFile and save to colourData
    for(int i = 0; i < imageLength;i++){
        tgaFile.read(&colourData[i].r,sizeof(colourData[i].r));
        tgaFile.read(&colourData[i].g,sizeof(colourData[i].g));
        tgaFile.read(&colourData[i].b,sizeof(colourData[i].b));
        //Read alpa if it's supported
        if(tgaHeader.imagePixelSize==32)
            tgaFile.read(&colourData[i].a,sizeof(colourData[i].a));
    }

    //debug statment prints if image is RGBA or RGB
    if(tgaHeader.imagePixelSize==32){
        printf("\nRead RGBA");   
    }else{
        printf("\nRead RGB");  
    }
     
    return colourData;
}

void tgaHandler::CreateAndCopy(char*  INPUT_FILE, char* OUTPUT_FILE, char* ALGORITHM){
    
    std::ifstream tgaFile;
    //Open file to read from
    tgaFile.open(INPUT_FILE, std::ios::in | std::ios::binary);
    
    if(!tgaFile.is_open()){
        printf("Could not open file at %s\n", INPUT_FILE);
        return;
    }

    //read tgaFile
    ReadFile(tgaFile);

    //cheak that image is uncompressed and an RGB(A) image
    if(tgaHeader.imageType == 2){
        
        //Skip over idLength and colourmap if it exist
        char buffer[] = "";
        tgaFile.read(buffer, tgaHeader.idLength);
        if (!(tgaHeader.colourMapType)){
            int colourMapSize = tgaHeader.colourMapType * tgaHeader.colorMapLength;
            tgaFile.read(buffer, colourMapSize);
            printf("\nNo Colour Map");
        }
        else
        {
            printf("\nColour Map");
        }

        //Set the width and height of the original tgaHeader
        width = tgaHeader.imageWidth;
        height = tgaHeader.imageHeight;
		
        //Get the colout data from the tgaFile
        ColourData* ColourData = ReadColourData(tgaFile);     

        char NearestNeigbourFileName[256];
        char BilinearFileName[256];
        // add NearestNeigbour_ to modifiedFileName
        strcpy(NearestNeigbourFileName, "NearestNeigbour_");
        // add OUTPUT_FILE to modifiedFileName
        strcat(NearestNeigbourFileName, OUTPUT_FILE);

        // add NearestNeigbour_ to modifiedFileName
        strcpy(BilinearFileName, "Bilinear_");
        // add OUTPUT_FILE to modifiedFileName
        strcat(BilinearFileName, OUTPUT_FILE);

        if(strcmp(ALGORITHM, "n") == 0 || strcmp(ALGORITHM, "N") == 0){
            //changed name to distinguis what algorithm was used
            CreateFileNearestNeigbour(NearestNeigbourFileName, ColourData);
        }else if(strcmp(ALGORITHM, "b") == 0 || strcmp(ALGORITHM, "B") == 0){
            CreateFileBilinear(BilinearFileName, ColourData);
        }else{
            printf("\nNo algorithm was chosen, using both\n");
            CreateFileBilinear(BilinearFileName, ColourData);
            CreateFileNearestNeigbour(NearestNeigbourFileName, ColourData);
        }

        delete[] ColourData;
    }

    tgaFile.close();
}
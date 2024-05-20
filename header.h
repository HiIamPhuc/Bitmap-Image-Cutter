#ifndef header_h
#define header_h

#include <iostream>
#include <stdint.h>
#include <fstream>
#include <iomanip>
#include <cstring>
using namespace std;

#pragma pack(push, 1)
struct HeaderFormat {
	uint16_t signature;
	uint32_t fileSize;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t pxArrayOffset;
};

#pragma pack(push, 1)
struct DibFormat {
	uint32_t dibSize;
	int32_t imgWidth;
	int32_t imgHeight;
	uint16_t colorPlanes;
	uint16_t colorDepth;
	uint32_t compressionAlgo;
	uint32_t pxArraySize;
	int32_t hrzResolution;
	int32_t vtcResolution;
	uint32_t numOfColors;
	uint32_t numOfImpColors;
};

struct Color {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

struct PixelArrayFormat {
	Color** pixels;
	uint32_t nRow;
	uint32_t nCol;

	char paddingSize;
	int lineSize;
	int rawBytesSize;
	unsigned char* rawBytes;
};

struct BitmapImg {
	HeaderFormat header;
	DibFormat dib;
	PixelArrayFormat pixelArray;
};

bool isBitmapFile(fstream& f);

void getHeader(fstream& f, BitmapImg& a);
void getDib(fstream& f, BitmapImg& a);
void getPixelArray(fstream& f, BitmapImg& a);
void convert(PixelArrayFormat& p, int width, int height);
void readBitmapFile(const char* fileName, BitmapImg& a);

void outputBmpInfor(BitmapImg& a);

char* getSuffix(int num);
char* nameDestPartFile(const char* src, int partNum);

void calculatePadLineRaw(BitmapImg& a);

void splitBitmapFile(BitmapImg& a, char* src, int partsInHeight, int partsInWidth);

void deletePointer(BitmapImg& a);

#endif
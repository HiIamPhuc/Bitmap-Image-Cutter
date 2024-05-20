#include "header.h"

bool isBitmapFile(fstream& f) {
	f.seekg(0, ios::beg);

	uint16_t signa;
	f.read((char*)&signa, sizeof(uint16_t));

	if (signa != 0x4d42) {
		return false;
	}
	else {
		return true;
	}
}

void getHeader(fstream& f, BitmapImg& a) {
	f.seekg(0, ios::beg);
	f.read((char*)&a.header, sizeof(HeaderFormat));
}

void getDib(fstream& f, BitmapImg& a) {
	f.seekg(sizeof(HeaderFormat), ios::beg);
	f.read((char*)&a.dib, sizeof(DibFormat));
}

void getPixelArray(fstream& f, BitmapImg& a) {
	calculatePadLineRaw(a);

	a.pixelArray.rawBytes = new unsigned char[a.pixelArray.rawBytesSize];

	f.seekg(a.header.pxArrayOffset, ios::beg);
	f.read((char*)a.pixelArray.rawBytes, a.pixelArray.rawBytesSize);

	convert(a.pixelArray, a.dib.imgWidth, a.dib.imgHeight);
}

void convert(PixelArrayFormat& p, int width, int height) {
	p.nRow = height;
	p.nCol = width;

	p.pixels = new Color * [p.nRow];
	for (int i = 0; i < p.nRow; i++) {
		p.pixels[p.nRow - 1 - i] = (Color*)(p.rawBytes + p.lineSize * i);
	}
}

void readBitmapFile(const char* fileName, BitmapImg& a) {
	fstream f(fileName, ios::binary | ios::in);

	if (!f) {
		cout << "Cannot open file.";
		exit(EXIT_FAILURE);
	}
	else if (isBitmapFile(f) == false) {
		cout << "This is not a bitmap file.";
		exit(EXIT_FAILURE);
	}

	getHeader(f, a);
	getDib(f, a);
	getPixelArray(f, a);

	f.close();

}

void outputBmpInfor(BitmapImg& a) {
	cout << "Bitmap file signature                     :\t" << hex << a.header.signature << endl;
	cout.unsetf(ios_base::hex);
	cout << "File size                                 :\t" << a.header.fileSize << " byte(s)" << endl;
	cout << "Reserved                                  :\t" << a.header.reserved1 << endl;
	cout << "Reserved                                  :\t" << a.header.reserved2 << endl;
	cout << "Pixel Array byte offset                   :\t" << a.header.pxArrayOffset << " byte(s)" << endl;
	cout << "DIB size                                  :\t" << a.dib.dibSize << " byte(s)" << endl;
	cout << "Image width                               :\t" << a.dib.imgWidth << " pixels" << endl;
	cout << "Image height                              :\t" << a.dib.imgHeight << " pixels" << endl;
	cout << "Color planes                              :\t" << a.dib.colorPlanes << endl;
	cout << "Color depth                               :\t" << a.dib.colorDepth << " bits" << endl;
	cout << "Compression algorithm                     :\t" << a.dib.compressionAlgo << endl;
	cout << "Pixel Array size                          :\t" << a.dib.pxArraySize << " byte(s)" << endl;
	cout << "Horizontal resolution                     :\t" << a.dib.hrzResolution << " pixels/m" << endl;
	cout << "Vertical resolution                       :\t" << a.dib.vtcResolution << " pixels/m" << endl;
	cout << "Number of colors in Color Table           :\t" << a.dib.numOfColors << endl;
	cout << "Number of important colors in Color Table :\t" << a.dib.numOfImpColors << endl;
}

char* getSuffix(int num) {
	char* suffix = new char[10];
	suffix = itoa(num, suffix, 10);//convert integral number partNum to char*
	suffix = (char*)realloc(suffix, strlen(suffix) + 1);
	suffix[strlen(suffix)] = '\0';

	return suffix;
}

//create the final part file name
char* nameDestPartFile(const char* src, int partNum) {
	int len_temp = strlen(src) + 1 - 4;//4 is the number of characters of ".bmp"
	char* temp = new char[len_temp];
	int i;
	for (i = 0; i < len_temp - 1; i++) {
		temp[i] = src[i];
	}
	temp[i] = '\0';

	//create a number suffix to add to the destination file name
	char* num = getSuffix(partNum);

	//declare a new string which will be the final destination file name
	char* dest;
	dest = new char[strlen(temp) + 5 + strlen(num) + 1];

	//assign dest1
	strcpy(dest, temp);
	strcat(dest, ".part");
	strcat(dest, num);
	strcat(dest, ".bmp");

	delete[]temp;
	delete[]num;

	return dest;
}

//calculate padding size, line size, size of raw bytes
void calculatePadLineRaw(BitmapImg& a) {
	int width = a.dib.imgWidth * (a.dib.colorDepth / 8);
	a.pixelArray.paddingSize = (4 - (width % 4)) % 4;
	a.pixelArray.lineSize = width + a.pixelArray.paddingSize;
	a.pixelArray.rawBytesSize = a.pixelArray.lineSize * a.dib.imgHeight;
}

void splitBitmapFile(BitmapImg& a, char* src, int partsInHeight, int partsInWidth) {
	int partHeight = a.dib.imgHeight / partsInHeight;
	int partWidth = a.dib.imgWidth / partsInWidth;

	int countPart = 1;
	for (int i = 0; i < partsInHeight; i++) {
		for (int j = 0; j < partsInWidth; j++) {
			//declare a part file
			BitmapImg part;

			//copy the header and dib information of source file to part file
			part.header = a.header;
			part.dib = a.dib;

			part.dib.imgHeight = partHeight;
			part.dib.imgWidth = partWidth;
			part.pixelArray.nRow = partHeight;
			part.pixelArray.nCol = partWidth;

			//calculate some information of pixel array of part file
			int rowStart = i * partHeight;
			int colStart = j * partWidth;

			part.pixelArray.pixels = new Color * [part.pixelArray.nRow];
			for (int k = 0; k < part.pixelArray.nRow; k++) {
				part.pixelArray.pixels[k] = new Color[part.pixelArray.nCol];
			}
			
			//copy data to pixel array of part
			for (int k = 0; k < part.pixelArray.nRow; k++) {
				for (int l = 0; l < part.pixelArray.nCol; l++) {
					part.pixelArray.pixels[k][l] = a.pixelArray.pixels[rowStart + k][colStart + l];
				}
			}

			calculatePadLineRaw(part);

			//transfer data in pixel array to raw bytes
			long long count = 0;
			part.pixelArray.rawBytes = new unsigned char[part.pixelArray.rawBytesSize];
			for (int k = part.pixelArray.nRow - 1; k >= 0; k--) {
				for (int l = 0; l < part.pixelArray.nCol; l++) {
					part.pixelArray.rawBytes[count++] = part.pixelArray.pixels[k][l].blue;
					part.pixelArray.rawBytes[count++] = part.pixelArray.pixels[k][l].green;
					part.pixelArray.rawBytes[count++] = part.pixelArray.pixels[k][l].red;
				}
				for (int l = 1; l <= part.pixelArray.paddingSize; l++) {
					part.pixelArray.rawBytes[count++] = 0;
				}
			}

			char* dest = nameDestPartFile(src, countPart);
			ofstream partFile;
			partFile.open(dest, ios::binary);
			if (!partFile) {
				cout << "Cannot open " << dest;
				return;
			}

			partFile.write((char*)&part.header, sizeof(part.header));
			partFile.write((char*)&part.dib, sizeof(part.dib));
			partFile.write((char*)part.pixelArray.rawBytes, part.pixelArray.rawBytesSize);

			cout << "Copy successfully part file no." << countPart << endl;

			countPart++;

			partFile.close();

			for (int k = 0; k < part.pixelArray.nRow; k++) {
				delete[]part.pixelArray.pixels[k];
			}
			delete[]part.pixelArray.pixels;
			delete[]part.pixelArray.rawBytes;
		}
	}
}

void deletePointer(BitmapImg& a) {
	delete[]a.pixelArray.rawBytes;
	delete[]a.pixelArray.pixels;
}
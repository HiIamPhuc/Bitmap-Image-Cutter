#include "header.h"

int main(int argc, char** argv) {
	BitmapImg a;

	if (argc == 3) {
		cout << "Invalid syntax.";
		return 1;
	}

	char* src;
	src = new char[strlen(argv[1]) + 1];
	strcpy(src, argv[1]);

	readBitmapFile(src, a);

	int partsInHeight = 1, partsInWidth = 1;
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			partsInHeight = atoi(argv[i + 1]);
		}
		if (strcmp(argv[i], "-w") == 0) {
			partsInWidth = atoi(argv[i + 1]);
		}
	}

	if (a.dib.colorDepth != 24) {
		cout << "Pardon! This program can split just 24-bit image.";
		return 0;
	}

	splitBitmapFile(a, src, partsInHeight, partsInWidth);

	delete[]src;
	deletePointer(a);
	return 0;
}
/*
 * StringBitmap.hpp
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 *  Helper-class for the now scrapped demo-application.
 *  Nothing to see here, move along.
 */

#ifndef STRINGBITMAP_HPP_
#define STRINGBITMAP_HPP_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class StringBitmap {
	char* bitmap;
	int width, height;
public:

	StringBitmap(int width, int height) {
		bitmap = (char*)malloc(sizeof(char) * (width + 1) * height + 1); //To have space for the '\0'
		this->width = width + 1;
		this->height = height;
		clear();
	}

	bool boundsCheck(int x, int y) {
		return boundsX(x) && boundsY(y);
	}

	inline bool boundsX(int x) { return 0 <= x && x < width - 1; }

	inline bool boundsY(int y) { return 0 <= y && y < height; }

	void drawDot(int x, int y, char c) {
		if(boundsCheck(x,y)) {
			bitmap[y * width + x] = c;
		}
	}

	void drawString(const char* str, int x, int y) {
		for(int i = 0; str[i] != '\0'; i++) {
			drawDot(x + i, y, str[i]);
		}
	}

	void clear() {
		memset(bitmap, ' ', width * height);
		for(int i = 0; i < height; i++) bitmap[width - 1 + i * width] = '\n';
		bitmap[width * height] = '\0';
	}

	void printBitmap() {
		printf(bitmap);
	}
};



#endif /* STRINGBITMAP_HPP_ */

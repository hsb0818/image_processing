#pragma once

#include <iostream>
#include <algorithm>

#define SAFE_DELETE(x) if (x != nullptr) { delete (x); }
#define SAFE_DELETE_ARRAY(x) if (x != nullptr) { delete[] (x); }

using namespace std;

typedef unsigned char BYTE;

class Filter
{
public:
	static void Grayscale(FILE* FO, const BYTE* const in, const int W, const int H);
	static void Mirroring(FILE* FO, const BYTE* const in, const int W, const int H);
	static void Flipping(FILE* FO, const BYTE* const in, const int W, const int H);
	static void NearestNeighbor(FILE* FO, const BYTE* const in, const int exp, const bool upsample, const int W, const int H);
	static void BilinearInterpolation(FILE* FO, const BYTE* const in, const int exp, const int W, const int H);
	static void Quadratic(FILE* FO, const BYTE* const in, const int exp, const int W, const int H);
};
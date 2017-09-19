#include <iostream>
#include "Filter.h"

#define W 256
#define H 256
#define MW 64
#define MH 64

using namespace std;

int main()
{	
	cout << ("1 : Bilinear\n2 : Quadratic\n") << endl;
	
	int n = 0;
	cin >> n;

	FILE* FI = nullptr;
	FILE* FO = nullptr;
	BYTE* mem_in = nullptr;

	if (n == 1)
	{
		mem_in = new BYTE[W * H];

		fopen_s(&FI, "lena.raw", "rb");
		fopen_s(&FO, "lena_out1.raw", "wb");
		fread(mem_in, sizeof(BYTE), H * W, FI);

		Filter::BilinearInterpolation(FO, mem_in, 2, W, H);
	}
	else
	{
		mem_in = new BYTE[MW * MH];

		fopen_s(&FI, "lena_LR.raw", "rb");
		fopen_s(&FO, "lena_out2.raw", "wb");
		fread(mem_in, sizeof(BYTE), MH * MW, FI);

		Filter::Quadratic(FO, mem_in, 4, MW, MH);
	}
	
	SAFE_DELETE_ARRAY(mem_in);
	fclose(FI);
	fclose(FO);

	return 0;
}
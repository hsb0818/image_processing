#include "Filter.h"

void Filter::Grayscale(FILE* FO, const BYTE* const in, const int W, const int H)
{
	BYTE* out = new BYTE[W * H];
	for (int i = 0; i<H; i++)
	{
		for (int j = 0; j<W; j++)
			out[i * W + j] = 255 - in[i * W + j];
	}

	fwrite(out, sizeof(BYTE), W * H * sizeof(BYTE), FO);
	SAFE_DELETE_ARRAY(out);
}

void Filter::Mirroring(FILE* FO, const BYTE* const in, const int W, const int H)
{
	BYTE* out = new BYTE[W * H];
	for (int i = 0; i<H; i++)
	{
		for (int j = 0; j<W; j++)
			out[i * W + j] = in[i * W + (W - 1) - j];
	}

	fwrite(out, sizeof(BYTE), W * H * sizeof(BYTE), FO);
	SAFE_DELETE_ARRAY(out);
}

void Filter::Flipping(FILE* FO, const BYTE* const in, const int W, const int H)
{
	BYTE* out = new BYTE[W * H];
	for (int i = 0; i<H; i++)
	{
		for (int j = 0; j<W; j++)
			out[i * W + j] = in[(H - 1 - i) * W + j];
	}

	fwrite(out, sizeof(BYTE), W * H * sizeof(BYTE), FO);
	SAFE_DELETE_ARRAY(out);
}

void Filter::NearestNeighbor(FILE* FO, const BYTE* const in, const int exp, const bool upsample, const int W, const int H)
{
	if (upsample)
	{
		const int CW = W * exp;
		const int CH = H * exp;		

		BYTE* out = new BYTE[CW * CH];
		for (int i = 0; i<CH; i++)
		{
			for (int j = 0; j<CW; j++)
				out[i * CW + j] = in[(i / exp) * W + (j / exp)];
		}

		fwrite(out, sizeof(BYTE), CW * CH * sizeof(BYTE), FO);
	}
	else
	{
		const int CW = W / exp;
		const int CH = H / exp;

		BYTE* out = new BYTE[CW * CH];
		for (int i = 0; i<CH; i++)
		{
			for (int j = 0; j<CW; j++)
				out[i * CW + j] = in[(i * W) * exp + (j * exp)];
		}

		fwrite(out, sizeof(BYTE), CW * CH * sizeof(BYTE), FO);
	}
}

void Filter::BilinearInterpolation(FILE* FO, const BYTE* const in, const int exp, const int W, const int H)
{
	auto func = [&W, &H](const BYTE* const in, const int x, const int y, const int n)
	{
		if (x%n == 0 && y%n == 0)
			return in[y / n * W + x / n];

		float cx = (float)x / (float)n;
		float cy = (float)y / (float)n;

		BYTE A = in[y / n*W + x / n];
		BYTE B = in[y / n*W + (x / n + 1)];
		BYTE C = in[(y / n + 1)*W + x / n];
		BYTE D = in[(y / n + 1)*W + (x / n + 1)];

		float a = cx - float(int(x / n));
		float b = cy - float(int(y / n));

		BYTE P1 = (A + a * (B - A));
		BYTE P2 = (C + a * (D - C));
		BYTE X = (P1 + b * (P2 - P1));

		return X;
	};

	const int CW = W * exp;
	const int CH = H * exp;

	BYTE* out = new BYTE[CW * CH];
	for (int i = 0; i < CH; i++)
	{
		for (int j = 0; j < CW; j++)
			out[i * CW + j] = func(in, j, i, exp);
	}

	fwrite(out, sizeof(BYTE), CW * CH * sizeof(BYTE), FO);
}

void Filter::Quadratic(FILE* FO, const BYTE* const in, const int exp, const int W, const int H)
{
	if (W < 3 || H < 3)
		return;

	auto func = [=](int x0, int x1, int x2, int x, int y)
	{
		int i0 = y / exp * W + x0 / exp;
		int i1 = y / exp * W + x1 / exp;
		int i2 = y / exp * W + x2 / exp;
		int i = y / exp * W + x / exp;

		BYTE f0 = in[i0];
		BYTE f1 = in[i1];
		BYTE f2 = in[i2];

		int b0 = f0;
		int b1 = (i1 - i0 != 0) ? (f1 - f0) / (i1 - i0) : 0;
		int b2 = ((i2 - i1) != 0 && (i1 - i0) != 0 && (i2 - i0) != 0) 
			? (((f2 - f1) / (i2 - i1)) - ((f1 - f0) / (i1 - i0))) / (i2 - i0) 
			: 0;
		
		return b0 + b1*(i - i0) + b2*(i - i0)*(i - i1);
	};

	const int CW = W * exp;
	const int CH = H * exp;

	BYTE* out = new BYTE[CW * CH];
	for (int i = 0; i < CH; i++)
	{
		for (int j = 0; j < CW; j += 3)
		{
			int base = j - (j % 3);
			if (base + 3 > CW)
				base = CW - 3;

			out[i * CW + j] = func(base, base + 1, base + 2, j, i);
			out[i * CW + (j + 1)] = func(base, base + 1, base + 2, j + 1, i);
			out[i * CW + (j + 2)] = func(base, base + 1, base + 2, j + 2, i);
		}
	}

	fwrite(out, sizeof(BYTE), CW * CH * sizeof(BYTE), FO);
}
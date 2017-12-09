#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "Motion.h"

#define height 240
#define width 352

const int BLOCK_SIZE = 2;
const int SEARCH_RANGE = 2;

typedef unsigned char BYTE;

BYTE prev_img[6*6] = {
	 20, 30, 30, 30, 40, 40 
	, 25, 30, 30, 40, 45, 50 
	, 81, 85, 91, 95, 45, 50 
	, 80, 85, 90, 96, 55, 55 
	, 50, 55, 55, 55, 60, 60 
	, 50, 50, 50, 50, 50, 50 
};

BYTE next_img[6*6] = {
	 20, 30, 30, 30, 40, 40 
	, 25, 30, 30, 40, 45, 50 
	, 70, 80, 80, 85, 90, 95 
	, 70, 75, 80, 85, 90, 95 
	, 50, 55, 55, 55, 60, 60 
	, 50, 50, 50, 50, 50, 50 
};

typedef unsigned char BYTE;

void main()
{

	FILE *F_I_Prev;
	FILE *F_I_Cur;
	FILE *F_I_Cur_Org;
	FILE *F_I_Next;


	fopen_s(&F_I_Prev, "fb001.raw", "rb");
	fopen_s(&F_I_Cur_Org, "fb002.raw", "rb"); // use only for evaluation
	fopen_s(&F_I_Next, "fb003.raw", "rb");

	fopen_s(&F_I_Cur, "fb002_mc.raw", "wb");

	BYTE* Prev = new BYTE[height*width];
	BYTE* Cur_Org = new BYTE[height*width];
	BYTE* Cur = new BYTE[height*width];
	BYTE* Next = new BYTE[height*width];

	fread(Prev, sizeof(BYTE), height*width, F_I_Prev);
	fread(Cur_Org, sizeof(BYTE), height*width, F_I_Cur_Org);
	fread(Next, sizeof(BYTE), height*width, F_I_Next);

	// define margin (will be 0 for this assignment)
	float total_margin = 0;
	int h_margin = int(float(height) * total_margin * 0.01f * 0.5f);
	int w_margin = int(float(width) *  total_margin * 0.01f * 0.5f);

	/*
	// Apply frame interpolation 
	// Currently, simple averaging is applied. You need to improve this!
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			Cur[h*width + w] = (Prev[h*width + w] + Next[h*width + w] + 1) / 2;
		}
	}
	*/

	Motion motion1(BLOCK_SIZE, SEARCH_RANGE);
	motion1.Estimate(Prev, Next, width, height);
//	motion1.MEMC(Cur, Prev, Next, width, height);

	Motion motion2(BLOCK_SIZE, SEARCH_RANGE);
	motion2.Estimate(Next, Prev, width, height);
//	motion2.MEMC(Cur1, Next, Prev, width, height);

	Motion::FRUC(motion1, motion2, Cur, Prev, Next, width, height);

	float MSE = 0.0f;

	// Compute MSE 
	for (int h = h_margin; h < height - h_margin; h++) {
		for (int w = w_margin; w < width - w_margin; w++) {
			MSE += powf(abs(Cur[h*width + w] - Cur_Org[h*width + w]), 2);
		}
	}
	MSE /= ((height - h_margin * 2) * (width - w_margin * 2));
	printf("MSE : %f \n", MSE);

	fwrite(Cur, sizeof(BYTE), height*width, F_I_Cur);

	delete Prev;
	delete Cur;
	delete Next;

	fclose(F_I_Prev);
	fclose(F_I_Cur);
	fclose(F_I_Next);

	getchar();
}
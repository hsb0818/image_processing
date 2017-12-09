#pragma once

#include <math.h>
#include <iostream>

typedef unsigned char BYTE;

class MotionVector
{
public:
	int x;
	int y;
	int sad;

public:
	MotionVector(int _x = 0, int _y = 0, int _sad = 0)
		: x(_x)
		, y(_y)
		, sad(_sad)
	{}
};

class Motion
{
private:
	MotionVector* m_vector_map;
	int m_block_size;
	int m_search_range;

public:
	Motion(const int block_size, const int search_range)
		: m_vector_map(nullptr)
		, m_block_size(block_size)
		, m_search_range(search_range)		
	{
	}

	~Motion()
	{
		if (m_vector_map != nullptr)
			delete[] m_vector_map;
	}

private:
	void Search(MotionVector& mv, const BYTE* prev_frame, const BYTE* next_frame,
		const int w, const int h, const int x, const int y)
	{
		int sxs = x - m_search_range;
		int sxe = x + m_block_size + m_search_range;
		int sys = y - m_search_range;
		int sye = y + m_block_size + m_search_range;

		sxs = (sxs >= 0) ? sxs : 0;
		sys = (sys >= 0) ? sys : 0;
		sxe = (sxe < w) ? sxe : w;
		sye = (sye < h) ? sye : h;

		int mvx = std::numeric_limits<int>::max();
		int mvy = std::numeric_limits<int>::max();
		int minsad = std::numeric_limits<int>::max();

		for (int i = sys; i < sye; i++)
		{
			for (int j = sxs; j < sxe; j++)
			{
				int sad = 0;
				for (int by = 0; by < m_block_size; by++)
				{
					for (int bx = 0; bx < m_block_size; bx++)
					{

						sad += abs(next_frame[(y + by) * w + (x + bx)] - prev_frame[(i + by) * w + (j + bx)]);
					}
				}

				if (sad < minsad)
				{
					mvx = j - x;
					mvy = i - y;
					minsad = sad;
				}
			}
		}		

		mv.x = mvx;
		mv.y = mvy;
		mv.sad = minsad;
	}

public:
	void Estimate(const BYTE* prev_frame, const BYTE* next_frame, const int w, const int h)
	{
		int bw = (w / m_block_size);
		int bh = (h / m_block_size);

		m_vector_map = new MotionVector[ bw * bh ];
		memset(m_vector_map, 0, bw * bh);

		for (int y = 0; y < bh; y++)
		{
			for (int x = 0; x < bw; x++)
			{
				Search(m_vector_map[y * bw + x], prev_frame, next_frame, w, h, x*m_block_size, y*m_block_size);
			}
		}
	}

	void Compensation(BYTE* out_frame, const BYTE* prev_frame, const int w, const int h)
	{
		int bw = (w / m_block_size);
		int bh = (h / m_block_size);

		for (int y = 0; y < bh; y++)
		{
			for (int x = 0; x < bw; x++)
			{	
				const MotionVector& mv = m_vector_map[y * bw + x];
				for (int by = 0; by < m_block_size; by++)
				{
					for (int bx = 0; bx < m_block_size; bx++)
					{
						out_frame[(y * m_block_size + by) * w + (x * m_block_size + bx)] = 
							prev_frame[((y * m_block_size + mv.y) + by) * w + ((x * m_block_size + mv.x) + bx)];
					}
				}
			}
		}
	}

	void MEMC(BYTE* out_frame, const BYTE* prev_frame, const BYTE* next_frame, const int w, const int h)
	{
		Estimate(prev_frame, next_frame, w, h);
		Compensation(out_frame, prev_frame, w, h);
	}

	static void FRUC(const Motion& m1, const Motion& m2, BYTE* out_frame, 
		const BYTE* m1_frame, const BYTE* m2_frame, const int w, const int h)
	{
		int block_size = m1.m_block_size;

		int bw = (w / block_size);
		int bh = (h / block_size);

		for (int y = 0; y < bh; y++)
		{
			for (int x = 0; x < bw; x++)
			{
				const MotionVector& mv1 = m1.m_vector_map[y * bw + x];
				const MotionVector& mv2 = m2.m_vector_map[y * bw + x];
				for (int by = 0; by < block_size; by++)
				{
					for (int bx = 0; bx < block_size; bx++)
					{
						out_frame[(y * block_size + by) * w + (x * block_size + bx)] =
							(m1_frame[((y * block_size + mv1.y) + by) * w + ((x * block_size + mv1.x) + bx)]
								+ m2_frame[((y * block_size + mv2.y) + by) * w + ((x * block_size + mv2.x) + bx)])
							/ 2;
					}
				}				
			}
		}
	}
};
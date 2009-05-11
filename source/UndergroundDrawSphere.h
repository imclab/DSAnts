#ifndef UNDERGROUNDDRAWSPHERE_H
#define UNDERGROUNDDRAWSPHERE_H

#include "Underground.h"
#include "StaticDraw.h"
#include "Patch.h"

class UndergroundDrawSphere: public Underground
{
	public:
		UndergroundDrawSphere(): Underground()
		{
			cached = false;
			computeCircles();
		}

		void draw();
		void drawSegment(int x, int y, int rightx, int upy, Patch* p);
		void computeCircles();

		void incX(){ centerX++; if (centerX > lats) centerX%=lats; }
		void decX(){ centerX--; if (centerX == -1) centerX=lats-1; }

		void incY(){ centerY++; if (centerY > longs) centerY%=longs; }
		void decY(){ centerY--; if (centerY == -1) centerY=longs-1; }

		// Returns index Num to the right of X, accounting for array wrap.
		int toTheRight(int X, int num){	int right = X + num;
																		if (right > lats) return right%lats;
																		return right; }
		// Returns index num to the left of X, accounting for array wrap.
		int toTheLeft(int X, int num){	int left = X - num;
																		if (left < 0) return (lats-1)+left;
																		return left; }
		// Shifts the center to the left and accounts for array wrap.
		int getLeftIndex(){	return toTheLeft(centerX, GRID_SIZE); }
		// Shifts the center to the right and accounts for array wrap.
		int getRightIndex(){	return toTheRight(centerX, GRID_SIZE); }

	private:
		bool cached;
		const static float innerR = 2;
		const static float outerR = 2.1;
		const static int lats = 100;
		const static int longs = 100;
		VectorF inner[lats][longs];
		VectorF outer[lats][longs];

		// need some sort of storage to know where to map the patches to the sphere.
		int centerPointX, centerPointY;
};

#endif

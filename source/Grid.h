#ifndef DIRTDISK_H
#define DIRTDISK_H

// stdlib for rand / srand
#include <stdlib.h>

//#include <nds.h>
#include <time.h>
#include <vector>

#include "global.h"
#include "Patch.h"
//#include "Dirt.h"
//#include "Empty.h"
//#include "Barrier.h"

class Grid
{
	public:
		Grid();

		// clamp the values just in case.
		Patch* getPatch(short width, short depth)
			{	if (width<0) width=0; if (depth<0) depth=0;
				return dd[width%WIDTH][depth%DEPTH]; }

		// move the x/y coord around.
		static void moveRight(short &x);
		static void moveLeft(short &x);
		static bool moveUp(short &y);
		static bool moveDown(short &y);

		// move the Patch around.
		static Patch* getRight(Patch* p);
		static Patch* getLeft(Patch* p);
		static Patch* getUp(Patch* p);
		static Patch* getDown(Patch* p);

		Patch* getRight(int x, int y);
		Patch* getLeft(int x, int y);
		Patch* getTop(int x, int y);
		Patch* getBottom(int x, int y);

		// This will generate a random nest with 'size' connected empty spaces
		// It will have one entrance which is always at (0,20,0)
		void generateNest(int size);

		// Keep track of cleared tiles in a vector.
		std::vector<Patch*> getCleared(){ return cleared; }
		void clear(Patch* p);
	private:
		Patch* dd[WIDTH][DEPTH];

		// a list of cleared tiles.
		std::vector<Patch*> cleared;
};

#endif
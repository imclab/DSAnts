#include "UndergroundDrawGrid.h"

UndergroundDrawGrid::UndergroundDrawGrid()
{
	smoothScrollX = smoothScrollY = 0;
	boxSide = 0.1;
}


// increment a scene shift some amount before scrolling the tiles.
void UndergroundDrawGrid::incX()
{
	smoothScrollX+=0.01;	

	if (smoothScrollX >= 0.2)
	{
		underground->moveRight(slice, centerX); 
		smoothScrollX = 0;
	}
}

void UndergroundDrawGrid::decX()
{ 
	smoothScrollX-=0.01;	
	if (smoothScrollX <= -0.2)
	{
		underground->moveLeft(slice, centerX);
		smoothScrollX = 0;
	}
}

void UndergroundDrawGrid::incY()
{ 
	if (centerY == 0) return;
	smoothScrollY+=0.01;
	if (smoothScrollY >= 0.2)
	{
		underground->moveUp(centerY);
		smoothScrollY = 0;
	}
}

void UndergroundDrawGrid::decY()
{ 
	if (centerY == 44) return;

	smoothScrollY-=0.01;
	if (smoothScrollY <= -0.2)
	{
		underground->moveDown(centerY);
		smoothScrollY = 0;
	}
}

void UndergroundDrawGrid::draw()
{
	int x,y, temp;

//	int left = getLeftIndex();
//	int right = getRightIndex();
//	int top = centerY + GRID_SIZE;
//	int bottom = centerY - GRID_SIZE;

	Patch* bottomLeft = '\0';
	Patch* tp = '\0';

	// Eventually this call will be made to whatever generic character the
	// map should be centered on.  i.e. the player, or an enemy ant.
	bottomLeft = underground->getPatch(slice, centerX, centerY);

	// Shift from the center to the bottomLeft.
	// shift left a bunch of times.
	for (temp=0; (bottomLeft && (temp < GRID_SIZE)) ; temp++)
		bottomLeft = bottomLeft->left;

	// shift up a bunch of times or until we hit the surface.
	for (temp=GRID_SIZE; ((bottomLeft) && (bottomLeft->bottom) && (temp < GRID_SIZE)); temp++)
		bottomLeft = bottomLeft->bottom;

	tp=bottomLeft;


	for (y=GRID_SIZE*-1; (bottomLeft && (y < ((GRID_SIZE*2)+1))); y+=1)
	{
		tp=bottomLeft;
		for (x=GRID_SIZE*-1; x < ((GRID_SIZE*2)+1); x+=1)
		{
			tp=tp->right;
			// "translate" by adding the smooth scroll factors.
			drawBox(x*0.2+smoothScrollX, y*0.2-smoothScrollY, tp);
		}
		if (bottomLeft)
			bottomLeft = bottomLeft->top;
	}
}

void UndergroundDrawGrid::drawBox(float x, float y, Patch *p)
{
// float boxSide = 0.1;
	float s = boxSide;
	glBegin(GL_QUADS);

// glColor can't work with glNormal, so use materials.
	if (p->TYPE == PATCH_DIRT)
	material(31,31,31);
	else if (p->TYPE == PATCH_EMPTY)
	material(15,15,15);
	else if (p->TYPE == PATCH_BARRIER)
	material(30,30,0);
	else if (p->TYPE == PATCH_TOP)
	material(1,1,30);

	//z face
	glNormal3f(0,0,1);
	glVertex3f(x	,y	,0);
	glVertex3f(x+s,y	,0);
	glVertex3f(x+s,y+s,0);
	glVertex3f(x	,y+s,0);

	//z+ face
	glNormal3f(0,0,-1);
	glVertex3f(x	,y	,s);
	glVertex3f(x	,y+s,s);
	glVertex3f(x+s,y+s,s);
	glVertex3f(x+s,y	,s);

	
	//x  face
	glNormal3f(1,0,0);
	glVertex3f(x,y		,0);
	glVertex3f(x,y+s	,0);
	glVertex3f(x,y+s	,s);
	glVertex3f(x,y		,s);

	//x + width face
	glNormal3f(-1,0,0);
	glVertex3f(x+s,y 	,0);
	glVertex3f(x+s,y 	,s);
	glVertex3f(x+s,y+s,s);
	glVertex3f(x+s,y+s,0);

	//y  face
	glNormal3f(0,-1,0);
	glVertex3f(x 	,y,0);
	glVertex3f(x 	,y,s);
	glVertex3f(x+s,y,s);
	glVertex3f(x+s,y,0);

	//y  + height face
	glNormal3f(0,1,0);
	glVertex3f(x 	,y+s,0);
	glVertex3f(x+s,y+s,0);
	glVertex3f(x+s,y+s,s);
	glVertex3f(x 	,y+s,s);

	glEnd();
}

void UndergroundDrawGrid::material(int r, int g, int b)
{
// This seems to work alright.
	glMaterialf(GL_DIFFUSE, RGB15(r, g, b) | BIT(15)); /// Bit 15 enables the diffuse color to act like being set with glColor(), only with lighting support. When not using lighting, this is going to be the default color, just like being set with glColor().
	glMaterialf(GL_AMBIENT, RGB15(4, 4, 5));
	glMaterialf(GL_SPECULAR, RGB15(0, 0, 0)); /// Bit 15 would have to be set here to enable a custom specularity table, instead of the default linear one.
	glMaterialf(GL_EMISSION, RGB15(0, 0, 0));
}

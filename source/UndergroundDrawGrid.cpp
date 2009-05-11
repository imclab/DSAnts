#include "UndergroundDrawGrid.h"

UndergroundDrawGrid::UndergroundDrawGrid()
{
	boxSide = 0.1;
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
			drawBox(x*0.2, y*0.2, tp);
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

	if (p->TYPE == PATCH_DIRT)
	glColor3f(1,1,1);
	else if (p->TYPE == PATCH_EMPTY)
	glColor3f(0.5,0.5,0.5);
	else if (p->TYPE == PATCH_BARRIER)
	glColor3f(1,1,0);
	else if (p->TYPE == PATCH_TOP)
	glColor3f(0,0,1);

	//z face
	glVertex3f(x	,y	,0);
	glVertex3f(x+s,y	,0);
	glVertex3f(x+s,y+s,0);
	glVertex3f(x	,y+s,0);

	//z+ face
	glVertex3f(x	,y	,s);
	glVertex3f(x	,y+s,s);
	glVertex3f(x+s,y+s,s);
	glVertex3f(x+s,y	,s);

	
	//x  face
	glVertex3f(x,y		,0);
	glVertex3f(x,y+s	,0);
	glVertex3f(x,y+s	,s);
	glVertex3f(x,y		,s);

	//x + width face
	glVertex3f(x+s,y 	,0);
	glVertex3f(x+s,y 	,s);
	glVertex3f(x+s,y+s,s);
	glVertex3f(x+s,y+s,0);

	//y  face
	glVertex3f(x 	,y,0);
	glVertex3f(x 	,y,s);
	glVertex3f(x+s,y,s);
	glVertex3f(x+s,y,0);

	//y  + height face
	glVertex3f(x 	,y+s,0);
	glVertex3f(x+s,y+s,0);
	glVertex3f(x+s,y+s,s);
	glVertex3f(x 	,y+s,s);

	glEnd();
}

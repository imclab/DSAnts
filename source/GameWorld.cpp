#include "GameWorld.h"

GameWorld::GameWorld()
{
	followingPlayer = true;

	in = new Input();

	cam = new Camera();
	//cam->translateZinc(2.2);

	ug = new Underground();
	surf = new Surface();
	surf->getGrid()->setLoopY();

	// start underground
	STATE = GAMEWORLD_STATE_UNDERGROUND;
	curMap = ug;

	// init picking variables
	picked = '\0';
	doPick = false;
	
	// Create player.
	Ant *tmp = new WorkerAnt();
	tmp->setPatch( ug->getGrid()->getPatch(0,2) );
	tmp->setLocation( STATE );
	tmp->setCarrying(PATCH_FOOD1);	
	tmp->setTakePortals( true );
	p = new Player(tmp);


	// Create queen(s) and put in a random location.
	tmp = new QueenAnt();
	tmp->setPatch( ug->getGrid()->getRandomCleared() );
	tmp->setLocation( STATE );
	tmp->setTakePortals( false );
	// Queen doesn't need an action, she just sits around making eggs
	black.push_back( tmp );

	// SETUP OBSERVERs:

	// Observing Player: GameWorld.
	p->attach(this);

	// Observing GameWorld: Player.
	attach(p);

	// Observing Input: Player, GameWorld.
	in->attach(p);
	in->attach(this);
	in->attach(cam);

	// This loops through The surface of the underground (both when I add the enemy underground)
	// and links them to the surface.
	linkSurfaceAntUnderground();
}

GameWorld::~GameWorld()
{
	delete ug;
	delete surf;
	delete p;
	delete in;
	delete cam;

	// loop through "black" and "red" and delete 'em.
	for (unsigned int i=0; i < black.size(); i++)
		delete black[i];
	for (unsigned int i=0; i < red.size(); i++)
		delete red[i];
}

int GameWorld::pickup(int loc, Patch *pat)
{
	if (OBJECT(pat))
	{
		int savet = pat->TYPE;
		getMap(loc)->getGrid()->takeObject( pat );
		// TODO: special cases: PATCH_FOOD10 -> PATCH_FOOD

		if (FOODi(savet))
		{
			switch (savet)
			{
				case PATCH_FOOD1 :
					// Ant is holding 1 food, Patch is empty.  Correct.
					break;

				case PATCH_FOOD2 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD1 );
					break;

				case PATCH_FOOD3 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD2 );
					break;
	
				case PATCH_FOOD4 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD3 );
					break;

				case PATCH_FOOD5 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD4 );
					break;

				case PATCH_FOOD6 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD5 );
					break;

				case PATCH_FOOD7 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD6 );
					break;

				case PATCH_FOOD8 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD7 );
					break;

				case PATCH_FOOD9 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD8 );
					break;

				case PATCH_FOOD10 :
					getMap(loc)->getGrid()->addObject( pat , PATCH_FOOD9 );
					break;

				default:
					// should never get here.  do nothing.
					break;
			}
			// if something picked up food, it only picks up one at a time.
			return PATCH_FOOD1;
		}

		// Object doesn't need special handling.
		else
			return savet;
	}
	// otherwise, nothing.
	return NOTHING;
}

bool GameWorld::drop(int loc, Patch* pat, int Ob)
{
	// Dropping food onto food.
	if (FOODi(Ob))
	{
		switch (pat->TYPE)
		{
			case PATCH_EMPTY :
				pat->TYPE = PATCH_FOOD1;
				break;

			case PATCH_FOOD1 :
				pat->TYPE = PATCH_FOOD2;
				break;

			case PATCH_FOOD2 :
				pat->TYPE = PATCH_FOOD3;
				break;

			case PATCH_FOOD3 :
				pat->TYPE = PATCH_FOOD4;
				break;

			case PATCH_FOOD4 :
				pat->TYPE = PATCH_FOOD5;
				break;

			case PATCH_FOOD5 :
				pat->TYPE = PATCH_FOOD6;
				break;

			case PATCH_FOOD6 :
				pat->TYPE = PATCH_FOOD7;
				break;

			case PATCH_FOOD7 :
				pat->TYPE = PATCH_FOOD8;
				break;

			case PATCH_FOOD8 :
				pat->TYPE = PATCH_FOOD9;
				break;

			case PATCH_FOOD9 :
				pat->TYPE = PATCH_FOOD10;
				break;

			// only case that should hit this is PATCH_FOOD10
			default:
				return false;
		}
		return true;
	}
	// Dropping something with no special handling..
	else if( EMPTY(pat) )
	{
		pat->TYPE = Ob;
		return true;
	}

	return false;
}


void GameWorld::linkSurfaceAntUnderground()
{
	int randX, randY;
	// loop across the top layer of the underground, link to random areas of the surface.
	// ug is the black ants, in the left side of the map.
	Patch* topleft = ug->getGrid()->getPatch(0,1);
	do
	{
		// find random locations until we get one that hasn't already been picked.
		do
		{
			randX = rand() % (WIDTH / 2);
			randY = rand() % DEPTH;
			topleft->portal = surf->getGrid()->getPatch(randX, randY);
		}while (!EMPTY(topleft->portal));

		topleft->portal->TYPE = PATCH_ENTRANCE;

		// make it two-way.
		// NOTE: the surface points to just below the top level of the underground
		topleft->portal->portal = topleft;
		// move right
		topleft = Grid::getRight( topleft );
		// until we loop all the way around.
	} while(topleft->x != 0);
	
}

void GameWorld::draw()
{
	numAnts = 0;
	// if player is offscreen, center screen.
	if (! curMap->isVisible( p->getPlayerAnt()->getX(), p->getPlayerAnt()->getY() ) )
		curMap->setCenter( p->getPlayerAnt()->getX(), p->getPlayerAnt()->getY() );

	curMap->doMapShift();
	curMap->beginQuads();
	// Draw game field.
	curMap->draw();

	// draw the ants			
	for (unsigned int i=0; (numAnts < 80) && (i < black.size()); i++)
		if ( black[i]->getLocation() == STATE )
			if (curMap->drawAnt(black[i], true))
				numAnts++;

//		for (unsigned int i=0; i < red.size(); i++)
//			if ( red[i]->getLocation() == GAMEWORLD_STATE_UNDERGROUND )
//				ug->drawAnt(red[i]);

	curMap->drawAnt(p->getPlayerAnt(), true);

	// DO THE PICKING
	// If the touch pad is being touched... see what its touching.
	if (doPick)
	{
		pickPoint(in->getTouchX(), in->getTouchY());
		doPick = false;
	}

	curMap->end();
}

void GameWorld::pickPoint(short x, short y)
{
	if (curMap->pickPoint(x, y, *cam))
	{
		picked = curMap->getPicked();
		// Disabling automove for now.
		//p->setDestination(picked->x, picked->y);
		//if (WALKABLE(picked))
		//	automove = true;
		//else
		//	automove = false;

		// update intself
		update(PLAYER_PICKED_SOMETHING);
	}
}

void GameWorld::stepAntsForward(int num)
{
	// test, move each ant around randomly.
	for (unsigned int i=0; i < black.size(); i++)
	{
		// AI for ant "black[i]->AImove()"
		black[i]->stateStep(num);
		black[i]->move(num);
	}
}

void GameWorld::stepForward(int num)
{
	// The "Player" doesn't keep track of its location, so I can't do this through observer.
	if (STATE != p->getPlayerAnt()->getLocation())
	{
		STATE = p->getPlayerAnt()->getLocation();
		curMap = getMap(STATE);
	}

	// The map needs to follow the player.
	// This needs to be done every frame rather than on player move event,
	// otherwise panning is jerky.
	if (followingPlayer)
		curMap->shiftCenter(p->getPlayerAnt(), num);


	// no draw here, it is handled elsewhere so that things will be able
	// to move forward if things start to lag.

	// process user input.
	// TODO: if this is moved into the VBlank, will the input registers be valid?
	// note: vBlank didn't work.
	in->process();

	// send everyone on their way.
	stepAntsForward(num);
	p->stepForward(num);

/*
	if( held & KEY_LEFT)
	{
		//p->moveLeft();
		set_val(PLAYER_HELD_LEFT);
		automove = false;
	}
	if( held & KEY_RIGHT)
	{
		//p->moveRight();
		set_val(PLAYER_HELD_RIGHT);
		automove = false;
	}
	if( held & KEY_UP)
	{
		//p->moveUp();
		set_val(PLAYER_HELD_UP);
		automove = false;
	}
	if( held & KEY_DOWN)
	{
		//p->moveDown();
		set_val(PLAYER_HELD_DOWN);
		automove = false;
	}

	// Pressing the D-Pad cancels automove.
	// automove is set by touching an empty spot.
	if (automove)
	{
		p->move();
	}
*/

}


void GameWorld::setProjection()
{
	//change ortho vs perspective
//	if(held & KEY_B)
//		cam->Ortho();
//	else 
		cam->Perspective();
}

void GameWorld::update(int value)
{
	// TODO: do follow cursor on hold?

	// do picking on PRESS
	if (value == PLAYER_PRESSED_TOUCHPAD)
		doPick = true;
	else if (value == PLAYER_PICKED_SOMETHING)
	{
		// If it is dirt, see if we can DIG IT.
		if (picked->TYPE == PATCH_DIRT)
			curMap->getGrid()->clear(p->dig());
		// If your carrying something, and the spot is empty, drop.
		else if (EMPTY(picked) && p->getPlayerAnt()->getCarrying())
		{
			p->drop();
			// "drop" clears out what its carring, so need to store it for a moment.
			//int saveO = p->getPlayerAnt()->getCarrying();
			//curMap->getGrid()->addObject( p->drop(), saveO );
		}
		// if it is an object, see if we can PICK IT.
		else if (OBJECT(picked))
		{
			p->pickUp();
			//curMap->getGrid()->takeObject( p->pickUp() );
		}
	}
	else if (value == PLAYER_RELEASED_TOUCHPAD)
	{
		if (picked)
			picked->picked = false;
	}


	// Hold A to spawn ants,
	else if(value == PLAYER_HELD_B)
	{
		// add a new ant on press.
		Ant *t = new WorkerAnt(ug->getGrid()->getPatch(0,2), GAMEWORLD_STATE_UNDERGROUND);
		t->setAction( ANT_ACTION_WANDER );
		black.push_back(t);
	}

	else if (value == PLAYER_HELD_A)
	{
		Ant *t = new QueenAnt(ug->getGrid()->getPatch(0,2), GAMEWORLD_STATE_UNDERGROUND);
		t->setAction( ANT_ACTION_WANDER );
		black.push_back(t);
	}
	else if (value == PLAYER_PRESSED_X)
	{
		#ifdef __PROFILING
			cygprofile_end();
		#endif
	}
}

//#ifdef __DEBUG
void GameWorld::printDebugFiveLines()
{
	// Interesting stats:
	//    Number of ants
	//    Map / Map coordinate
	//		Spot touched
	//		Player info
	//		player automove?
//	printf("\nWORLD, Ants: black(%i), red(%i)", black.size(), red.size());
	if (STATE == GAMEWORLD_STATE_UNDERGROUND)
		printf("\nCurrent map: underground");
	else if (STATE == GAMEWORLD_STATE_SURFACE)
		printf("\nCurrent map: surface");
	else
		printf("\nCurrent map: unknown");
//	printf("\nMap Stats: <UG> <Surf>");
//	printf("\n  Cleared: %5d %5d", ug->getGrid()->numCleared(), surf->getGrid()->numCleared());
//	printf("\n  Objects: %5d %5d", ug->getGrid()->numObjects(), surf->getGrid()->numObjects());
//	printf("\nMap Center: (%i, %i)", curMap->getCenterX(), curMap->getCenterY());
	printf("\nAnts: drawn(%i)\n black(%i), red(%i)", numAnts, black.size(), red.size());
//	printf("\nCamera distance: %f", cam->getCamLocation().z);
//	printf("\nTouch coord: (%i, %i)", curX, curY);
	p->printDebug();


}
//#endif

#ifndef CREATURE_H
#define CREATURE_H

#include "Patch.h"
#include "Grid.h"
//#ifdef __DEBUG
#include <stdio.h>
//#endif

class Creature
{
  public:
    Creature();
    Creature(Patch* pat, int location);

    // TODO: this needs to clear out the ant-pointer in the patch upon deletion.
    ~Creature();
    // template class.
    virtual int getType() = 0;

    // move the ant.
    // return value: true if tile changed.
    bool moveRight();
    bool moveLeft();
    bool moveUp();
    bool moveDown();
    // If moving into a portal from any direction, need to handle it the same way.
    // returns true if it needed to be handled and the move needed to be stopped.
    virtual bool handlePortal();
    virtual bool handleCollision();
    virtual bool checkCollision(Patch* pat);
    virtual bool moveTo(Patch* pat, bool force = false);

    virtual inline bool directionIsOk(int n, int o, Patch* p)
      {
        return ((n != o) && patchIsMovable(p) && !checkCollision(p));
      }

    virtual inline bool patchIsMovable(Patch* pat)
      {
        return ((pat != '\0') && WALKABLE(pat) && !checkVisited(pat));
      }

    // checks whether the patch is in the recent visited memory
    virtual bool checkVisited(Patch* newPatch)
      {
        if (!use_visit_memory)
          return true;

        for (int i = 0; i < MAX_VISIT_MEMORY; i++)
          if ( newPatch == visited[i] ) return true;
        return false;
      }

    virtual void clearVisited()
      {
        for (int i = 0; i < MAX_VISIT_MEMORY; i++)
          visited[i] = '\0';
      }

    // order is not important, so just loop through to the beginning
    // of the array overwriting the first one.
    virtual void addVisited(Patch* newPatch)
      {
        if (!checkVisited(newPatch))
          visited[visited_index++] = newPatch;
        if (visited_index == MAX_VISIT_MEMORY)
          visited_index = 0;
      }

    virtual void handleFeramone();
    // This lets the ant move on its own accord, influenced by:
    //    -feramone level of adjacent tiles
    //    -current action
    virtual void stateStep(int num);

    // move is broken into several parts.
    virtual void move(int num);
    virtual void move();
    virtual void moveAI();
    virtual void wander();
    virtual void attack();
    virtual void forage() = 0;
    virtual void goHome();

    // use the carrying object (i.e. eat food / egg)
    virtual int use();

    // True if will pickup.  False otherwise.
    void pickup(Patch *p);

    // True if it can drop, false otherwise.
    bool drop(Patch *p);

    //-----------------------//
    // Accessors and Getters //
    //-----------------------//
    void setAction(int a){ ACTION = a; }
    int getAction(){ return ACTION; }

    inline int getOffsetX(){ return offsetX; }
    inline int getOffsetY(){ return offsetY; }

    inline void setAI(bool inai){ ai = inai; }
    inline bool getAI(){ return ai; }

    inline short getFacingX(){ return directionX; }
    inline short getFacingY(){ return directionY; }

    inline int getCarrying(){ return carrying; }
    inline void setCarrying(int x){ carrying = x; }

    inline void setHP(short h){ hp=h; }
    inline short getHP(){ return hp; }

    inline short getX(){ return p->x; }
    inline short getY(){ return p->y; }

    inline void setPatch(Patch* inp){ p = inp; }
    inline Patch* getPatch(){ return p; }

    inline int getLocation(){ return location; }
    inline void setLocation(int l){ location = l; }

    inline bool getTakePortals(){ return takePortals; }
    inline void setTakePortals(bool tp){ takePortals = tp; }

    inline bool getUseVisitMemory(){ return use_visit_memory; }
    inline void setUseVisitMemory(bool uvm){ use_visit_memory = uvm; }

    //#ifdef __DEBUG
    void printDebug();
    //#endif

    // save the direction between steps to prevent re-calculation.
    int direction;
    int directionOld; // ant wants to avoid turning around.
    // if false, will not go through portals.
    bool takePortals;
  private:
    // these are used to change offsetX / offsetY and keep the direction correct.
    void incrementOffsetX();
    void decrementOffsetX();
    void incrementOffsetY();
    void decrementOffsetY();
    void clampDirections();

    //-----------------//
    // AI INFORMATION: //
    //-----------------//
    // True or false if the AI needs to intervene.
    bool ai;
    // ai will set these after deciding where the ant is heading.
    // TODO: Dijkstra?
    short dest_x, dest_y;
    // Action the ai has decided the ant will perform, ie:
    //  FORAGE, FIGHT, DEFEND, FOLLOW, ...?
    int ACTION;

       //-----------------------//
      // LOCATION INFORMATION: //
     //-----------------------//
    // The x/y coordinate the ant is in, for convenient draw checks.
    // UNDERGROUND, SURFACE, ENEMY_UNDERGROUND.
    int location;

    // The patch the ant is in, for convenient adjacentcy checks.
    Patch *p;
    // flag so that we don't go back and forth through a portal infinitly.
    bool portaled;
    // collision counter.  If collide too many times look for another direction.
    short failCount;

    // The offset within the patch
    short offsetX;
    short offsetY;

    // These are needed so that the ant is rotated in the right direction.
    short directionX;
    short directionY;
    
    // Status, health information.
    short hp;
    // what type of creature is it (ANT_WORKER, ANT_QUEEN, SPIDER, etc)
    short TYPE;

    // is the ant carrying something?
    int carrying;

    bool use_visit_memory;
    short visited_index;
    Patch* visited[MAX_VISIT_MEMORY];
};

#endif

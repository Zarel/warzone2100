/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2010  Warzone 2100 Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
/** @file
 *  Definitions for the Power Functionality.
 */

#ifndef __INCLUDED_SRC_POWER_H__
#define __INCLUDED_SRC_POWER_H__

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/** Free power on collection of oildrum. */
#define OILDRUM_POWER		100

#define	POWER_PER_CYCLE		5

/** Used to determine the power cost of repairing a droid.
    Definately DON'T WANT the brackets round 1/2 - it will equate to zero! */
#define REPAIR_POWER_FACTOR 1/5

/** Used to multiply all repair calculations by to avaoid rounding errors. */
#define POWER_FACTOR        100

// The droid orders
typedef enum _power_queue_type
{
	PQ_NONE,
	PQ_MANUFACTURE,
	PQ_RESEARCH,
	PQ_BUILD,
	PQ_NUM_TYPES
} POWER_QUEUE_TYPE;

typedef struct _power_queue
{
	POWER_QUEUE_TYPE     type;

	int                  player;
	BASE_OBJECT*         worker;
	void*                workSubject;
	/*
	 * type == PQ_MANUFACTURE
	 *   worker is STRUCTURE*
	 *   workSubject is BASE_STATS*
	 * type == PQ_RESEARCH
	 *   worker is STRUCTURE*
	 *   workSubject is BASE_STATS*
	 * type == PQ_BUILD
	 *   worker is DROID*
	 *   workSubject is []
	 */

	int                  price;       ///< How much money we need to dequeue the thing
	bool                 ready;       ///< We have enough power to dequeue this node
	struct _power_queue* next;        ///< Next in the power queue linked list
} POWER_QUEUE;

#define POWER_QUEUE_MAX 100

typedef struct _player_power
{
	int32_t          currentPower;          // 24.8 fixed point; 256 = 1 power point
	int32_t          queuedPower;    // 24.8 fixed point
	POWER_QUEUE*     powerQueue;
	POWER_QUEUE*     powerQueueBack;
	int              powerQueueSize;
} PLAYER_POWER;

extern PLAYER_POWER	asPower[MAX_PLAYERS];

/** Allocate the space for the playerPower. */
extern BOOL allocPlayerPower(void);

/** Clear the playerPower. */
extern void clearPlayerPower(void);

/** Reset the power levels when a power_gen or resource_extractor is destroyed. */
extern BOOL resetPlayerPower(UDWORD player, STRUCTURE *psStruct);

/** Free the space used for playerPower. */
extern void releasePlayerPower(void);

/** Check the available power. */
extern BOOL checkPower(int player, int quantity);

extern void addPower(int player, int quantity);

BOOL checkPower(int player, int quantity);
void usePower(int player, int quantity);

/** Update current power based on what was extracted during the last cycle and what Power Generators exist. */
extern void updatePlayerPower(UDWORD player);

/** Used in multiplayer to force power levels. */
extern void setPower(int player, int power);

/** Get the amount of power current held by the given player. */
extern int getPower(int player);
extern int getQueuedPower(int player);
extern int getExtractedPower(int player);

/** Resets the power levels for all players when power is turned back on. */
void powerCalc(BOOL on);

/** Temp function to give all players some power when a new game has been loaded. */
void newGameInitPower(void);

/**	Returns the next res. Ext. in the list from the one passed in. returns 1st one
	in list if passed in is NULL and NULL if there's none?
*/
extern STRUCTURE *getRExtractor(STRUCTURE *psStruct);

/** Defines which structure types draw power - returns true if use power. */
extern BOOL structUsesPower(STRUCTURE *psStruct);

/** Defines which droid types draw power - returns true if use power. */
extern BOOL droidUsesPower(DROID *psDroid);

/** Flag used to check for power calculations to be done or not. */
extern	BOOL			powerCalculated;

extern int powerQueueProgress(BASE_OBJECT* worker);
extern POWER_QUEUE* powerQueueAdd(POWER_QUEUE_TYPE type, BASE_OBJECT* worker, void* workSubject, int price);
extern bool powerQueueMoveToFront(POWER_QUEUE* node);
extern bool powerQueueMoveToBack(POWER_QUEUE* node);
extern bool powerQueueCancel(POWER_QUEUE* node);
extern void powerQueueCancelWorker(BASE_OBJECT* worker);
extern bool powerQueueCancelSubject(BASE_OBJECT* worker, void* workSubject);
extern bool powerQueueAccept(POWER_QUEUE* node);
extern POWER_QUEUE* powerQueueNext(POWER_QUEUE_TYPE type, BASE_OBJECT* worker);
extern bool powerQueueForce(POWER_QUEUE_TYPE type, BASE_OBJECT* worker, void* workSubject, int price);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __INCLUDED_SRC_POWER_H__

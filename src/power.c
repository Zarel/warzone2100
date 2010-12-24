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
/**
 * @file power.c
 *
 * Store PlayerPower and other power related stuff!
 *
 */
#include <string.h>
#include "objectdef.h"
#include "power.h"
#include "hci.h"
#include "lib/gamelib/gtime.h"
#include "lib/sound/audio.h"
#include "objmem.h"
#include "frontend.h"
#include "lib/netplay/netplay.h"
#include "multiplay.h"
#include "multiint.h"

#include "feature.h"
#include "structure.h"
#include "mission.h"
#include "research.h"
#include "intdisplay.h"
#include "action.h"
#include "difficulty.h"
#include "random.h"


#define EXTRACT_POINTS	    1
#define EASY_POWER_MOD      110
#define NORMAL_POWER_MOD    100
#define HARD_POWER_MOD      90
#define MAX_POWER           (100000<<8)

//flag used to check for power calculations to be done or not
BOOL	powerCalculated;
UDWORD nextPowerSystemUpdate;

/* Updates the current power based on the extracted power and a Power Generator*/
static void updateCurrentPower(POWER_GEN *psPowerGen, UDWORD player);
/** Each Resource Extractor yields EXTRACT_POINTS per second until there are none left in the resource. */
static float updateExtractedPower(STRUCTURE *psBuilding);

//returns the relevant list based on OffWorld or OnWorld
static STRUCTURE* powerStructList(UBYTE player);

// O(1) method to remove a node, when given the previous node
static inline void powerQueueCancelNext(int player, POWER_QUEUE* prevNode, bool clearCache);
// find the next node the hard way - for queue types that don't support caching, or to update the cache
static POWER_QUEUE* powerQueueFindNext(int player, POWER_QUEUE_TYPE type, BASE_OBJECT* worker);

PLAYER_POWER		asPower[MAX_PLAYERS];

/*allocate the space for the playerPower*/
BOOL allocPlayerPower(void)
{
	clearPlayerPower();
	powerCalculated = true;
	return true;
}

/*clear the playerPower */
void clearPlayerPower(void)
{
	UDWORD player;

	for (player = 0; player < MAX_PLAYERS; player++)
	{
		asPower[player].currentPower = 0;
		asPower[player].queuedPower = 0;
		while (asPower[player].powerQueue)
		{
			POWER_QUEUE* nextPowerQueue;
			nextPowerQueue = asPower[player].powerQueue->next;
			free(asPower[player].powerQueue);
			asPower[player].powerQueue = nextPowerQueue;
		}
		asPower[player].powerQueueBack = NULL;
		asPower[player].powerQueueSize = 0;
	}
}

/*Free the space used for playerPower */
void releasePlayerPower(void)
{
	// nothing now
}

/*check the current power - if enough return true, else return false */
BOOL checkPower(int player, int quantity)
{
	ASSERT_OR_RETURN(false, player < MAX_PLAYERS, "Bad player (%d)", player);

	//if not doing a check on the power - just return true
	if (!powerCalculated)
	{
		return true;
	}

	if (asPower[player].currentPower >= (quantity<<8))
	{
		return true;
	}
	return false;
}

void usePower(int player, int quantity)
{
	ASSERT_OR_RETURN(, player < MAX_PLAYERS, "Bad player (%d)", player);
	asPower[player].currentPower = MAX(0, asPower[player].currentPower - (quantity<<8));
}

void addPower(int player, int quantity)
{
	ASSERT_OR_RETURN(, player < MAX_PLAYERS, "Bad player (%d)", player);
	asPower[player].currentPower = MAX(0, asPower[player].currentPower + (quantity<<8));
	if (asPower[player].currentPower > MAX_POWER)
	{
		asPower[player].currentPower = MAX_POWER;
	}
}

/*resets the power calc flag for all players*/
void powerCalc(BOOL on)
{
	powerCalculated = on;
}

/** Each Resource Extractor yields EXTRACT_POINTS per second FOREVER */
float updateExtractedPower(STRUCTURE	*psBuilding)
{
	RES_EXTRACTOR		*pResExtractor;
	SDWORD				timeDiff;
	UBYTE			modifier;
	float pointsToAdd,extractedPoints;

	pResExtractor = (RES_EXTRACTOR *) psBuilding->pFunctionality;
	extractedPoints = 0;

	//only extracts points whilst its active ie associated with a power gen
	//and has got some power to extract
	if (pResExtractor->active)
	{
		// if the extractor hasn't been updated recently, now would be a good time.
		if (pResExtractor->timeLastUpdated < 20 && gameTime >= 20)
		{
			pResExtractor->timeLastUpdated = gameTime;
		}
		timeDiff = (int)gameTime - (int)pResExtractor->timeLastUpdated;
		// Add modifier according to difficulty level
		if (getDifficultyLevel() == DL_EASY)
		{
			modifier = EASY_POWER_MOD;
		}
		else if (getDifficultyLevel() == DL_HARD)
		{
			modifier = HARD_POWER_MOD;
		}
		else
		{
			modifier = NORMAL_POWER_MOD;
		}
		// include modifier as a %
		pointsToAdd = ((float)modifier * EXTRACT_POINTS * timeDiff) / (GAME_TICKS_PER_SEC * 100);

		if (timeDiff > GAME_TICKS_PER_SEC || -timeDiff > GAME_TICKS_PER_SEC)
		{
			// extractor is not in the right time zone
			// we have a maximum time skip of less than a second, so this can't be caused by lag
			ASSERT(false, "Oil derrick out of sync.");
			pointsToAdd = 0;
		}

		pResExtractor->timeLastUpdated = gameTime;
		extractedPoints += pointsToAdd;
	}
	ASSERT(extractedPoints >= 0, "extracted negative amount of power");
	return extractedPoints;
}

//returns the relevant list based on OffWorld or OnWorld
STRUCTURE* powerStructList(UBYTE player)
{
	ASSERT(player < MAX_PLAYERS, "powerStructList: Bad player");
	if (offWorldKeepLists)
	{
		return (mission.apsStructLists[player]);
	}
	else
	{
		return (apsStructLists[player]);
	}
}

/* Update current power based on what Power Generators exist */
void updatePlayerPower(UDWORD player)
{
	STRUCTURE		*psStruct;

	ASSERT(player < MAX_PLAYERS, "updatePlayerPower: Bad player");

	for (psStruct = powerStructList((UBYTE)player); psStruct != NULL; psStruct =
		psStruct->psNext)
	{
		if (psStruct->pStructureType->type == REF_POWER_GEN && psStruct->
			status == SS_BUILT)
		{
			updateCurrentPower((POWER_GEN *)psStruct->pFunctionality, player);
		}
	}

	// update power queue
	{
		POWER_QUEUE*	currentNode = asPower[player].powerQueue;
		POWER_QUEUE*	prevNode = NULL;
		int				currentPowerTotal = 0;
		while (currentNode)
		{
			if (currentNode->worker->died)
			{
				currentNode = currentNode->next;
				powerQueueCancelNext(player, prevNode, false);
				if (currentNode->worker->type == OBJ_STRUCTURE)
				{
					((STRUCTURE*)currentNode->worker)->cachedPowerQueue = NULL; // shouldn't be an issue, but let's play it safe
				}
			}
			else
			{
				currentPowerTotal += (currentNode->price<<8);
				currentNode->ready = (currentPowerTotal <= asPower[player].currentPower);

				prevNode = currentNode;
				currentNode = currentNode->next;
			}
		}
	}
}

/* Updates the current power based on the extracted power and a Power Generator*/
void updateCurrentPower(POWER_GEN *psPowerGen, UDWORD player)
{
	int i;
	float extractedPower;

	ASSERT(player < MAX_PLAYERS, "updateCurrentPower: Bad player");

	//each power gen can cope with its associated resource extractors
	extractedPower = 0;
	for (i=0; i < NUM_POWER_MODULES; i++)
	{
		if (psPowerGen->apResExtractors[i])
		{
			//check not died
			if (psPowerGen->apResExtractors[i]->died)
			{
				psPowerGen->apResExtractors[i] = NULL;
			}
			else
			{
				extractedPower += updateExtractedPower(psPowerGen->apResExtractors[i]);
			}
		}
	}

	asPower[player].currentPower += (extractedPower * psPowerGen->multiplier * (1<<8)) / 100;
	ASSERT(asPower[player].currentPower >= 0, "negative power");
	if (asPower[player].currentPower > MAX_POWER)
	{
		asPower[player].currentPower = MAX_POWER;
	}
}

// only used in multiplayer games.
void setPower(int player, int power)
{
	ASSERT(player < MAX_PLAYERS, "setPower: Bad player (%u)", player);

	asPower[player].currentPower = (power << 8);
	ASSERT(asPower[player].currentPower >= 0, "negative power");
}

int getPower(int player)
{
	ASSERT(player < MAX_PLAYERS, "setPower: Bad player (%u)", player);

	return (asPower[player].currentPower >> 8);
}

int getQueuedPower(int player)
{
	ASSERT(player < MAX_PLAYERS, "setPower: Bad player (%u)", player);

	return (asPower[player].queuedPower >> 8);
}

/*Temp function to give all players some power when a new game has been loaded*/
void newGameInitPower(void)
{
	UDWORD		inc;

	for (inc=0; inc < MAX_PLAYERS; inc++)
	{
		addPower(inc, 400);
	}
}

STRUCTURE *getRExtractor(STRUCTURE *psStruct)
{
STRUCTURE	*psCurr;
STRUCTURE	*psFirst;
BOOL		bGonePastIt;

	for(psCurr = apsStructLists[selectedPlayer],psFirst = NULL,bGonePastIt = false;
		psCurr; psCurr = psCurr->psNext)
	{
		if( psCurr->pStructureType->type == REF_RESOURCE_EXTRACTOR )
		{

			if(!psFirst)
			{
				psFirst = psCurr;
			}

			if(!psStruct)
			{
				return(psCurr);
			}
			else if(psCurr!=psStruct && bGonePastIt)
			{
				return(psCurr);
			}

			if(psCurr==psStruct)
			{
				bGonePastIt = true;
			}


		}
	}
	return(psFirst);
}

/*defines which structure types draw power - returns true if use power*/
BOOL structUsesPower(STRUCTURE *psStruct)
{
    BOOL    bUsesPower = false;

	ASSERT( psStruct != NULL,
		"structUsesPower: Invalid Structure pointer" );

    switch(psStruct->pStructureType->type)
    {
        case REF_FACTORY:
	    case REF_CYBORG_FACTORY:
    	case REF_VTOL_FACTORY:
	    case REF_RESEARCH:
            bUsesPower = true;
            break;
        default:
            bUsesPower = false;
            break;
    }

    return bUsesPower;
}

/*defines which droid types draw power - returns true if use power*/
BOOL droidUsesPower(DROID *psDroid)
{
    BOOL    bUsesPower = false;

	ASSERT(psDroid != NULL,	"droidUsesPower: Invalid unit pointer" );

    switch(psDroid->droidType)
    {
        case DROID_CONSTRUCT:
        case DROID_CYBORG_CONSTRUCT:
            bUsesPower = true;
            break;
        default:
            bUsesPower = false;
            break;
    }

    return bUsesPower;
}

/*
 * Power queue subroutines
 */

// does this power queue type support caching?
static inline bool powerQueueHasCache(POWER_QUEUE_TYPE type)
{
	return (type == PQ_RESEARCH || type == PQ_MANUFACTURE || type == PQ_BUILD);
}
// get the cached power queue node for this worker, if it exists
static inline POWER_QUEUE* powerQueueCachedNext(int player, POWER_QUEUE_TYPE type, BASE_OBJECT* worker)
{
	if (type == PQ_RESEARCH || type == PQ_MANUFACTURE || type == PQ_BUILD)
	{
		return ((STRUCTURE*)worker)->cachedPowerQueue;
	}
	return NULL;
}
// set the power queue cache, if it's a queue type that supports caching
static inline void powerQueueSetCache(POWER_QUEUE_TYPE type, BASE_OBJECT* worker, POWER_QUEUE* pq)
{
	if (powerQueueHasCache(type))
	{
		((STRUCTURE*)worker)->cachedPowerQueue = pq;
	}
}
// update the power queue cache; it's outdated
static inline void powerQueueClearCache(int player, POWER_QUEUE_TYPE type, BASE_OBJECT* worker)
{
	powerQueueSetCache(type, worker, powerQueueFindNext(player, type, worker));
}

/*
 * Power queue
 */

// Returns percentage of price of current object we currently have
int powerQueueProgress(BASE_OBJECT* worker)
{
	if (worker && asPower[worker->player].powerQueue && asPower[worker->player].powerQueue->worker == worker)
	{
		return MIN(100 * getPower(worker->player) / asPower[worker->player].powerQueue->price, 100);
	}
	return 0;
}

// Returns node if the add was successful
POWER_QUEUE* powerQueueAdd(POWER_QUEUE_TYPE type, BASE_OBJECT* worker, void* workSubject, int price)
{
	const int player = worker->player;
	POWER_QUEUE* pqnode;
	if (asPower[player].powerQueueSize > POWER_QUEUE_MAX)
	{
		return NULL;
	}

	pqnode = malloc(sizeof(POWER_QUEUE));
	if (!pqnode) // we're kind of screwed if this happens
	{
		return NULL;
	}
	pqnode->type = type;
	pqnode->player = player;
	pqnode->worker = worker;
	pqnode->workSubject = workSubject;
	pqnode->price = price;
	pqnode->next = NULL;
	asPower[player].queuedPower += (price<<8);
	pqnode->ready = (asPower[player].queuedPower <= asPower[player].currentPower);

	if (asPower[player].powerQueueBack)
	{
		asPower[player].powerQueueBack->next = pqnode;
	}
	else
	{
		asPower[player].powerQueue = pqnode;
	}
	asPower[player].powerQueueBack = pqnode;
	if (!powerQueueCachedNext(player, type, worker))
	{
		powerQueueSetCache(type, worker, pqnode);
	}
	return pqnode;
}

// Move this node to the front of its queue
bool powerQueueMoveToFront(POWER_QUEUE* node)
{
	int player;
	POWER_QUEUE* currentNode;
	if (!node)
	{
		return false;
	}
	player = node->player;
	currentNode = asPower[player].powerQueue;
	if (currentNode == node)
	{
		return true;
	}
	while (currentNode)
	{
		if (currentNode->next == node)
		{
			currentNode->next = node->next;
			node->next = asPower[player].powerQueue;
			asPower[player].powerQueue = node;
			return true;
		}
		currentNode = currentNode->next;
	}
	return false;
}

// Move this node to the back of its queue
bool powerQueueMoveToBack(POWER_QUEUE* node)
{
	int player;
	POWER_QUEUE* currentNode;
	if (!node)
	{
		return false;
	}
	player = node->player;
	currentNode = asPower[player].powerQueue;
	if (node->next == NULL)
	{
		return true;
	}
	if (currentNode == node)
	{
		asPower[player].powerQueue->next = node->next;
		node->next = NULL;
	}
	else while (currentNode->next)
	{
		if (currentNode->next == node)
		{
			currentNode->next = node->next;
			node->next = NULL;
		}
		currentNode = currentNode->next;
	}
	if (node->next == NULL && currentNode->next == NULL && node != currentNode)
	{
		currentNode->next = node;
		return true;
	}
	return false;
}

// Returns true if the node was successfully removed
bool powerQueueCancel(POWER_QUEUE* node)
{
	const int player = node->player;
	POWER_QUEUE* currentNode = asPower[player].powerQueue;
	if (currentNode == node)
	{
		powerQueueCancelNext(player,NULL,true);
		return true;
	}
	while (currentNode)
	{
		if (currentNode->next == node)
		{
			powerQueueCancelNext(player,currentNode,true);
			return true;
		}
		currentNode = currentNode->next;
	}
	return false;
}

// O(1) method to remove a node, when given the previous node
static inline void powerQueueCancelNext(int player, POWER_QUEUE* prevNode, bool clearCache)
{
	POWER_QUEUE* node;
	POWER_QUEUE_TYPE type;
	BASE_OBJECT* worker;
	if (!prevNode)
	{
		node = asPower[player].powerQueue;
		type = node->type; worker = node->worker;

		asPower[player].powerQueue = node->next;
		if (node->next == NULL)
		{
			asPower[player].powerQueueBack = NULL;
		}
		asPower[player].queuedPower -= (node->price<<8);
		free(node);
	}
	else
	{
		node = prevNode->next;
		type = node->type; worker = node->worker;

		prevNode->next = node->next;
		asPower[player].queuedPower -= (node->price<<8);
		free(node);
	}
	if (clearCache)
	{
		powerQueueClearCache(player, type, worker);
	}
}

// cancel everything a worker has done
void powerQueueCancelWorker(BASE_OBJECT* worker)
{
	const int player = worker->player;
	POWER_QUEUE* currentNode = asPower[player].powerQueue;
	if (!currentNode)
	{
		return;
	}
	if (currentNode->worker == worker)
	{
		powerQueueCancelNext(player,NULL,false);
	}
	while (currentNode && currentNode->next)
	{
		if (currentNode->next->worker == worker)
		{
			powerQueueCancelNext(player, currentNode, false);
		}
		currentNode = currentNode->next;
	}
	powerQueueSetCache(PQ_NONE, worker, NULL);
}

// cancel a single subject from a worker
bool powerQueueCancelSubject(BASE_OBJECT* worker, void* workSubject)
{
	const int player = worker->player;
	POWER_QUEUE* currentNode = asPower[player].powerQueue;
	if (!currentNode)
	{
		return false;
	}
	if (currentNode->worker == worker && currentNode->workSubject == workSubject)
	{
		powerQueueCancelNext(player,NULL,true);
		return true;
	}
	while (currentNode->next)
	{
		if (currentNode->next->worker == worker && currentNode->next->workSubject == workSubject)
		{
			powerQueueCancelNext(player,currentNode,true);
			return true;
		}
		currentNode = currentNode->next;
	}
	return false;
}

// Returns true if the node was successfully accepted
bool powerQueueAccept(POWER_QUEUE* node)
{
	const int player = node->player;
	int price = node->price;
	if (!checkPower(player,price))
	{
		return false;
	}
	if (powerQueueCancel(node))
	{
		usePower(player, node->price);
		return true;
	}
	return false;
}

// Returns the next power queue node with the passed type and subject.
static POWER_QUEUE* powerQueueFindNext(int player, POWER_QUEUE_TYPE type, BASE_OBJECT* worker)
{
	POWER_QUEUE* currentNode = asPower[player].powerQueue;
	while (currentNode)
	{
		if (currentNode->type == type && currentNode->worker == worker)
		{
			return currentNode;
		}
		currentNode = currentNode->next;
	}
	return NULL;
}

POWER_QUEUE* powerQueueNext(POWER_QUEUE_TYPE type, BASE_OBJECT* worker)
{
	const int player = worker->player;
	if (powerQueueHasCache(type))
	{
		return powerQueueCachedNext(player, type, worker);
	}
	return powerQueueFindNext(player, type, worker);
}

// Use this function if you want to use the power queue as a black box that you eventually get power out of.
// Instructions: Repeatedly call it until it returns true. I call this "forcing" something through the power queue,
//   hence the function name.
// "Forcing" something through a power queue can only be done if the worker is not using the power queue in any
//   other way, and the worker is only forcing one thing through the power queue at a time.
bool powerQueueForce(POWER_QUEUE_TYPE type, BASE_OBJECT* worker, void* workSubject, int price)
{
	// can we start now?
	POWER_QUEUE* queue = powerQueueNext(type, worker);
	if (!queue || (BASE_STATS*)queue->workSubject != workSubject)
	{
		// What we're currently building isn't in the power queue yet. Put it in, and start
		// next update.
		if (queue)
		{
			powerQueueCancelWorker(worker);
		}
		if (!workSubject)
		{
			// we're just clearing the power queue.
			return false;
		}
		queue = powerQueueAdd(type, worker, workSubject, price);
	}
	if (queue->ready && powerQueueAccept(queue))
	{
		// okay, we're good
		return true;
	}
	else
	{
		// not enough power to start; workers still on strike.
		return false;
	}
}


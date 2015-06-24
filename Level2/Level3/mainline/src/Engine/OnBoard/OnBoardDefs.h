#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include "Common/containr.h"

#include "Common/Point.h"
#include "Common/Point2008.h"
#include "Common/ARCVector.h"
#include "Common/ARCPath.h"
#include "Common/elaptime.h"

#include "Common/OnBoardStates.h"
#include "Common/OnBoardException.h"
#include "OnBoardMsg.h"

// Tile on Deck.
#define Deck_tile_width 10 //cm
#define Deck_tile_height 10 //cm

// PathFinding weight value.
#define Traversal_directly_cost 10
#define Traversal_diagonal_cost 14


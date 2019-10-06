/*
 *  Copyright (C) 2019	Marco Pulimeno -- University of Salento
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef COMMON_H
#define COMMON_H

//#define ENABLE_TEST // define ENABLE_TEST to enable test code
//#define ENABLE_MIN_TEST

// common headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <assert.h>

#include "bostree.h"

// type for field 'data' of X and Y AVL tree nodes
typedef struct _NodeData {

    long seqNo; //the seqNo of the item, by time

    BOSNode *bs; //in X/Y, this is Bmin
    BOSNode *bg; //in X/Y, this is Bmax

    BOSNode *succ; //ptr to successor, by time

} NodeData;

// type for field 'key' of B AVL tree nodes
typedef struct _BNode {
    double value;
    BOSNode *toX; // in B this is x_i
    BOSNode *toY; // in B this is y_j
} BNode;

#endif // COMMON _H

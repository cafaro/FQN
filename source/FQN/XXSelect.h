/**
 * @file XXSelect.h
 * @author CM
 * 
 * @brief Implementation of Arjomandi and Mirzaian procedures
 * 
 * @note working on X+(-X)
 * @note As defined by Arjomandi's paper: Selection in X+Y...
 * 
 */

#ifndef __XX_SELECT_H__
#define __XX_SELECT_H__

#include "Header.h"


struct VALUES_PAIR{
    ITEM_VALUE max;
    ITEM_VALUE min;
};
typedef struct VALUES_PAIR PAIR;

struct INDEX_PAIR{
    ITEMS k1;
    ITEMS k2;
};




// ITEMS inline getSubLen(ITEMS n);
// struct INDEX_PAIR inline getK1K2(ITEMS n, ITEMS k1, ITEMS k2);
// ITEMS pickL(ITEMS max_len, ITEM_VALUE L[max_len], ITEM_VALUE max, ITEM_VALUE min, ITEMS n, ITEM_VALUE X[n]);
// ITEMS rankLeftV(ITEMS n, ITEM_VALUE X[n], ITEM_VALUE a);
// ITEMS rankRightV(ITEMS n, ITEM_VALUE X[n], ITEM_VALUE a);
// void getSubVector(ITEMS n, ITEM_VALUE X[n], ITEMS n_sub, ITEM_VALUE X_sub[n_sub]);
// struct VALUES_PAIR biselectV(ITEMS n, ITEM_VALUE X[n], ITEMS k1, ITEMS k2);



/**
 * @brief Starts the selection of the k-th element in the matrix obtained ad X+(-X) (as defined by Arjomandi in X+Y) 
 * @param n, the length of the vector in input
 * @param X, the input vector
 * @param k, the k-th order statistic
 * @return the k-th element in the matrix X+(-X)
 */
ITEM_VALUE selectStatisticFrom(ITEMS n, ITEM_VALUE X[n], ITEMS k);

#endif //__XX_SELECT_H__

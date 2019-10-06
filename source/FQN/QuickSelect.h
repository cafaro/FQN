/**
 * @file QuickSelect.h
 * @author CM
 * 
 * @brief Implementation of some statistic functions.
 * 
 * 
 * (Some sample C code for the quickselect algorithm, taken from Numerical Recipes in C):
 * http://www.stat.cmu.edu/~ryantibs/median/quickselect.c
 * 
 */

#ifndef __QSELECT_H__
#define __QSELECT_H__

#include "Header.h"

#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;



/**
 * @brief In an array of data, of type ITEM_VALUE, with length 'len', the statistic of order 'pos' is given by:
 * 
 * @param data, the dataset/window
 * @param len, the len of the dataset/window
 * @param pos, the order statistic to calculate (passed as order-1)
 * 
 * @return the pos-th element of the array
 * 
 * @note: beginning from 0, so pos is equal to (order+1) 
 */
ITEM_VALUE quickselect(ITEM_VALUE *data, ITEMS len, ITEMS pos);



/**
 * @brief get the median of an unorder dataset of len len
 * 
 * @param data, the dataset/window
 * @param len, the len of the dataset/window
 * 
 * @return the median of the ordered set
 */
ITEM_VALUE getQ2(ITEM_VALUE *data, ITEMS N);




/**
 * @brief get the first quartile of an unorder dataset of len len
 * 
 * @param data, the dataset/window
 * @param len, the len of the dataset/window
 * 
 * @return the Q1 of the ordered set
 */
ITEM_VALUE getQ1(ITEM_VALUE *data, ITEMS N);


#endif //__QSELECT_H__

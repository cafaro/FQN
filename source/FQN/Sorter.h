/**
 * @file Sorter.h
 * @author CM
 * @brief Functions that implements incremental sorting on fixed windows
 * 
 */

#ifndef __SORTER_H__
#define __SORTER_H__

#include "Header.h"

/**
 * @brief Incremental insertion sort: at each time step, a new item is placed in a sorted array
 * @param a, the sorted array
 * @param len, the number of items already in the array
 * @param new the value to add and sort in the array
 * @return void
 * 
 * Used by v3
 * 
 * @note: At the end, the function returns the updated and sorted array V, with len+1 items
 */
void isort_v3(ITEM_VALUE *V, ITEMS_COUNT len, ITEM_VALUE new);

void updateValues_v3(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old);



void isort_v4(ITEM_VALUE *V, ITEMS_COUNT len, ITEM_VALUE new);
void updateValues_v4(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old);



void isort_v5(ITEM_VALUE *V, ITEMS_COUNT len, ITEM_VALUE new);
void updateValues_v5(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old);


void updateValues_v51(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old);

#endif //__SORTER_H__

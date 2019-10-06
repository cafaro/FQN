/**
 * @file Debugger.h
 * @author CM
 * @brief Debug functions and logs
 * 
 */

#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include "Header.h"

struct debugFiles{
    FILE *fp_in;
    FILE *fp_twin;
    FILE *fp_vwin;
};


int openDebugFiles(char *file1, char *file2, char *file3, struct debugFiles *df);
void closeDebugFiles(struct debugFiles *df);


/**
 * @brief Function that prints the current window, sorting value by sequence (temporal order)
 * @param t, the time window
 * @param s, the sequence window
 * @param n, the length of the arrays
 * @param oldest, the oldest by time value
 * @param fp, the log file 
 * 
 * @return void
 */
void debugTimeW(ITEM_VALUE *t, ITEMS_COUNT *s, ITEMS_COUNT n, ITEMS_COUNT oldest, FILE *fp);


/**
 * @brief Function that prints the current values window, sorting by value (insertion sorting)
 * @param v, the values window, sorted
 * @param n, the length of the window
 * @param fp, the log file 
 * 
 * @return void
 */
void debugValueW(ITEM_VALUE *v, ITEMS_COUNT n, FILE *fp);



/**
 * @brief This function creates a copy of the time window and sorts it. 
 *        Then, it compares array of values (sorted with incremental insertion sort) 
 *        with the sorted copy of time window, element by element, and prints to stderr non matching elements/correspondance...
 *
 * @param t, the window ordered by time
 * @param v, the window ordered by value
 * @param n, the size of the window
 * 
 * @return void
 */
void debugWindows(ITEM_VALUE *t, ITEM_VALUE *v, ITEMS_COUNT n);



/**
 * @brief Given the time window, the function computes the n*n differences, then sorts them and select the k-th differences
 * comparing it with the value returned by Arjomandi algo...
 * 
 * @param knew, (kth+offset) statistic order to look for in the set of (n*n) differences 
 * @param kth_value, the order statistic returned by Arjomandi algo
 * @param t, the time window
 * @param n, the len of the window
 * 
 * @return void
 * 
 */
void CheckArjomandi(ITEMS_COUNT knew, ITEM_VALUE kth_value, ITEM_VALUE *t, ITEMS_COUNT n);


/**
 * @brief Given the time window, the function computes the n*n differences, then use quickselect to return the exact order statistic
 * comparing it with the value returned by Arjomandi algo...
 * 
 * @param knew, (kth+offset) statistic order to look for in the set of (n*n) differences 
 * @param kth_value, the order statistic returned by Arjomandi algo
 * @param t, the time window
 * @param n, the len of the window
 * 
 * @return void
 * 
 */
void CheckForArjomandi(ITEMS_COUNT knew, ITEM_VALUE kth_value, ITEM_VALUE *t, ITEMS_COUNT n);

#endif //__DEBUGGER_H__

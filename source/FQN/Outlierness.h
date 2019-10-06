/**
 * @file Outlierness.h
 * @author CM
 * @brief Management of the output logs and check for outlierness of points.
 * 
 */

#ifndef __OUTLIERNESS_H__
#define __OUTLIERNESS_H__

#include "Header.h"


#define NAMELEN 128             //output file name max len

/**
 * @brief lopPoint used to store info about outliers and inliners
 */
struct logPoint {   
    ITEM_VALUE item;
    ITEMS_COUNT seq;
};


struct OutputsStats {

    ITEM_VALUE  QnScaleFactor;  //Qn Scale Factor, as defined by Rousseeuw and Croux
    
    ITEMS_COUNT inliers;        //inliers count
    ITEMS_COUNT outliers;       //outliers count
    
    FILE        *fp_inliers;    //log of inliers to file
    FILE        *fp_outliers;   //log of outliers to file

    char fout[NAMELEN];         //path to file for outliers
    char fin[NAMELEN];          //path to file for inliers
    
    ITEMS_COUNT maxSize;
    struct logPoint *logOut;    //array of outliers
    struct logPoint *logIn;     //array of inliers

};


void initStats(ITEMS_COUNT n, struct OutputsStats *o, ITEMS_COUNT k, char *pathfile, ITEMS_COUNT streamLen);

/**
 * @brief Post processing to save binary files to text files..
 */
void closeStats(struct OutputsStats *o);



/**
 * @brief check for outliers: defines scale factor as per Rousseeuw and Croux paper (1992)
 * @param n, the window size
 * @param scalingFactor, the correction factor
 * 
 * @return the Qn Scaling Factor (view paper for details)
 */
ITEM_VALUE getQnScaleFactor(ITEMS n, ITEM_VALUE scalingFactor);



/**
 * @brief check for outliers: given the central item in the time window and its deviation from the median value, 
 * the procedure checks against a threshold to determine if it's outlier or inlier
 * @param middle, the central item in the current window [(streamLen-K), if streamLen is the number of received items and K the half-window]
 * @param seqNo, the sequence number of middle, (logging purposes)
 * @param median, the median of the current time window
 * @param kth_value, the kth order statistic in the current window
 * @param o, the OuputStats containing log and Scale Factor info
 * 
 * @return void, the result is used for logs
 */
void checkForOutlier(ITEM_VALUE middle, ITEMS_COUNT seqNo, ITEM_VALUE median, ITEM_VALUE kth_value, struct OutputsStats *o);

#endif //__OUTLIERNESS_H__

/**
 * @file Outlierness.c
 * @author CM
 */

#include "Outlierness.h"

void initStats(ITEMS_COUNT n, struct OutputsStats *o, ITEMS_COUNT k, char *pathfile, ITEMS_COUNT streamLen) {

	ITEM_VALUE QFactor = 2.2219; //as defined by Rousseeuw and Croux (paper 1992)
	o->QnScaleFactor = getQnScaleFactor(n, QFactor);

    o->outliers = 0;
	o->inliers = 0;

    bool quit=false;
    int u = strlen(pathfile);
    while(!quit && u>=0) {
        if(pathfile[u]=='/'){
			quit = true;
            break;
        }
        --u;
    }//wend

	// char fout[NAMELEN] = {0};
	// char fin[NAMELEN] = {0};
    snprintf(o->fout, NAMELEN-1, "data/Outliers/%.4s_out_%ld.csv", &pathfile[u+1], k);
    snprintf(o->fin, NAMELEN-1, "data/Inliers/%.4s_in_%ld.csv", &pathfile[u+1], k);

	#ifndef TESTMODE
	
	//log to files
    o->fp_outliers = fopen(o->fout, "w");
    o->fp_inliers = fopen(o->fin, "w");
	
	#else
	
	//or buffering to arrays
	o->maxSize = streamLen;
	o->logOut = malloc(sizeof(struct logPoint)*streamLen);    //array of outliers
    o->logIn = malloc(sizeof(struct logPoint)*streamLen);     //array of inliers
	
	#endif
}



void closeStats(struct OutputsStats *o) {

	if (o != NULL) {

		#ifndef TESTMODE
		
		fclose(o->fp_outliers);
		fclose(o->fp_inliers);
		
		#else

		if (o->logOut){

			FILE *fp_log_out = fopen(o->fout, "w");
			if (fp_log_out == NULL){
				fprintf(stderr, "Error opening %s\n", o->fout);
				return;
			}

			for(ITEMS_COUNT u = 0; u<(o->outliers); ++u){
				fprintf(fp_log_out, "%ld,%.3f\n", o->logOut[u].seq - 1, o->logOut[u].item);
			} 
			fclose(fp_log_out);
			free(o->logOut);
		}
		
		if (o->logIn){

			FILE *fp_log_in = fopen(o->fin, "w");
			if (fp_log_in == NULL){
				fprintf(stderr, "Error opening %s\n", o->fin);
				return;
			}

			for(ITEMS_COUNT u = 0; u<(o->inliers); ++u){
				fprintf(fp_log_in, "%ld,%.3f\n", o->logIn[u].seq -1, o->logIn[u].item);
			} 
			fclose(fp_log_in);
			
			free(o->logIn);
		}
		
		#endif
	}
}

//******************************************************		Qn estimator


ITEM_VALUE getQnScaleFactor(ITEMS n, ITEM_VALUE scalingFactor){
	ITEM_VALUE dn;
	if (n <= 9) 
	{
		if (n == 2) {
			dn = .399;
		} else if (n == 3) {
			dn = .994;
		} else if (n == 4) {
			dn = .512;
		} else if (n == 5) {
			dn = .844;
		} else if (n == 6) {
			dn = .611;
		} else if (n == 7) {
			dn = .857;
		} else if (n == 8) {
			dn = .669;
		} else {
			dn = .872;
		}
	}
	else
	{
		if (n % 2 == 1) {
			dn = n / (n + 1.4);
		} else {
			dn = n / (n + 3.8);
		}
	}//fi (n<9)
	return  (dn * scalingFactor);
}



void checkForOutlier(ITEM_VALUE middle, ITEMS_COUNT seqNo, ITEM_VALUE median, ITEM_VALUE kth_value, struct OutputsStats *o){
    
    //check if the central point in window is outlier:
	if ( (fabs(middle - median) - 3*(o->QnScaleFactor)*kth_value) > 0) {
    
        //outlier!

		#ifdef CHECKS
		fprintf(o->fp_outliers, "[%.1f, %.1f, %.3f]\t%ld,%.3f\n", middle, median, kth_value, seqNo, middle);
		#endif
		
		#ifndef TESTMODE
        fprintf(o->fp_outliers, "%ld,%.3f\n", seqNo, middle);
		#else
		o->logOut[o->outliers].item = middle;
		o->logOut[o->outliers].seq = seqNo;
		#endif
		
        ++o->outliers;

	} else {
        
        //not outlier
		
		#ifdef CHECKS
		fprintf(o->fp_inliers, "[%.1f, %.1f, %.3f]\t%ld,%.3f\n", middle, median, kth_value, seqNo, middle);
		#endif
		
		#ifndef TESTMODE
        fprintf(o->fp_inliers, "%ld,%.3f\n", seqNo, middle);
		#else
		o->logIn[o->inliers].item = middle;
		o->logIn[o->inliers].seq = seqNo;
		#endif
        
		++o->inliers;

    }//fi check test
}

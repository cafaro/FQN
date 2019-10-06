/**
 * @file QuickSelect.c
 * @author CM
 * 
 * @brief Implementation of statistics
 */

#include "QuickSelect.h"


ITEM_VALUE quickselect(ITEM_VALUE *data, ITEMS len, ITEMS pos) {
  
  ITEMS i, ir, j, l, mid;
  ITEM_VALUE a, temp;

  l=0;
  ir=len-1;

  for(;;) {
    if (ir <= l+1) { 
      if (ir == l+1 && data[ir] < data[l]) {
	    SWAP(data[l],data[ir]);
      }
    return data[pos];
    }
    else 
    {
      mid=(l+ir) >> 1; 
      SWAP(data[mid],data[l+1]);
      if (data[l] > data[ir]) {
	    SWAP(data[l],data[ir]);
      }
      if (data[l+1] > data[ir]) {
	    SWAP(data[l+1],data[ir]);
      }
      if (data[l] > data[l+1]) {
	    SWAP(data[l],data[l+1]);
      }
      i=l+1; 
      j=ir;
      a=data[l+1];

      for (;;) { 
	    do i++; while (data[i] < a); 
	    do j--; while (data[j] > a); 
	    if (j < i) break; 
	    SWAP(data[i],data[j]);
      } 
      data[l+1]=data[j]; 
      data[j]=a;
      if (j >= pos) ir=j-1; 
      if (j <= pos) l=i;
    }
  }

}


ITEM_VALUE getQ2(ITEM_VALUE *data, ITEMS N){
  
  ITEMS median_index;
  ITEM_VALUE median;
  
  if (N % 2 == 1) 
  {
    median_index = (N+1)/2;
    ////fprintf(stderr, "N odd (dispari): %ld\n", median_index);
    median = quickselect(data, N, median_index-1);
    //fprintf(stderr, "\tMedian %.3f (pos %ld)\n", median, median_index-1);
  } 
  else 
  {
    median_index = N/2;
    ////fprintf(stderr, "N even (pari): %ld\n", median_index);
    ////fprintf(stderr, " median inf on %ld items = %ld\n", N, median_index-          
    ITEM_VALUE median_inf = quickselect(data, N, median_index-1);
    ITEM_VALUE median_sup = quickselect(data, N, median_index);
    median = (median_inf + median_sup)/2.0;
    //fprintf(stderr, "\tMedian %.3f (pos %ld) ((inf+sup)/2)\n", median, median_index-1);
  }//fi Q2

  return median;
}


ITEM_VALUE getQ1(ITEM_VALUE *data, ITEMS N){

  if (N==1){
    return data[0];
  }

  ITEMS Q1_index;
  ITEM_VALUE Q1_value;

  if (N % 4 == 2) 
  {
    Q1_index = (N+2)/4;

    if (Q1_index < 1) {
      Q1_index = 1;
    }
    ////fprintf(stderr,"(then branch), Q1_index: %ld\n", Q1_index);
    fprintf(stderr, "Q1_index esatto:%ld\n", Q1_index-1);

    Q1_value = quickselect(data, N, Q1_index-1);
    //fprintf(stderr, "\tQ1_value: %.3f (pos %ld)\n", Q1_value, Q1_index-1);
  } 
  else 
  {
    //Q1: (sup-inf)remainder/4 + inf
    ITEMS remainder = (N+2)%4;
    Q1_index = (N+2)/4; 

    if (Q1_index < 1) {
      Q1_index = 1;
    }
    ////fprintf(stderr,"(else branch), Q1_index: %ld\n", Q1_index);
    fprintf(stderr, "Q1_index inf:%ld\n", Q1_index-1);

    ITEM_VALUE q1_inf = quickselect(data, N, Q1_index-1);
    ITEM_VALUE q1_sup = quickselect(data, N, Q1_index);
    Q1_value = (q1_sup - q1_inf)*(remainder/4.0) + q1_inf;
    //fprintf(stderr, "\tQ1_value: %.3f (pos %ld) ((inf+sup)/4)\n", Q1_value, Q1_index-1);   
  }//fi Q1

  return Q1_value;
}
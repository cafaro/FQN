/**
 * @file XXSelect.c
 * @author CM 
 */
#include <sys/time.h>
#include "XXSelect.h"
#include "QuickSelect.h"


ITEMS getSubLen(ITEMS n){
    return (ITEMS)ceil( (double)( (n+1)/2.0) );
}


struct INDEX_PAIR getK1K2(ITEMS n, ITEMS k1, ITEMS k2){
    struct INDEX_PAIR K;

    if(n%2==0) {
        //n even (pari)
        K.k1 = n + 1 + ceil((double)(k1/4.0));
    } else {
        //n odd (dispari)
        K.k1 = ceil((double)((2*n+1+k1)/4.0));
    }
    K.k2 = (int)floor((double)((k2+3)/4.0));

return K;
}



ITEMS pickL(ITEMS max_len, ITEM_VALUE L[max_len], ITEM_VALUE max, ITEM_VALUE min, ITEMS n, ITEM_VALUE X[n]){

    ITEMS k = 0; //in_range_items

    ITEMS i = 0;    //row index
    ITEMS j = 0;    //col index
    ITEMS poisoned_column = 0;

    for(i=1;i<n;++i) //first rown in X+(-X) can be dropped, also last column
    {
        j = poisoned_column;
        while(j<i && (X[i]-X[j])>min) // while(j<n && (X[i]-X[j])>min)
        { //we need only lower triangular values
            if ( (X[i]-X[j]) >= max ) {
                poisoned_column = j+1;
            } 
            else //if ( (X[i]-X[j])<max)
            {
                L[k] = X[i]-X[j];
                ++k;
            }//fi
            ++j;
        }//wend
    }//for

    return k; 
}



ITEMS rankLeftV(ITEMS n, ITEM_VALUE X[n], ITEM_VALUE a){
    ITEMS j = 0;
    ITEMS x = 0;

    for (ITEMS i=0; i < n; ++i){
    
        while(j<i && ((X[i]-X[j]) >= a) ){ //while(j<n && ((X[i]-X[j]) >= a) ){
            // fprintf(stderr, "while loop %d for rankLeft\n", ++count);
            ++j;
        }
        x += n - j;
    }

return x;
}



// ITEMS rankRightV(ITEMS n, ITEM_VALUE X[n], ITEM_VALUE a){
//     ITEMS j=0;
//     ITEMS k = 0;
//     ITEMS i = 0;
//     for (i=0; i<n; ++i){
//         j=0;
//         while (j<i && (X[i]-X[j] > a)){
//             ++j;
//             ++k;
//         }
//     }
//     return k;
// }

ITEMS rankRightV(ITEMS n, ITEM_VALUE X[n], ITEM_VALUE b){
    //versione 3
    ITEMS i, j=0, rank=0; // =n for the first line
    for (i=0; i<n; ++i){
        while (j<i && (X[i]-X[j] > b)){
            ++j;
        }
        rank += j; 
    }

    return rank;
}




void getSubVector(ITEMS n, ITEM_VALUE X[n], ITEMS n_sub, ITEM_VALUE X_sub[n_sub]){
    ITEMS k = 0;

    for(ITEMS r=0; r<n; r+=2){
        X_sub[k] = X[r];
        ++k;
    }
    if (k <= (n_sub-1)) {
        X_sub[n_sub-1] = X[n-1];
    }
}




PAIR biselectV(ITEMS n, ITEM_VALUE X[n], ITEMS k1, ITEMS k2){

    PAIR P;
    
    if (n <= 2) 
    {
        P.max = X[n-1] - X[0];
        P.min = X[0]-X[n-1];
    } 
    else 
    {
        //ITEMS n_sub = getSubLen(n);
        ITEMS n_sub = (ITEMS)ceil((double)( (n+1)/2.0) );

        //struct INDEX_PAIR K = getK1K2(n, k1, k2);
        ITEMS _k1, _k2;
        if(n%2==0){
            //n even, pari
            _k1 = (n+1) + (ITEMS)ceil((double)(k1/4.0));
        }else{
            //n odd, dispari
            _k1 = (ITEMS)ceil((double)( (2*n + 1 + k1)/4.0 ));
        }
        _k2 = (ITEMS)floor((double)((k2+3)/4.0));

        ITEM_VALUE *X_sub = malloc(sizeof(ITEM_VALUE)*n_sub);
        getSubVector(n, X, n_sub, X_sub);
       
        PAIR candidates = biselectV(n_sub, X_sub, _k1, _k2);//biselectV(n_sub, X_sub, K.k1, K.k2); //
        free(X_sub);

        ITEMS ra = rankLeftV(n, X, candidates.max);
        ITEMS rb = rankRightV(n, X, candidates.min);
        
        ITEM_VALUE *L = NULL;
        ITEMS items = 0;

        if (ra <= (k1-1)) {
            P.max = candidates.max;
        } else if ((k1 + rb - n*n) <= 0) {
            P.max = candidates.min;
        } else {
            
            ITEMS max_len = ra + rb - (n*n);
            L = malloc(sizeof(ITEM_VALUE) * max_len);
            if (L == NULL){
                fprintf(stderr, "Not enough memory for L\n\n");
                exit(128);
            }
            items = pickL(max_len, L, candidates.max, candidates.min, n, X);
            P.max = quickselect(L, items, (k1+rb-n*n)-1);

        }//fi k1
      

        if (ra <= (k2 - 1)) {
            P.min = candidates.max;
        } 
        else if ((k2+rb-n*n) <= 0)
        {
            P.min = candidates.min;
        } else {  

            if (L==NULL) {
                
                ITEMS max_len = ra + rb - (n*n); 
                L = malloc(sizeof(ITEM_VALUE) * max_len);
                if (L == NULL){
                    fprintf(stderr, "Not enough memory for L\n\n");
                    exit(128);
                }
                items = pickL(max_len, L, candidates.max, candidates.min, n, X);
            }
            P.min = quickselect(L, items, (k2+rb-n*n)-1);

        }//fi k2

        if (L){
            free(L);
            items = 0;
        }

    }//fi recursion n>2

    return P;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% PUBLIC 


ITEM_VALUE selectStatisticFrom(ITEMS n, ITEM_VALUE X[n], ITEMS k){
    PAIR xy = biselectV(n, X, k, k);
    return xy.max;
}

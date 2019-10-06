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

/*  The qn* and whimed routines are modified versions of the original C code
 *  in files qn_sn.c by Martin Maechler and wgt_himed_templ.h,
 *  which are contained in the robustbase R package.
 *  The qselect routine and relative support functions are cleaned and adapted versions
 *  from https://github.com/swenson/sort code, distributed by Christopher Swenson under
 *  the MIT License (MIT).
 */

#ifndef QN_H
#define QN_H

#include <stdint.h>
#include "bostree.h"

typedef struct _M_point {
    double value;
    int x_idx;
    int y_idx;
} M_point;

/* Main routines */
double qn(double *x, int n, int finite_corr);
/* these have no extra factors (no consistency factor & finite_corr): */
double qn0(double *x, int n);
/* this takes three AVL trees as input and implements offline qn0 computation in Nunkesser et al. algorithm */
BOSNode* compute_qn0_offline(BOSTree *X, BOSTree *Y, BOSTree *B, int n, double *qnvalue);

/* Support functions */
double qselect(double *arr, int n, int k);
double whimed(double *a, int *w, int n,
              double* a_cand, int* w_cand);

#endif //QN_H

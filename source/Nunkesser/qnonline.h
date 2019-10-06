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


#include "bostree.h"
#include "common.h"
#include "qn.h"

#ifndef QNONLINE_H
#define QNONLINE_H

int insert_x_item(double xnew, long seq_no, BOSTree *X, BOSTree *Y, BOSTree *B);
int delete_x_item(double xdel, BOSTree *X, BOSTree *Y, BOSTree *B);
int insert_y_item(double ynew, long seq_no, BOSTree *X, BOSTree *Y, BOSTree *B);
int delete_y_item(double ydel, BOSTree *X, BOSTree *Y, BOSTree *B);

void init_trees(double *items, long w, BOSTree **X, BOSTree **Y, BOSTree **B);
void delete_trees(BOSTree **X, BOSTree **Y, BOSTree **B);
void reset_B (BOSTree **B);
BOSNode *update_window(double xnew, int seqno, double xdel, BOSTree *X, BOSTree *Y, BOSTree *B, BOSNode* qn0);

void print_X_plus_Y (BOSTree *X, BOSTree *Y);
void print_X (BOSTree *tree);
void print_Y (BOSTree *tree);
void print_B (BOSTree *B);
int test_verify_pointers(BOSTree *X, BOSTree *Y, BOSTree *B, BOSNode *qn0, bool delete_qn);

#endif //QNONLINE_H

// MASTER-ONLY: DO NOT MODIFY THIS FILE

/*
 * Copyright (C) Telecom Paris
 * 
 * This file must be used under the terms of the CeCILL. This source
 * file is licensed as described in the file COPYING, which you should
 * have received as part of this distribution. The terms are also
 * available at:
 * http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt
*/

#include <stdlib.h>
#include <math.h>

#include "utils.h"
#include "pcc.h"

void pcc_s2s(float *pcc, int samples_length, int ny, float *x, int **y) {
	int i, j;
	float sumx, sumx2, *sumxy, tmp1, tmp2;
	long *sumy, *sumy2;

	if(samples_length < 2) {
		ERROR(, -1, "not enough realizations (%d, min 2)", samples_length);
	}
	if(ny < 1) {
		ERROR(NULL, -1, "Invalid number of Y random variables: %d", ny);
	}
	sumx = 0.0;
	sumx2 = 0.0;
	sumy = XMALLOC(ny * sizeof(long));
	sumxy = XMALLOC(ny * sizeof(float));
	sumy2 = XMALLOC(ny * sizeof(long));
	for(j = 0; j < ny; j++) {
		sumy[j] = 0;
		sumy2[j] = 0;
		sumxy[j] = 0.0;
	}
	for(i = 0; i < samples_length; i++) {
		sumx += x[i];
		sumx2 += x[i] * x[i];
		for(j = 0; j < ny; j++) {
			sumy[j] += y[j][i];
			sumxy[j] += x[i] * y[j][i];
			sumy2[j] += y[j][i] * y[j][i];
		}
	}
	tmp1 = sqrt(samples_length * sumx2 - sumx * sumx);
	if (tmp1 == 0.0) {
		ERROR(, -1, "X variance equals zero; could it be that your X variable is constant?");
	}
	for(j = 0; j < ny; j++) {
		tmp2 = sqrt(samples_length * sumy2[j] - sumy[j] * sumy[j]);
		if(tmp2 == 0.0) {
			ERROR(, -1, "Y%d variance equals zero; could it be that your Y%d variable is constant?", j, j);
		}
		pcc[j] = (samples_length * sumxy[j] - sumx * sumy[j]) / (tmp1 * tmp2);
	}
}

void pcc_v2s(float **pcc, int samples_length, int vector_length, int ny, float **x, int **y) {
	int i, j, k;
	float *sumx, *sumx2, sumy, sumy2, *sumxy, tmp;

	if(samples_length < 2) {
		ERROR(, -1, "not enough realizations (%d, min 2)", samples_length);
	}
	if(vector_length < 1) {
		ERROR(, -1, "invalid length of X vector (%d, min 1)", vector_length);
	}
	if(ny < 1) {
		ERROR(NULL, -1, "Invalid number of Y random variables (%d, min 1)", ny);
	}
	sumx = XMALLOC(vector_length * sizeof(float));
	sumx2 = XMALLOC(vector_length * sizeof(float));
	sumxy = XMALLOC(vector_length * sizeof(float));
	for(i = 0; i < vector_length; i++) {
		sumx[i] = 0.0;
		sumx2[i] = 0.0;
		for(j = 0; j < samples_length; j++) {
			sumx[i] += x[j][i];
			sumx2[i] += x[j][i] * x[j][i];
		}
		pcc[ny - 1][i] = sqrt(samples_length * sumx2[i] - sumx[i] * sumx[i]);
		if (pcc[ny - 1][i] == 0.0) {
			ERROR(, -1, "X[%d] variance equals zero; could it be that it is constant?", i);
		}
	}
	for(k = 0; k < ny; k++) {
		sumy = 0.0;
		sumy2 = 0.0;
		for(j = 0; j < samples_length; j++) {
			sumy += y[k][j];
			sumy2 += y[k][j] * y[k][j];
		}
		tmp = sqrt(samples_length * sumy2 - sumy * sumy);
		if(tmp == 0.0) {
			ERROR(, -1, "Y%d variance equals zero; could it be that it is constant?", k);
		}
		for(i = 0; i < vector_length; i++) {
			sumxy[i] = 0.0;
			for(j = 0; j < samples_length; j++) {
				sumxy[i] += x[j][i] * y[k][j];
			}
			pcc[k][i] = (samples_length * sumxy[i] - sumx[i] * sumy) / (pcc[ny - 1][i] * tmp);
		}
	}
	free(sumx);
	free(sumx2);
	free(sumxy);
}

// vim: set tabstop=4 softtabstop=4 shiftwidth=4 noexpandtab textwidth=0:

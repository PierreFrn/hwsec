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

#ifndef PCC_H
#define PCC_H

/** \file pcc.h
 *  The \b pcc library, a software library dedicated to the computation of estimates of Pearson Correlation Coefficients (PCC).
 *  \author Renaud Pacalet, renaud.pacalet@telecom-paris.fr
 *  \date 2020-05-05
 *
 * Defines functions to estimate Pearson Correlation Coefficients (PCC) between a random variable \f$X\f$ and a set of \f$ny\f$ random variables \f$Y_n, 0\le n<ny\f$.
 *
 * These coefficients are statistical tools to evaluate the correlation between random variables. The formula of a PCC between random variables \f$X\f$ and \f$Y\f$ is: \f$PCC(X,Y) = [E(X\times Y) - E(X)\times E(Y)] / [\sigma(X)\times\sigma(Y)]\f$ where \f$E(Z)\f$ is the expectation of random variable \f$Z\f$ and \f$\sigma(Z)\f$ is its standard deviation. The value of the PCC is in range -1 to +1. Values close to 0 indicate no or a weak correlation. Values close to -1 or +1 indicate strong correlations.
 */

/** The \b `pcc_s2s` (`s2s` for scalar-to-scalar) function estimates the PCC between a floating point random variable \f$X\f$ and a set of \f$ny\f$ integer random variables \f$Y_n, 0\le n<ny\f$, based on samples of each. The sample of \f$X\f$ is passed as an array `x` of `float` values where `x[i]` is the `i`-th realization in the sample. The samples of the \f$Y_n\f$ are passed as an array of \f$ny\f$ arrays of `int` values where `y[n][i]` is the `i`-th realization in the sample of \f$Y_n\f$. The length of the samples must be specified and set to the smallest of all samples' lengths. The extra realizations in larger samples, if any, are ignored.
 *
 * Example of use with samples of length 1000 and 4 \f$Y_n\f$ variables. `get_next_x` and `get_next_y(n)` are two functions returning realizations of the \f$X\f$ and \f$Y_n\f$ random variables, respectively:
 * \code
 * float *pcc;          // array of float for the result PCCs
 * float *x;            // array of float for the X sample
 * int **y;             // array of arrays of int for the samples of the Yn
 * int ny;              // number of Yn variables
 * int n;               // index of Yn variables
 * int samples_length;  // length of samples
 * int i;               // index of realizations in samples
 * ...
 * ny = 4;
 * samples_length = 1000;
 * ...
 * pcc = malloc(ny * sizeof(float));               // allocate array for the result PCCs
 * x = malloc(samples_length * sizeof(float));     // allocate array for the X sample
 * y = malloc(ny * sizeof(int *));                 // allocate array of arrays for the Yn samples
 * for(n = 0; n < ny; n++) {
 *   y[n] = malloc(samples_length * sizeof(int));
 * }
 * for(i = 0; i < samples_length; i++) {
 *     x[i] = get_next_x();                        // realization of random variable X
 *     for(n = 0; n < ny; n++) {
 *         y[n][i] = get_next_y(n);                // realization of random variable Yn
 *     }
 * }
 * pcc_s2s(pcc, samples_length, ny, x, y);         // compute ny PCC estimates at once
 * for(n = 0; n < ny; n++) {
 *   printf("PCC(X,Y%d) = %f\n", n, pcc[n]);       // print PCC(X,Yn)
 * }
 * free(pcc);                                      // free array of PCC results
 * free(x);                                        // free array of X sample
 * for(n = 0; n < ny; n++) {                       // free arrays of Yn samples
 *   free(y[n]);
 * }
 * free(y);
 * \endcode
 *
 * If there is only one single \f$Y\f$ variable the code can be silghtly simplified:
 *
 * \code
 * float pcc;           // result PCC
 * float *x;            // array of float for the X sample
 * int *y;              // array of int for the Y sample
 * int i;               // index of realizations in samples
 * int samples_length;  // length of samples
 * ...
 * samples_length = 1000;
 * ...
 * x = malloc(samples_length * sizeof(float)); // allocate array for the X sample
 * y = malloc(samples_length * sizeof(int));   // allocate array for the Y sample
 * for(i = 0; i < samples_length; i++) {
 *     x[i] = get_next_x();                    // realization of random variable X
 *     y[i] = get_next_y();                    // realization of random variable Y
 * }
 * pcc_s2s(&pcc, samples_length, 1, x, &y);    // compute PCC estimate
 * printf("PCC(X,Y) = %f\n", pcc);             // print PCC(X,Y)
 * free(x);                                    // free array of X sample
 * free(y);                                    // free array of Y sample
 * \endcode
 *
 * Note that, if \f$ny>1\f$, using \f$ny\f$ times this second version is not equivalent to using only once the first version because many operations involving only \f$X\f$ would be re-computated \f$ny\f$ times. If \f$ny>1\f$ always prefer the first version, it is far more efficient.
 *
 */
void pcc_s2s(
		float *pcc,         /**< array of result PCCs, pcc[n] = \f$PCC(X,Y_n)\f$ */
		int samples_length, /**< length of smallest sample */
		int ny,             /**< number of \f$Yn\f$ random variables */
		float *x,           /**< \f$X\f$ sample, x[i] = i-th realization of \f$X\f$ */
		int **y             /**< \f$Y_n\f$ samples, y[n][i] = i-th realization of \f$Yn\f$ */
		);

/** The \b `pcc_v2s` (`v2s` for vector-to-scalar) function estimates the PCC between a **vector** floating point random variable (denoted \f$X\f$) and a set of \f$ny\f$ **scalar** integer random variables \f$Y_n, 0\le n<ny\f$, based on samples of each. Each realization of \f$X\f$ is thus itself an array of `float` components. Pearson coefficients are computed component-wise of \f$X\f$ and the results are vectors of PCC estimates: \f$PCC(X,Y_n)[i]=PCC(X[i],Y_n), 0\le i<\lvert X\rvert, 0\le n<ny\f$. The sample of \f$X\f$ is passed as an array of arrays of `float` values where `x[i][j]` is the `j`-th component of the `i`-th realization in the sample of \f$X\f$. The samples of the \f$Y_n\f$ are passed as an array of \f$ny\f$ arrays of `int` values where `y[n][i]` is the `i`-th realization in the sample of \f$Y_n, 0\le n<ny\f$. The length of the samples must be specified and set to the smallest of all samples' lengths. The extra realizations in larger samples, if any, are ignored. Example of use with 500-components vectors, samples of length 1000 and 4 \f$Y_n\f$ variables. `get_next_x(j)` and `get_next_y(n)` are two functions returning realizations of the `j`-th component of \f$X\f$ and of \f$Y_n\f$, respectively:
 * \code
 * float **pcc;         // array of arrays of float for the result PCCs
 * float **x;           // array of arrays of float for the X sample
 * int **y;             // array of arrays of int for the Yn samples
 * int i;               // index of realizations in samples
 * int j;               // index of components in vectors
 * int n;               // index of Yn variables
 * int ny;              // number of Yn variables
 * int samples_length;  // length of samples
 * int vectors_length;  // length of vectors
 * ...
 * ny = 4;
 * samples_length = 1000;
 * vectors_length = 500;
 * pcc = malloc(ny * sizeof(float *));              // allocate array of arrays for the result PCCs
 * for(n = 0; n < ny; n++) {
 *   pcc[n] = malloc(vectors_length * sizeof(float));
 * }
 * x = malloc(samples_length * sizeof(float *));    // allocate array of arrays for the X sample
 * for(n = 0; n < ny; n++) {
 *   x[n] = malloc(vectors_length * sizeof(float));
 * }
 * y = malloc(ny * sizeof(int *));                  // allocate array of arrays for the Yn samples
 * for(n = 0; n < ny; n++) {
 *   y[n] = malloc(samples_length * sizeof(int));
 * }
 * for(i = 0; i < samples_length; i++) {
 *   for(j = 0; j < vectors_length; j++) {
 *     x[i][j] = get_next_x(j);                     // realization of j-th component of random variable X
 *   }
 *   for(n = 0; n < ny; n++) {
 *     y[n][i] = get_next_y(n);                     // realization of random variable Yn
 *   }
 * }
 * pcc_v2s(pcc, samples_length, vectors_length, ny, x, y);
 * for(n = 0; n < ny; n++) {
 *   for(j = 0; j < vectors_length; j++) {
 *     printf("PCC(X[%d],Y%d) = %f\n", j, n, pcc[n][j]);
 *   }
 * }
 * for(n = 0; n < ny; n++) {
 *   free(pcc[n]);
 *   free(y[n]);
 * }
 * free(pcc);
 * free(y);
 * for(i = 0; i < samples_length; i++) {
 *   free(x[i]);
 * }
 * free(x);
 * \endcode
 */
void pcc_v2s(
    float **pcc,        /**< array of arrays of result PCCs, pcc[n][j] = j-th component of \f$PCC(X,Y_n)\f$ */
    int samples_length, /**< number of realizations in each sample */
    int vectors_length, /**< length of \f$X\f$ vector random variable */
    int ny,             /**< number of \f$Yi\f$ random variables */
    float **x,          /**< X sample, x[i][j] = j-th component of i-th realization of \f$X\f$ */
    int **y             /**< Y sample, y[n][i] = i-th realization of \f$Y_n\f$ */
    );

#endif /** not PCC_H */

// vim: set tabstop=4 softtabstop=4 shiftwidth=4 noexpandtab textwidth=0:

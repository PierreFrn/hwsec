/*
 * Copyright (C) Telecom Paris
 * 
 * This file must be used under the terms of the CeCILL. This source
 * file is licensed as described in the file COPYING, which you should
 * have received as part of this distribution. The terms are also
 * available at:
 * http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt
*/

/* THIS IS NOT A REAL POWER ATTACK: it assumes that the last round key is
 * 0x0123456789ab. Your goal is to retrieve the true last round key, instead. */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include "utils.h"
#include "traces.h"
#include "des.h"

/* The P permutation table, as in the standard. The first entry (16) is the
 * position of the first (leftmost) bit of the result in the input 32 bits word.
 * Used to convert target bit index into SBox index (just for printed summary
 * after attack completion). */
int p_table[32] = {
  16, 7, 20, 21,
  29, 12, 28, 17,
  1, 15, 23, 26,
  5, 18, 31, 10,
  2, 8, 24, 14,
  32, 27, 3, 9,
  19, 13, 30, 6,
  22, 11, 4, 25
};

tr_context ctx;  // Trace context (see traces.h)
int target_bit;  // Index of target bit.
int target_sbox; // Index of target SBox.
int best_guess;  // Best guess
int best_idx;    // Best argmax
float best_max;  // Best max sample value
float *dpa[64];  // 64 DPA traces
uint64_t rk;     // Last round key

/* A function to allocate cipher texts and power traces, read the
 * datafile and store its content in allocated context. */
void read_datafile (char *name, int n);

/* Compute the average power trace of the traces context ctx, print it in file
 * <prefix>.dat and print the corresponding gnuplot command in <prefix>.cmd. In
 * order to plot the average power trace, type: $ gnuplot -persist <prefix>.cmd
 * */
void average (char *prefix);

/* Decision function: computes bit <target_bit> of L15 for all possible values
 * of the corresponding 6-bits subkey. Takes a ciphertext and returns an array
 * of 64 values (0 or 1). */
void decision (uint64_t ct, int d[64]);

/* Apply P. Kocher's DPA algorithm based on decision function. Computes 64 DPA
 * traces dpa[0..63], best_guess (6-bits subkey corresponding to highest DPA
 * peak), best_max (height of highest DPA peak) and best_idx (index of highest
 * DPA peak). */
void dpa_attack (void);

int main (int argc, char **argv) {
  int n; // Number of acquisitions to use
  int g; // Guess on a 6-bits subkey

  /************************************************************************/
  /* Before doing anything else, check the correctness of the DES library */
  /************************************************************************/
  if (!des_check ()) {
    ERROR (0, -1, "DES functional test failed");
  }

  /*************************************/
  /* Check arguments and read datafile */
  /*************************************/
  /* If invalid number of arguments (including program name), exit with error
   * message. */
  if (argc != 3 && argc != 4) {
    ERROR (0, -1, "\
usage: pa FILE N [B]\n\
  FILE: name of the traces file in HWSec format\n\
  N: number of acquisitions to use\n\
  B: index of target bit in L15 (1 to 32, as in DES standard, default: 1)\n");
  }
  /* Number of acquisitions to use is argument #2, convert it to integer and
   * store the result in variable n. */
  n = atoi (argv[2]);
  if (n < 1) { // If invalid number of acquisitions.
    ERROR (0, -1, "Invalid number of acquisitions: %d (shall be greater than 1)", n);
  }
  target_bit = 1;
  /* If 3 arguments, target bit is argument #3, convert it to integer and store
   * the result in variable target_bit. */
  if (argc == 4) {
    target_bit = atoi (argv[3]);
  }
  if (target_bit < 1 || target_bit > 32) { // If invalid target bit index
    ERROR (0, -1, "Invalid target bit index: %d (shall be between 1 and 32 included)", target_bit);
  }
  // Compute index of corresponding SBox
  target_sbox = (p_table[target_bit - 1] - 1) / 4 + 1;
  /* Read power traces and ciphertexts. Name of data file is argument #1. n is
   * the number of acquisitions to use. */
  read_datafile (argv[1], n);

  /*****************************************************************************
   * Compute and print average power trace. Store average trace in file
   * "average.dat" and gnuplot command in file "average.cmd". In order to plot
   * the average power trace, type: $ gnuplot -persist average.cmd
   *****************************************************************************/
  average ("average");

  /***************************************************************
   * Attack target bit in L15=R14 with P. Kocher's DPA technique *
   ***************************************************************/
  dpa_attack ();

  /*****************************************************************************
   * Print the 64 DPA traces in a data file named dpa.dat. Print corresponding
   * gnuplot commands in a command file named dpa.cmd. All DPA traces are
   * plotted in blue but the one corresponding to the best guess which is
   * plotted in red with the title "Trace X (0xY)" where X and Y are the decimal
   * and heaxdecimal forms of the 6 bits best guess.
   *****************************************************************************/
  // Plot DPA traces in dpa.dat, gnuplot commands in dpa.cmd
  tr_plot (ctx, "dpa", 64, best_guess, dpa);

  /*****************
   * Print summary *
   *****************/
  fprintf (stderr, "DPA traces stored in file 'dpa.dat'.\n");
  fprintf (stderr, "Target bit: %d\n", target_bit);
  fprintf (stderr, "Target SBox: %d\n", target_sbox);
  fprintf (stderr, "Best guess: %d (0x%02x)\n", best_guess, best_guess);
  fprintf (stderr, "Maximum of DPA trace: %e\n", best_max);
  fprintf (stderr, "Index of maximum in DPA trace: %d\n", best_idx);

  /*************************
   * Free allocated traces *
   *************************/
  for (g = 0; g < 64; g++) { // For all guesses for 6-bits subkey
    tr_free_trace (ctx, dpa[g]);
  }
  tr_free (ctx); // Free traces context

  /********************************************
   * Print last round key to standard output. *
   ********************************************/
  rk = UINT64_C(0x0123456789ab); /* 0x0123456789ab last round key. */
  fprintf(stderr, "Last round key (hex):\n");
  printf("0x%012" PRIx64 "\n", rk);

  return 0; // Exits with "everything went fine" status.
}

void read_datafile (char *name, int n) {
  int tn;

  ctx = tr_init (name, n);
  tn = tr_number (ctx);
  if (tn != n) {
    tr_free (ctx);
    ERROR (, -1, "Could not read %d acquisitions from traces file. Traces file contains %d acquisitions.", n, tn);
  }
}

void average (char *prefix) {
  int i;      // Loop index
  int n;      // Number of traces.
  float *sum; // Power trace for the sum
  float *avg; // Power trace for the average

  n = tr_number (ctx);                    // Number of traces in context
  sum = tr_new_trace (ctx);               // Allocate a new power trace for the sum.
  avg = tr_new_trace (ctx);               // Allocate a new power trace for the average.
  tr_init_trace (ctx, sum, 0.0);          // Initialize sum trace to all zeros.
  for (i = 0; i < n; i++) {               // For all power traces
    tr_acc (ctx, sum, tr_trace (ctx, i)); // Accumulate trace #i to sum
  }                                       // End for all power traces
  // Divide trace sum by number of traces and put result in trace avg
  tr_scalar_div (ctx, avg, sum, (float) (n));
  tr_plot (ctx, prefix, 1, -1, &avg);
  fprintf (stderr, "Average power trace stored in file '%s.dat'.\n", prefix);
  tr_free_trace (ctx, sum); // Free sum trace
  tr_free_trace (ctx, avg); // Free avg trace
}

void decision (uint64_t ct, int d[64]) {
  int g;           // Guess
  uint64_t r16l16; // R16|L16 (64 bits state register before final permutation)
  uint64_t l16;    // L16 (as in DES standard)
  uint64_t r16;    // R16 (as in DES standard)
  uint64_t er15;   // E(R15) = E(L16)
  uint64_t l15;    // L15 (as in DES standard)
  uint64_t rk;     // Value of last round key

  r16l16 = des_ip (ct);          // Compute R16|L16
  l16 = des_right_half (r16l16); // Extract right half
  r16 = des_left_half (r16l16);  // Extract left half
  er15 = des_e (l16);            // Compute E(R15) = E(L16)
  /* For all guesses (64). rk is a 48 bits last round key with all 6-bits
   * subkeys equal to current guess g (nice trick, isn't it?). */
  for (g = 0, rk = UINT64_C (0); g < 64; g++, rk += UINT64_C (0x041041041041)) {
    l15 = r16 ^ des_p (des_sboxes (er15 ^ rk));       // Compute L15
    d[g] = (l15 >> (32 - target_bit)) & UINT64_C (1); // Extract value of target bit
  } // End for guesses
}

void dpa_attack (void) {
  int i;         // Loop index
  int n;         // Number of traces.
  int g;         // Guess on a 6-bits subkey
  int idx;       // Argmax (index of sample with maximum value in a trace)
  int d[64];     // Decisions on the target bit

  float *t;      // Power trace
  float max;     // Max sample value in a trace
  float *t0[64]; // Power traces for the zero-sets (one per guess)
  float *t1[64]; // Power traces for the one-sets (one per guess)

  int n0[64];    // Number of power traces in the zero-sets (one per guess)
  int n1[64];    // Number of power traces in the one-sets (one per guess)

  uint64_t ct;   // Ciphertext

  for (g = 0; g < 64; g++) { // For all guesses for 6-bits subkey
    dpa[g] = tr_new_trace (ctx);     // Allocate a DPA trace
    t0[g] = tr_new_trace (ctx);      // Allocate a trace for zero-set
    tr_init_trace (ctx, t0[g], 0.0); // Initialize trace to all zeros
    n0[g] = 0;                       // Initialize trace count in zero-set to zero
    t1[g] = tr_new_trace (ctx);      // Allocate a trace for one-set
    tr_init_trace (ctx, t1[g], 0.0); // Initialize trace to all zeros
    n1[g] = 0;                       // Initialize trace count in one-set to zero
  } // End for all guesses
  n = tr_number (ctx);          // Number of traces in context
  for (i = 0; i < n; i++) { // For all acquisitions
    t = tr_trace (ctx, i);       // Get power trace
    ct = tr_ciphertext (ctx, i); // Get ciphertext
    decision (ct, d);            // Compute the 64 decisions
    for (g = 0; g < 64; g++) { // For all guesses (64)
      if (d[g] == 0) { // If decision on target bit is zero
        tr_acc (ctx, t0[g], t); // Accumulate power trace in zero-set
        n0[g] += 1;             // Increment traces count for zero-set
      }
      else { // If decision on target bit is one
        tr_acc (ctx, t1[g], t);   // Accumulate power trace in one-set
        n1[g] += 1;       // Increment traces count for one-set
      }
    } // End for guesses
  } // End for acquisitions
  best_guess = 0; // Initialize best guess
  best_max = 0.0; // Initialize best maximum sample
  best_idx = 0;   // Initialize best argmax (index of maximum sample)
  for (g = 0; g < 64; g++) { // For all guesses for 6-bits subkey
    tr_scalar_div (ctx, t0[g], t0[g], (float) (n0[g])); // Normalize zero-set
    tr_scalar_div (ctx, t1[g], t1[g], (float) (n1[g])); // Normalize one-set
    tr_sub (ctx, dpa[g], t1[g], t0[g]);                 // Compute one-set minus zero-set
    max = tr_max (ctx, dpa[g], &idx);                   // Get max and argmax of DPA trace
    if (max > best_max || g == 0) { // If better than current best max (or if first guess)
      best_max = max; // Overwrite best max with new one
      best_idx = idx; // Overwrite best argmax with new one
      best_guess = g; // Overwrite best guess with new one
    }
  } // End for all guesses
  // Free allocated traces
  for (g = 0; g < 64; g++) { // For all guesses for 6-bits subkey
    tr_free_trace (ctx, t0[g]);
    tr_free_trace (ctx, t1[g]);
  }
}

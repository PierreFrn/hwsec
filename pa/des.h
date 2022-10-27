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

/** \file des.h
The **des** library, a software library dedicated to the Data Encryption Standard (DES).
\author Renaud Pacalet, renaud.pacalet@telecom-paris.fr
\date 2009-07-08
\attention
- Most functions of the library check their input parameters and issue warnings or errors when they carry illegal values. Warnings are printed on the standard error output. Errors are also printed on the standard error output and the program exits with a -1 exit status.
- The library uses a single data type to represent all the data of the DES standard: `uint64_t`. It is a 64 bits unsigned integer.
- Data are always right aligned: when the data width is less than 64 bits, the meaningful bits are always the rightmost bits of the `uint64_t`.
*/

#ifndef DES_H
#define DES_H

#include <stdint.h>
#include <inttypes.h>

/**
Number of left shifts per round. `left_shifts[0]` corresponds to round #1, ... `left_shifts[15]` corresponds to round #16. A value of 0 means one shift. A value of 1 means two shifts.
*/
extern uint8_t left_shifts[16];

/** Returns the Hamming weight of a 64 bits word. Note: the input's width can be anything between 0 and 64, as long as the unused bits are all zeroes.
 * \return The Hamming weight of the input as a 64 bits uint64_t. */
int hamming_weight (uint64_t val /**< The 64 bits input. */ );

/** Returns the Hamming distance between two 64 bits words. Note: the width of the inputs can be anything between 0 and 64, as long as they are the same, aligned and that the unused bits are all zeroes.
 * \return The Hamming distance between the two inputs as a 64 bits uint64_t. */
int hamming_distance (uint64_t val1 /**< The first 64 bits input. */ ,
		      uint64_t val2
		  /**< The second 64 bits input. */
  );

/**
Initial permutation (64 to 64 bits). Same as `des_n_fp()`.
\return The permutated input as a 64 bits `uint64_t`.
\param val The 64 bits input.
*/
uint64_t des_ip (uint64_t val);

/** Inverse of initial permutation (64 to 64 bits). Same as `des_fp()`.
 * \return The permutated input as a 64 bits `uint64_t`. */
uint64_t des_n_ip (uint64_t val /**< 64 bits input. */ );

/** Final permutation (64 to 64 bits).  Inverse of initial permutation, same as `des_n_ip()`.
 *  \return The permutated input as a 64 bits `uint64_t`. */
uint64_t des_fp (uint64_t val /**< 64 bits input. */ );

/** Inverse of final permutation (64 to 64 bits). Same as `des_ip()`.
 * \return The permutated input as a 64 bits `uint64_t`. */
uint64_t des_n_fp (uint64_t val /**< 64 bits input. */ );

/** E expansion - permutation (32 to 48 bits). \return The expanded and permutated input as a 48 bits `uint64_t`. */
uint64_t des_e (uint64_t val /**< 32 bits input. */ );

/** Inverse of E expansion - permutation (48 to 32 bits). Duplicated bits must have the same value. If they do not, an error is raised.
 * \return The permutated and selected input as a 32 bits `uint64_t`. */
uint64_t des_n_e (uint64_t val /**< 48 bits input. */ );

/** P permutation (32 to 32 bits). \return The permutated input as a 32 bits `uint64_t`. */
uint64_t des_p (uint64_t val /**< 32 bits input. */ );

/** Inverse of P permutation (32 to 32 bits). \return The permutated input as a 32 bits `uint64_t`. */
uint64_t des_n_p (uint64_t val /**< 32 bits input. */ );

/** PC1 permutation - selection (64 to 56 bits). No parity check.
 * \return The permutated and selected input as a 56 bits `uint64_t`. */
uint64_t des_pc1 (uint64_t val /**< 64 bits input. */ );

/** Computes the 8 parity bits of a 64 bits word. Parity bits are the rightmost bit of each byte. Once computed, the number of set bits of each byte is odd, as specified in the DES standard.
 * \return The input with odd parity bits, as a 64 bits `uint64_t`. */
uint64_t des_set_parity_bits (uint64_t val /**< 64 bits input. */ );

/** Inverse of PC1 permutation - selection (56 to 64 bits). Parity bits are computed.
 * \return The permutated and expanded input as a 64 bits `uint64_t`. */
uint64_t des_n_pc1 (uint64_t val /**< 56 bits input. */ );

/** PC2 permutation - selection (56 to 48 bits).
 * \return The permutated and selected input as a 48 bits `uint64_t`. */
uint64_t des_pc2 (uint64_t val /**< 56 bits input. */ );

/** Inverse of PC2 permutation - selection (48 to 56 bits). Missing bits are set to 0.
 * \return The permutated and expanded input as a 56 bits `uint64_t`. */
uint64_t des_n_pc2 (uint64_t val /**< 48 bits input. */ );

/** Single SBox computation (6 to 4 bits).
 * \return The 4 bits output of SBox number sbox corresponding to the 4 bits input, as a 4 bits `uint64_t`. */
uint64_t des_sbox (int sbox /**< SBox number, from 1 to 8. */ ,
		   uint64_t val
		 /**< 6 bits input. */
  );

/** All SBoxes computation (48 to 32 bits).
 * \return The 32 bits output of all SBoxes corresponding to the 48 bits input, as a 32 bits `uint64_t`. */
uint64_t des_sboxes (uint64_t val /**< 48 bits input. */ );

/** Returns the 32 bits right half of a 64 bits word.
 * \return The 32 bits right half of a 64 bits word, as a 32 bits `uint64_t`. */
uint64_t des_right_half (uint64_t val /**< 64 bits input. */ );

/** Returns the 32 bits left half of a 64 bits word.
 * \return The 32 bits left half of a 64 bits word, as a 32 bits `uint64_t`. */
uint64_t des_left_half (uint64_t val /**< 64 bits input. */ );

/** Applies the <strong>left shift</strong> rotation of the standard (56 to 56 bits).
 * \return The rotated input, as a 56 bits `uint64_t`. */
uint64_t des_ls (uint64_t val /**< 56 bits input. */ );

/** Applies the <strong>right shift</strong> rotation of the standard (56 to 56 bits).
 * \return The rotated input, as a 56 bits `uint64_t`. */
uint64_t des_rs (uint64_t val /**< 56 bits input. */ );

/** The F function of the standard (48+32 bits to 32 bits).
 * \return The transformed input, as a 32 bits `uint64_t`. */
uint64_t des_f (uint64_t rk /**< 48 bits round key. */ ,
		uint64_t val
		 /**< 32 bits data input. */
  );

/** Computes the whole key schedule from a 64 bits secret key and stores the sixteen 48 bits round keys in an array. The sixteen 48 bits round keys are returned in the array passed as first parameter. */
void des_ks (
    /** The array where to store the sixteen 48 bits round keys. On return `ks[0]` holds the first round key, ..., `ks[15]` holds the last round key. Must be allocated prior the call. */
	      uint64_t * ks, uint64_t val
		 /**< 64 bits key. */
  );

/** Enciphers a 64 bits plaintext with a pre-computed key schedule.
 * \return The enciphered plaintext as a 64 bits `uint64_t`. */
uint64_t des_enc (uint64_t * ks /**< The pre-computed key schedule. */ ,
		  uint64_t val
		 /**< The 64 bits plaintext. */
  );

/** Deciphers a 64 bits plaintext with a pre-computed key schedule.
 * \return The deciphered ciphertext as a 64 bits `uint64_t`. */
uint64_t des_dec (uint64_t * ks /**< The pre-computed key schedule. */ ,
		  uint64_t val
		 /**< The 64 bits ciphertext. */
  );

/** A functional verification of the DES implementation. Runs a number of encipherments with `des_enc()` and the corresponding decipherments with `des_dec()` and checks the results against pre-computed plaintext, ciphertexts and secret keys. If compiled in `DEBUG` mode, prints warnings on mismatches or a **OK** message if the tests pass.
 * \returns One on success, zero on errors. */
int des_check (void);

#endif /** not DES_H */

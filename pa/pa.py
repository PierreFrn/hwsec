#! /usr/bin/env python3

#
# Copyright (C) Telecom Paris
#
# This file must be used under the terms of the CeCILL. This source
# file is licensed as described in the file COPYING, which you should
# have received as part of this distribution. The terms are also
# available at:
# http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt



import sys
import argparse

import des
import traces
import pcc

import multiprocessing as mp

# The P permutation table, as in the standard. The first entry (16) is the
# position of the first (leftmost) bit of the result in the input 32 bits word.
# Used to convert target bit index into SBox index (just for printed summary
# after attack completion).
p_table = [16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25]



def main ():
    # ************************************************************************
    # * Before doing anything else, check the correctness of the DES library *
    # ************************************************************************
    if not des.check ():
        sys.exit ("DES functional test failed")


    # *************************************
    # * Check arguments and read datafile *
    # *************************************
    argparser = argparse.ArgumentParser(description="Apply P. Kocher's DPA algorithm based on decision function")
    argparser.add_argument("datafile", metavar='FILE', help='name of the traces file in HWSec format. (e.g.  pa.hws)')
    argparser.add_argument("n", metavar='N', type=int, help='number of acquisitions to use')
    argparser.add_argument("target_bit", metavar='[B]', type=int, nargs='?', default=1, help='index of target bit in L15 (1 to 32, as in DES standard, default: 1)')
    args = argparser.parse_args()

    if args.n < 1:                                      # If invalid number of acquisitions.
        sys.exit ("Invalid number of acquisitions: %d (shall be greater than 1)" % args.n)

    if args.target_bit < 1 or args.target_bit > 32:     # If invalid target bit index.
        sys.exit ("Invalid target bit index: %d (shall be between 1 and 32 included)" % args.target_bit)

    # Compute index of corresponding SBox
    target_sbox = (p_table[args.target_bit - 1] - 1) / 4 + 1
    # Read power traces and ciphertexts. n is the number of acquisitions to use.
    ctx = read_datafile (args.datafile, args.n)


    # *****************************************************************************
    # * Compute and print average power trace. Store average trace in file        *
    # * "average.dat" and gnuplot command in file "average.cmd". In order to plot *
    # * the average power trace, type: $ gnuplot -persist average.cmd             *
    # *****************************************************************************
    average (ctx, "average")

    global n_acq
    n_acq = args.n

    global ctx_c
    ctx_c = ctx.c

    global focus_traces
    focus_traces = [trace[575:625] for trace in ctx.t] # Since the computation that interests us is at clock cycles 23 and 24, we keep only that part


    if n_acq >= 250:

        pool_big_number = mp.Pool(mp.cpu_count())
        weights = pool_big_number.map(process_keypart_weight_big_number, [sbox for sbox in range(8)])
        possible_key_parts = pool_big_number.map(process_keypart_PCC_big_number, weights)
        pool_big_number.close()

        key = 0
        for i in range(8):
            key += possible_key_parts[i][0][0]*(64**i)

        print("Final key: ", file=sys.stderr)
        print(hex(key))

    else:

        pool = mp.Pool(mp.cpu_count())
        weights = [pool.starmap(process_keypart_weight, [[sbox, i] for i in range(64)]) for sbox in range(4)]
        weights = [[keypart for sublist in weights_sub for keypart in sublist] for weights_sub in weights]
        possible_key_parts = pool.map(process_keypart_PCC, weights)

        ## Note that the threshold and step are not fixed values that the pcc has to be higher than but a value used to compare two PCCs from each other.
        threshold = 1 ##Because I consider that checking the two first key_parts possibilities is very important, unless the gap is really big.
        step = 0.95  ##I need a way to quickly reduce the weight of the next parts, so unless we're in a case where the PCCs are really near, they won't appear.
        #if n_acq > 90:
        #    step = 0.7
        possible_keys = []
        ## The step is a trade-off between complexity and the number of acquisitions needed, I had to tweak it a little bit since we have a limit of 1 hour on the pipeline.

        for sbox1 in range(4096):
            if ((sbox1 == 0) or (possible_key_parts[0][0][1]/possible_key_parts[0][sbox1][1] <= threshold + step**(sbox1-1))):
                for sbox2 in range(4096):
                    if ((sbox2 == 0) or (possible_key_parts[1][0][1]/possible_key_parts[1][sbox2][1] <= threshold + step**(sbox2-1))):
                        for sbox3 in range(4096):
                            if ((sbox3 == 0) or (possible_key_parts[2][0][1]/possible_key_parts[2][sbox3][1] <= threshold + step**(sbox3-1))):
                                for sbox4 in range(4096):
                                    if ((sbox4 == 0) or (possible_key_parts[3][0][1]/possible_key_parts[3][sbox4][1] <= threshold + step**(sbox4-1))):
                                        key = 0
                                        key += possible_key_parts[0][sbox1][0]*(4096**0)
                                        key += possible_key_parts[1][sbox2][0]*(4096**1)
                                        key += possible_key_parts[2][sbox3][0]*(4096**2)
                                        key += possible_key_parts[3][sbox4][0]*(4096**3)
                                        possible_keys.append(key)
                                    else:
                                        break
                            else:
                                break
                    else:
                        break
            else:
                break

        print("Number of keys to be tested : " + str(len(possible_keys)), file=sys.stderr)
        weights = pool.map(process_key_weight, possible_keys)
        keys_pcc = pool.starmap(process_key_pcc, [[possible_keys[i], weights[i]] for i in range(len(possible_keys))])
        keys_pcc = sorted(keys_pcc, key=lambda x:x[1], reverse=True)
        pool.close()


        print("Final key: ", file=sys.stderr)
        print(hex(keys_pcc[0][0]))

def process_keypart_weight_big_number(sbox):
    weights = [[] for i in range(64)]
    for k in range(64):
        for j in range(n_acq):
            r16l16 = des.ip(ctx_c[j])
            l16 = des.right_half(r16l16)
            l15 = des.left_half(r16l16) ^ des.p(des.sboxes(des.e(l16) ^ (k*(64**sbox))))
            weights[k].append(des.hamming_weight(des.n_p(l16 ^ l15) & (15*(16**sbox))))
    return weights

def process_keypart_PCC_big_number(weights):
    pcc_list = []
    for k in range(64):
        pcc_list.append([k, max(pcc.pcc(focus_traces, weights[k]))[0]])
    pcc_list = sorted(pcc_list, key=lambda x:x[1], reverse=True)

    return pcc_list


def process_keypart_weight(sbox, big_part):
    weights = [[] for i in range(64)]
    big_part_m = 64*big_part
    for k in range(64):
        for j in range(n_acq):
            r16l16 = des.ip(ctx_c[j])
            l16 = des.right_half(r16l16)
            l15 = des.left_half(r16l16) ^ des.p(des.sboxes(des.e(l16) ^ (big_part_m + k)*(4096**sbox)))
            weights[k].append(des.hamming_weight(des.n_p(l16 ^ l15) & (255*(256**sbox))))

    return weights

def process_keypart_PCC(weights):
    pcc_list = []
    for k in range(4096):
        pcc_list.append([k, max(pcc.pcc(focus_traces, weights[k]))[0]])
    pcc_list = sorted(pcc_list, key=lambda x:x[1], reverse=True)

    return pcc_list

def process_key_weight(key):
    weights = []
    for j in range(n_acq):
        r16l16 = des.ip(ctx_c[j])
        l16 = des.right_half(r16l16)
        l15 = des.left_half(r16l16) ^ des.p(des.sboxes(des.e(l16) ^ key))
        weights.append(des.hamming_weight(des.n_p(l16 ^ l15)))
    return weights

def process_key_pcc(key, weights):
    coeff = max(pcc.pcc(focus_traces, weights))[0]
    return [key, coeff]

# A function to allocate cipher texts and power traces, read the
# datafile and store its content in allocated context.
def read_datafile (name, n):
    ctx = traces.trContext (name, n)
    if ctx.n != n:
        sys.exit ("Could not read %d acquisitions from traces file. Traces file contains %d acquisitions." % (n, ctx.n));

    return ctx

# Compute the average power trace of the traces context ctx, print it in file
# <prefix>.dat and print the corresponding gnuplot command in <prefix>.cmd. In
# order to plot the average power trace, type: $ gnuplot -persist <prefix>.cmd
def average (ctx, prefix):
    acc = [sum(i) for i in zip(*ctx.t)]
    avg = [i/len(ctx.t) for i in acc]

    traces.plot (prefix, -1, [avg]);
    print("Average power trace stored in file '%s.dat'." % prefix, file=sys.stderr)

if __name__ == "__main__":
    main ()

# vim: set tabstop=8 softtabstop=4 shiftwidth=4 expandtab textwidth=0:

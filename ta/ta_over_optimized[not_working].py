#! /usr/bin/env python3

#
# Copyright (C) Telecom Paris
#
# This file must be used under the terms of the CeCILL. This source
# file is licensed as described in the file COPYING, which you should
# have received as part of this distribution. The terms are also
# available at:
# http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt
#

# THIS IS NOT A REAL TIMING ATTACK: it assumes that the last round key is
# 0x0123456789ab. Your goal is to retrieve the true last round key, instead.

import sys
import argparse

import des
import pcc

import multiprocessing as mp
# import km

def main():
    # ************************************************************************
    # * Before doing anything else, check the correctness of the DES library *
    # ************************************************************************
    if not des.check():
        sys.exit("DES functional test failed")

    # *************************************
    # * Check arguments and read datafile *
    # *************************************
    argparser = argparse.ArgumentParser(description="Apply P. Kocher's TA algorithm")
    argparser.add_argument("datafile", metavar='file',
                        help='name of the data file (generated with target)')
    argparser.add_argument("n", metavar='n', type=int,
                        help='number of experiments to use')
    args = argparser.parse_args()

    if args.n < 1:                                      # If invalid number of experiments.
        sys.exit("Invalid number of experiments: %d (shall be greater than 1)" % args.n)

    # Read encryption times and ciphertexts. n is the number of experiments to use.
    read_datafile(args.datafile, args.n)

    global n_acq

    n_acq = args.n

    pool = mp.Pool(mp.cpu_count())


    print("Processing keyparts weights", file=sys.stderr)

    weights = [pool.starmap(process_keypart_weight, [[sbox, i] for i in range(64)]) for sbox in range(4)]

    weights = [[keypart for sublist in weights_sub for keypart in sublist] for weights_sub in weights]

    print("Processing keyparts PCCs", file=sys.stderr)

    possible_key_parts = pool.map(process_keypart_PCC, weights)

    #possible_keyparts_file = open("possible_keyparts.txt", "w")
    #print("printing keyparts in possible_keys.txt")
    #for keypart in possible_key_parts[1]:
    #    print("Keypart :" + str(hex(keypart[0])) + " | PCC :" + str(keypart[1]) , file=possible_keyparts_file)


    #for sbox in range(8):
    #    for k in range(64):
    #        possible_key_parts[sbox][k] = [k,pcc.pcc(weights[sbox][k], t)[0][0]]
    #    possible_key_parts[sbox] = sorted(possible_key_parts[sbox], key=lambda x:x[1], reverse=True)


    #for i in range(8):
    #    print("Sbox : "+str(i), file=sys.stderr)
    #    for j in range(4): ## Range only to 4 because the rest is usually useless and it's only for debug.
    #        print(str(possible_key_parts[i][j*4]) + str(possible_key_parts[i][j*4+1]) + str(possible_key_parts[i][j*4+2]) + str(possible_key_parts[i][j*4+3]), file=sys.stderr)


    if (n_acq > 4000):
        key = 0
        key += possible_key_parts[0][0][0]*(4096**0)
        key += possible_key_parts[1][0][0]*(4096**1)
        key += possible_key_parts[2][0][0]*(4096**2)
        key += possible_key_parts[3][0][0]*(4096**3)

        print(hex(key))

    else:

        possible_keys = []

        ## Note that the threshold and step are not fixed values that the pcc has to be higher than but a value used to compare two PCCs from each other.
        threshold = 1 ##Because I consider that checking the two first key_parts possibilities is very important, unless the gap is really big.
        step = 0.85   ##I need a way to quickly reduce the weight of the next parts, so unless we're in a case where the PCCs are really near, they won't appear.

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


        #possible_keys_file = open("possible_keys.txt", "w")
        #print("printing keys in possible_keys.txt")
        #for key in possible_keys:
        #    print(hex(key), file=possible_keys_file)

        if (len(possible_keys) == 1):
            print(hex(possible_keys[0]))
        else:
            length = len(possible_keys)
            print("Number of keys to be tested : " + str(length), file=sys.stderr)

            print("Processing keys PCCs", file=sys.stderr)

            weights = pool.map(process_key_weight, possible_keys)


            pool.close()

            correct_key_index = 0
            best = 0
            for i in range(length):
                coeff = pcc.pcc(weights[i],t)[0][0]
                if coeff > best:
                    correct_key_index = i
                    best = coeff

            print(hex(possible_keys[correct_key_index]))

def process_keypart_weight(sbox, big_part):

    weights = [[] for i in range(64)]
    big_part_m = 64*big_part
    for j in range (64):
        for i in range(n_acq):
            weights[j].append(des.hamming_weight(des.sboxes(des.e(des.right_half(des.ip(ct[i]))) ^ (big_part_m + j)*(4096**sbox)) & (255*(256**sbox))))

    return weights

def process_keypart_PCC(weights):

    pcc_list = []
    for k in range(len(weights)):
        pcc_list.append([k,pcc.pcc(weights[k], t)[0][0]])
    pcc_list = sorted(pcc_list, key=lambda x:x[1], reverse=True)

    return pcc_list

def process_key_weight(key):

    k15 = des.pc2(des.rs(des.n_pc2(key)))
    """
    k14 = des.pc2(des.rs(des.rs(des.n_pc2(k15))))
    k13 = des.pc2(des.rs(des.rs(des.n_pc2(k14))))
    k12 = des.pc2(des.rs(des.rs(des.n_pc2(k13))))
    k11 = des.pc2(des.rs(des.rs(des.n_pc2(k12))))
    k10 = des.pc2(des.rs(des.rs(des.n_pc2(k11))))
    k9 = des.pc2(des.rs(des.rs(des.n_pc2(k10))))
    k8 = des.pc2(des.rs(des.n_pc2(k9)))
    k7 = des.pc2(des.rs(des.rs(des.n_pc2(k8))))
    k6 = des.pc2(des.rs(des.rs(des.n_pc2(k7))))
    k5 = des.pc2(des.rs(des.rs(des.n_pc2(k6))))
    k4 = des.pc2(des.rs(des.rs(des.n_pc2(k5))))
    k3 = des.pc2(des.rs(des.rs(des.n_pc2(k4))))
    k2 = des.pc2(des.rs(des.rs(des.n_pc2(k3))))
    k1 = des.pc2(des.rs(des.rs(des.n_pc2(k2))))
    """

    weights = []

    for i in range(n_acq):
        r16l16 = des.ip(ct[i])
        l16 = des.right_half(r16l16)
        sb_output = des.sboxes(des.e(l16) ^ key)
        weights.append(des.hamming_weight(sb_output))
        l15 = des.left_half(r16l16) ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l15) ^ k15)
        weights[i] += des.hamming_weight(sb_output)
        """
        l14 = l16 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l14) ^ k14)
        weights[i] += des.hamming_weight(sb_output)
        l13 = l15 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l13) ^ k13)
        weights[i] += des.hamming_weight(sb_output)
        l12 = l14 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l12) ^ k12)
        weights[i] += des.hamming_weight(sb_output)
        l11 = l13 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l11) ^ k11)
        weights[i] += des.hamming_weight(sb_output)
        l10 = l12 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l10) ^ k10)
        weights[i] += des.hamming_weight(sb_output)
        l9 = l11 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l9) ^ k9)
        weights[i] += des.hamming_weight(sb_output)
        l8 = l10 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l8) ^ k8)
        weights[i] += des.hamming_weight(sb_output)
        l7 = l9 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l7) ^ k7)
        weights[i] += des.hamming_weight(sb_output)
        l6 = l8 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l6) ^ k6)
        weights[i] += des.hamming_weight(sb_output)
        l5 = l7 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l5) ^ k5)
        weights[i] += des.hamming_weight(sb_output)
        l4 = l6 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l4) ^ k4)
        weights[i] += des.hamming_weight(sb_output)
        l3 = l5 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l3) ^ k3)
        weights[i] += des.hamming_weight(sb_output)
        l2 = l4 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l2) ^ k2)
        weights[i] += des.hamming_weight(sb_output)
        l1 = l3 ^ des.p(sb_output)
        sb_output = des.sboxes(des.e(l1) ^ k1)
        weights[i] += des.hamming_weight(sb_output)
        """

    return weights


# Open datafile <name> and store its content in global variables
# <ct> and <t>.
def read_datafile(name, n):
    global ct, t

    if not isinstance(n, int) or n < 0:
        raise ValueError('Invalid maximum number of traces: ' + str(n))

    try:
        f = open(str(name), 'rb')
    except IOError:
        raise ValueError("cannot open file " + name)
    else:
        try:
            ct = []
            t = []
            for _ in range(n):
                a, b = f.readline().split()
                ct.append(int(a, 16))
                t.append(float(b))
        except(EnvironmentError, ValueError):
            raise ValueError("cannot read cipher text and/or timing measurement")
        finally:
            f.close()

if __name__ == "__main__":
    main()

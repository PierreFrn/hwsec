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
    weights = pool.map(process_keypart_weight, [sbox for sbox in range(8)])
    possible_key_parts = pool.map(process_keypart_PCC, weights)

    ## Note that the threshold and step are not fixed values that the pcc has to be higher than but a value used to compare two PCCs from each other.
    threshold = 1 ##Because I consider that checking the two first key_parts possibilities is very important, unless the gap is really big.
    step = 0.8   ##I need a way to quickly reduce the weight of the next parts, so unless we're in a case where the PCCs are really near, they won't appear.
    possible_keys = []
    ## The step is a trade-off between complexity and the number of acquisitions needed, I had to tweak it a little bit since we have a limit of 1 hour on the pipeline.


    for sbox1 in range(64):
        if ((sbox1 == 0) or (possible_key_parts[0][0][1]/possible_key_parts[0][sbox1][1] <= threshold + step**(sbox1-1))):
            for sbox2 in range(64):
                if ((sbox2 == 0) or (possible_key_parts[1][0][1]/possible_key_parts[1][sbox2][1] <= threshold + step**(sbox2-1))):
                    for sbox3 in range(64):
                        if ((sbox3 == 0) or (possible_key_parts[2][0][1]/possible_key_parts[2][sbox3][1] <= threshold + step**(sbox3-1))):
                            for sbox4 in range(64):
                                if ((sbox4 == 0) or (possible_key_parts[3][0][1]/possible_key_parts[3][sbox4][1] <= threshold + step**(sbox4-1))):
                                    for sbox5 in range(64):
                                        if ((sbox5 == 0) or (possible_key_parts[4][0][1]/possible_key_parts[4][sbox5][1] <= threshold + step**(sbox5-1))):
                                            for sbox6 in range(64):
                                                if ((sbox6 == 0) or (possible_key_parts[5][0][1]/possible_key_parts[5][sbox6][1] <= threshold + step**(sbox6-1))):
                                                    for sbox7 in range(64):
                                                        if ((sbox7 == 0) or (possible_key_parts[6][0][1]/possible_key_parts[6][sbox7][1] <= threshold + step**(sbox7-1))):
                                                            for sbox8 in range(64):
                                                                if ((sbox8 == 0) or (possible_key_parts[7][0][1]/possible_key_parts[7][sbox8][1] <= threshold + step**(sbox8-1))):
                                                                    key = 0
                                                                    key += possible_key_parts[0][sbox1][0]*(64**0)
                                                                    key += possible_key_parts[1][sbox2][0]*(64**1)
                                                                    key += possible_key_parts[2][sbox3][0]*(64**2)
                                                                    key += possible_key_parts[3][sbox4][0]*(64**3)
                                                                    key += possible_key_parts[4][sbox5][0]*(64**4)
                                                                    key += possible_key_parts[5][sbox6][0]*(64**5)
                                                                    key += possible_key_parts[6][sbox7][0]*(64**6)
                                                                    key += possible_key_parts[7][sbox8][0]*(64**7)
                                                                    possible_keys.append(key)
                                                                else:
                                                                    break
                                                        else:
                                                            break
                                                else:
                                                    break
                                        else:
                                            break
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

    print(hex(keys_pcc[0][0]))

def process_keypart_weight(sbox):
    weights = [[] for i in range(64)]
    for j in range (64):
        for i in range(n_acq):
            weights[j].append(des.hamming_weight(des.sboxes(des.e(des.right_half(des.ip(ct[i]))) ^ j*(64**sbox)) & (15*(16**sbox))))

    return weights

def process_keypart_PCC(weights):
    pcc_list = []
    for k in range(64):
        pcc_list.append([k,pcc.pcc(weights[k], t)[0][0]])
    pcc_list = sorted(pcc_list, key=lambda x:x[1], reverse=True)

    return pcc_list

def process_key_weight(key):
    k15 = des.pc2(des.rs(des.n_pc2(key)))
    weights = []
    for i in range(n_acq):
        r16l16 = des.ip(ct[i])
        sb_output = des.sboxes(des.e(des.right_half(r16l16)) ^ key)
        weights.append(des.hamming_weight(sb_output))
        sb_output = des.sboxes(des.e(des.left_half(r16l16) ^ des.p(sb_output)) ^ k15)
        weights[i] += des.hamming_weight(sb_output)

    return weights

def process_key_pcc(key, weights):
    coeff = pcc.pcc(weights,t)[0][0]
    return [key, coeff]

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

<!-- MASTER-ONLY: DO NOT MODIFY THIS FILE

Copyright (C) Telecom Paris

This file must be used under the terms of the CeCILL. This source
file is licensed as described in the file COPYING, which you should
have received as part of this distribution. The terms are also
available at:
http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt

-->

Timing attack against a DES software implementation

----

- [General description](#general-description)
- [Some useful material](#some-useful-material)
- [Directions](#directions)
  * [Select your preferred programming language](#select-your-preferred-programming-language)
  * [Select the computer(s) you will use](#select-the-computer-s--you-will-use)
  * [Check your set-up](#check-your-set-up)
  * [Acquisition phase](#acquisition-phase)
  * [Attack phase](#attack-phase)
- [Automatic evaluation](#automatic-evaluation)
  * [Submitting your work](#submitting-your-work)
  * [The hall of fame](#the-hall-of-fame)
- [Also run the attack target and design countermeasures (bonus optional part)](#also-run-the-attack-target-and-design-countermeasures--bonus-optional-part-)
  * [Run the target](#run-the-target)
  * [Design countermeasures](#design-countermeasures)

----

**Important**: do not start the lab if you did not yet follow the [initial setup] instructions. And if you did, before starting the lab, merge the master branch in your personal branch in order to import the last updates (if any):

```bash
$ cd some-where/hwsec
$ git checkout John.Doe
$ git fetch
$ git merge origin/master
```

where `John.Doe` is the name of your personal branch.

# General description

In this lab, you will try to exploit a flaw in a DES software implementation which computation time depends on the input messages and on the secret key: its P permutation was implemented by a not-too-smart software designer who did not know anything about timing attacks (and not much about programming). The pseudo-code of his implementation of the P permutation is the following:

```
// Permutation table. Input bit #16 is output bit #1 and
// input bit #25 is output  bit #32.
p_table = {16,  7, 20, 21,
           29, 12, 28, 17,
            1, 15, 23, 26,
            5, 18, 31, 10,
            2,  8, 24, 14,
           32, 27,  3,  9,
           19, 13, 30,  6,
           22, 11,  4, 25};

p_permutation(val) {
  res = 0;                    // Initialize the 32 bits output result to all zeros
  for i in 1 to 32 {          // For all input bits #i (32 of them)
    if get_bit(i, val) == 1   // If input bit #i is set
      for j in 1 to 32        // For all 32 output bits #j (32 of them)
        if p_table[j] == i    // If output bits #j is input bit #i
          k = j;              // Remember output bit index
        endif
      endfor                  // output bit #k corresponds to input bit #i
      set_bit(k, res);        // Set bit #k of result
    endif
  endfor
  return res;                 // Return result
}
```

Do you understand why, apart from being very inefficient, this implementation is a nice target for a timing attack? What model of the computation time could be used by an attacker?

# Some useful material

* The [DES standard]
* [Timing Attacks on Implementations of Diffie-Hellman, RSA, DSS, and Other Systems (Paul Kocher, CRYPTO'96)]
* The [introduction lecture]
* The [lecture on side channel attacks]
* For the C language version:
    * [The **des** library, dedicated to the Data Encryption Standard (DES)][DES C library]
    * [The **pcc** library, dedicated to the computation of Pearson Correlation Coefficients (PCC)][pcc C library]
* For the python language version:
    * [The **des** module, dedicated to the Data Encryption Standard (DES)][DES python module]
    * [The **pcc** module, dedicated to the computation of Pearson Correlation Coefficients (PCC)][pcc module library]

# Directions

**Important**: the only files that you are asked to modify during the lab are:
* `ta.c`: a C source file that you will use as a starting point for your own attack if you chose the C language.
* `ta.py`: a python source file that you will use as a starting point for your own attack if you chose the python language.
* `p.c`: a C source file that defines the P permutation and that you will edit to fix the flaw (no python version, sorry).
* `language`: a file that you will add to select your preferred programming language (see below).

Any modification to other files or any new file that you could add will be ignored by the automated evaluation.

## Select your preferred programming language

The automatic evaluation system needs to know which programming language (C or python) you decided to use. To indicate this, simply add a file named `language` to the `ta` subdirectory containing either `c` or `py` and `git add-commit-push`. Example if you prefer python:

```bash
$ cd some-where/hwsec/ta
$ echo "py" > language
$ git add language
$ git commit -m 'Add language file'
$ git push
```

The evaluation will fail if this language file is not found or if it contains something else than `c` or `py`. If you started working with one language but decide to change, simply change the content of the language file:

```bash
$ cd some-where/hwsec/ta
$ echo "c" > language
$ git add language
$ git commit -m 'Change language file'
$ git push
```

## Select the computer(s) you will use

During this lab you will develop an attacking program and use it against the target DES implementation. To run and test it you can use your personal computer or one of the EURECOM's computers.

Your attack program should run without any problem on the GNU/Linux desktop computers in EURECOM's rooms 52 and 53. To avoid conflicts, if you decide to work remotely through ssh on one of these computers, please use the computer that has been allocated to you in the following list:

| LASTNAME Firstname | Desktop   | LASTNAME Firstname     | Desktop   |
| :----------------- | :------   | :-----------------     | :------   |
| BEGUM Zuha         | eurecom1  | KUMAR Yash             | eurecom19 |
| BIN MOHD ISA Mohd  | eurecom2  | LE BRISHOUAL Thomas    | eurecom20 |
| BORI Demetrio      | eurecom3  | LEBRIN Clément         | eurecom21 |
| CARPENTIER Tom     | eurecom4  | LEONET Clara           | eurecom22 |
| COUPIN Eloïse      | eurecom5  | MAZAUD Samuel          | eurecom23 |
| EIDE Henrik        | eurecom6  | MITHASSEL Benedikt     | eurecom24 |
| FERRER Rémi        | eurecom7  | PORCHERON LUCAS Agathe | eurecom25 |
| FISCHER Janika     | eurecom8  | PUAUX Charles          | eurecom26 |
| FOURNIER Pierre    | eurecom9  | ROLFO Edoardo          | eurecom27 |
| GALVANI Isaiah     | eurecom10 | RULH Basile            | eurecom28 |
| GEORGET Lucas      | eurecom11 | SINGH Verneet          | eurecom29 |
| GIANELLA Antonin   | eurecom12 | SOURSOU Lucas          | eurecom20 |
| HENIA Hassib       | eurecom13 | SUHAIMI Khairul        | eurecom31 |
| HERNANDEZ Aurélien | eurecom14 | THOMAS Alexandre       | eurecom32 |
| HUILLET Thibault   | eurecom15 | VENDITTI Pietro        | eurecom33 |
| JACQUIER Emilie    | eurecom16 | VENKATRAMAN Rajiv      | eurecom34 |
| JOHARI Muhamad     | eurecom17 | VERGARI Cosma          | eurecom35 |
| KATURI Vikram      | eurecom18 | VIDAL Gauthier         | eurecom36 |

To code your attack program you can use the C or the python3 languages, as you wish. The C and python versions have been tested under MacOS and several GNU/Linux distributions. The python version should work if python3 is installed. The C version should work if GNU make and gcc (or clang with a gcc wrapper) are installed. No tests have been made under Windows but if you know how to program in python3 or in C under Windows it should not be too difficult to adapt the lab for this OS. If you do not want or cannot run the attack program on your own computer you must run it on your remote desktop at EURECOM, there is no other solution.

## Check your set-up

Before working on the lab please check that the lab is compatible with your set-up. If your preferred language is python simply run the provided example python attack program and compare the output with the expected one:

```bash
$ cd some-where/hwsec/ta
$ python3 ta.py ta.dat.example 1000
Hamming weight: 2
Average timing: 166254.953700
Last round key (hex):
0x0123456789AB
```

If you prefer the C language first compile the provided example C attack program, run it and compare the output with the expected one:

```bash
$ cd some-where/hwsec/ta
$ make ta
gcc -Wall -c -I. ta.c -o ta.o
gcc -Wall -c -O3 -I. des.c -o des.o
gcc -Wall -c -O3 -I. utils.c -o utils.o
gcc -Wall -c -I. pcc.c -o pcc.o
gcc  ta.o des.o utils.o pcc.o -o ta -lm
$ ./ta ta.dat.example 1000
Hamming weight: 2
Average timing: 166254.953700
Last round key (hex):
0x0123456789AB
```

If something goes wrong please try to install the missing software packages or select another computer. Do not hesitate to signal any problem that you cannot solve alone.

**Optional**: if you also want to run the C (only) software DES implementation that we try to attack (you do not have to), test if it compiles and runs on your computer:

```bash
$ cd some-where/hwsec/ta
$ make target
gcc -Wall -c -O3 -I. target.c -o target.o
gcc -Wall -c -O3 -I. des.c -o des.o
gcc -Wall -c -O3 -I. rdtsc_timer.c -o rdtsc_timer.o
gcc -Wall -c -O3 -I. utils.c -o utils.o
gcc -Wall -c -O0 -I. p.c -o p.o
gcc  target.o des.o rdtsc_timer.o utils.o p.o -o target -lm
$ ./target 100
100%
Acquisitions stored in: ta.dat
Secret key stored in:  ta.key
Last round key (hex):
0x51c9343d2632
```

This should have encrypted 100 random plaintexts and stored the results in two text files: `ta.dat` and `ta.key`. If it does not work on your computer, don't worry, example result files are provided (see the next section).

## Acquisition phase

An acquisition phase has already been done for you: the target DES software implementation has been compiled and run 100000 times on a GNU/Linux computer. Each time a random plaintext has been encrypted and the encryption has been accurately timed using the hardware timer of the computer. The results are provided in 2 text files:

* `ta.key.example` contains the 64-bits DES secret key, its 56-bits version (without the parity bits), the 16 corresponding 48-bits round keys and, for each round key, the eight 6-bits subkeys.
* `ta.dat.example` contains the 100000 ciphertexts and timing measurements.

Let us look at some lines of `ta.dat.example`:

```
$ head -5 ta.dat.example
0xd5f0520500cab987 102306.500000
0xd889a9a094534d82 99408.400000
0xf31644a4d90c4c9a 95737.300000
0xc9bf4758502f3ce9 93619.800000
0xd4579bf9c029d58e 96940.000000
...
```

Each line is an acquisition corresponding to one of the 100000 random plaintexts. The first field on the line is the 64 bits ciphertext returned by the DES engine, in hexadecimal form. With the numbering convention of the DES standard, the leftmost character (d in the first acquisition of the above example) corresponds to bits 1 to 4. The following one (5) corresponds to bits 5 to 8 and so on until the rightmost (7) which corresponds to bits 61 to 64. In the first acquisition of the above example, bit number 6 is set while bit number 7 is unset. Please check your understanding of this numbering convention, if you miss something here, there are very little chances that you complete the lab. The second field is the timing measurement.

## Attack phase

It is now time to design a timing attack. You will start from a provided example application (`ta.c` or `ta.py`). It shows how to use the most useful features of the provided software libraries. It is not a real timing attack. In particular, it assumes that the last round key is `0x0123456789ab`, instead of trying to recover it from the acquisitions. Your job is thus to turn it into a real timing attack and to retrieve the real last round key. Of course, you can easily find out the last round key by looking at the `ta.key.example` file, but in real life things would not be so easy. So, use the `ta.key.example` file for verification only. The provided example application:
* takes two arguments: the name of a data file and a number of acquisitions to use,
* reads the specified number of acquisitions from the data file,
* stores the ciphertexts and timing measurements in two arrays named `ct` and `t`,
* assumes that the last round key is `0x0123456789ab`, and based on this, computes the 4-bits output of SBox #1 in the last round of the last acquisition, and prints the Hamming weight of this output,
* computes and prints the average value of the timing measurements of all acquisitions and, finally,
* prints the assumed `0x0123456789ab` last round key in hexadecimal format.

All printed messages are sent to the standard error (`stderr`). The only message that is sent to the standard output (`stdout`) is the 48-bits last round key, in hexadecimal form.

The example application uses some functions of the provided software libraries. To see the complete list of what these libraries offer, look at their documentation (see the [Some useful material](#some-useful-material) section).

As is, this application is not very useful, so, edit the `ta.c` (`ta.py`) file and transform it into a successful timing attack to recover the 48-bits last round key.

**Important note**: whatever the changes you make, preserve the following **interface specification**:
* your program takes exactly two arguments: the name of a data file and a number of acquisitions to use,
* your program can output anything on the standard error (`stderr`), use it freely for debugging,
* your program must output only the last round key on the standard output (`stdout`), in hexadecimal format, with 12 hexits and preceded by `0x`, the hexadecimal format indicator (e.g. `0x0123456789ab`).

Imagine what your attack will be, what model of timing you will be using, what statistical tools and the general algorithm. Code your ideas and as soon as you think it should work, save the file and run again:

```bash
$ make ta
$ ./ta ta.dat.example 100000
Hamming weight: 1
Average timing: 169490.540600
Last round key (hex):
0xaa0acc89efd5
```

or:

```bash
$ python3 ta.py ta.dat.example 100000
Hamming weight: 1
Average timing: 169490.540600
Last round key (hex):
0xaa0acc89efd5
```

If the printed 48-bits last round key is the same as in `ta.key.example`, your attack works.

Once you successfully recovered the last round key, try to reduce the number of acquisitions you are using: the less acquisitions the more practical your attack.

# Automatic evaluation

## Submitting your work

When you will be satisfied with your attack program, check once more its compliance with the interface specifications (see above). If it does add-commit-push:

```bash
$ git add REPORT.md
$ git add ta.c                       # C version
$ git add ta.py                      # python version
$ git commit -m 'My timing attack'
$ git push
```

The daemon responsible for the automatic evaluation will run until the written exam. You can configure your [GitLab account](https://gitlab.eurecom.fr/profile/notifications) to automatically receive an e-mail with your results for each new version you submit. Else you can visit the [GitLab pipelines page](https://gitlab.eurecom.fr/renaud.pacalet/hwsec/-/pipelines), search the entry corresponding to your last submission and click on the icon in the _Stages_ column. Be patient, if the server is heavily loaded, its response time can be significant. Note that there are time and memory limitations: attacks taking too long or consuming too much memory will be rejected. See the explanations in the [Hall of Fame].

Good luck.

## The hall of fame

Will your attack be the best all attacks, this year? Will it be the best of all times? Check the [Hall of Fame].

# Also run the attack target and design countermeasures (bonus optional part)

## Run the target

The target DES software implementation is provided as a C program (`target.c`) that uses the `RDTSC` Intel and AMD specific instruction to measure the time taken by the DES encryptions. If the microprocessor of your computer does not support the `RDTSC` instruction you cannot run the target on your computer. If you do not know how to compile and run a C program you cannot run the target on your computer. If you do not want or cannot run the target program on your own computer you can run it on your remote desktop at EURECOM.

Compile the target and run your own the acquisition phase:

```bash
$ make target
gcc -Wall -c -O3 -I. target.c -o target.o
gcc -Wall -c -O3 -I. des.c -o des.o
gcc -Wall -c -O3 -I. rdtsc_timer.c -o rdtsc_timer.o
gcc -Wall -c -O3 -I. utils.c -o utils.o
gcc -Wall -c -O0 -I. p.c -o p.o
gcc  target.o des.o rdtsc_timer.o utils.o p.o -o target -lm
$ ./target 100000
100%
Experiments stored in: ta.dat
Secret key stored in:  ta.key
Last round key (hex):
0x79629dac3cf0
```

Note: the 48-bits last round key is printed on the standard output (`stdout`), all other printed messages are sent to the standard error (`stderr`).

Note: you can also chose the secret key with:

```bash
$ ./target 100000 0x0123456789abcdef
```

where `0x0123456789abcdef` is the 64-bits DES secret key you want to use, in hexadecimal form.

Try now to use your attack to recover the last round key and compare with the expected one (`0xaa0acc89efd5` in our example):

```bash
$ make ta
$ ./ta ta.dat 100000
Hamming weight: 1
Average timing: 169490.540600
Last round key (hex):
0x79629dac3cf0
```

or:

```bash
$ python3 ta.py ta.dat 100000
Hamming weight: 1
Average timing: 169490.540600
Last round key (hex):
0x79629dac3cf0
```

If it matches your attack really works. How many acquisitions, minimum, do you need?

## Design countermeasures

Last, but not least, design a countermeasure by rewriting the P permutation function. The `p.c` file contains the C (no python version, sorry) source code of the function. Edit it and fix the flaw, save the file and compile the new version of the `target` application:

```bash
$ make target
...
```

This will compile the acquisition application with your implementation of the P permutation function. Fix the errors if any.

Run again the acquisition phase:

```bash
$ ./target 100000
...
```

This will first check the functional correctness of the modified DES implementation. Fix the errors if any until the application runs fine and creates a new `ta.dat` file containing 100000 acquisitions. Try to attack with these acquisitions and see whether it still works... Do you think your implementation is really protected against timing attacks? Explain. If you are not convinced that your implementation is 100% safe, explain what we could do to improve it.

[initial setup]: https://gitlab.eurecom.fr/renaud.pacalet/hwsec#gitlab-and-git-set-up
[DES standard]: /doc/data/des.pdf
[Timing Attacks on Implementations of Diffie-Hellman, RSA, DSS, and Other Systems (Paul Kocher, CRYPTO'96)]: http://www.cryptography.com/resources/whitepapers/TimingAttacks.pdf
[EURECOM Gitlab]: https://gitlab.eurecom.fr/
[hwsec project]: https://gitlab.eurecom.fr/renaud.pacalet/hwsec
[introduction lecture]: https://perso.telecom-paris.fr/pacalet/HWSec/lectures/introduction/main.pdf
[lecture on side channel attacks]: https://perso.telecom-paris.fr/pacalet/HWSec/lectures/side_channels/main.pdf
[DES C library]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/ta/c/des_8h.html
[pcc C library]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/ta/c/pcc_8h.html
[DES python module]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/ta/py/des.html
[pcc python module]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/ta/py/pcc.html
[Hall of Fame]: https://labsoc.eurecom.fr/hwsec/hf_ta.html

<!-- vim: set tabstop=4 softtabstop=4 shiftwidth=4 noexpandtab textwidth=0: -->

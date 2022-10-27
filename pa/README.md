<!-- MASTER-ONLY: DO NOT MODIFY THIS FILE

Copyright (C) Telecom Paris

This file must be used under the terms of the CeCILL. This source
file is licensed as described in the file COPYING, which you should
have received as part of this distribution. The terms are also
available at:
http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt

-->

Differential power analysis of a DES crypto-processor

----

- [General description](#general-description)
- [Some useful material](#some-useful-material)
- [Directions](#directions)
  * [Select your preferred programming language](#select-your-preferred-programming-language)
  * [Select the computer(s) you will use](#select-the-computer-s--you-will-use)
  * [Check your set-up](#check-your-set-up)
  * [Acquisitions phase](#acquisitions-phase)
  * [Attack phase](#attack-phase)
- [Automatic evaluation](#automatic-evaluation)
  * [Submitting your work](#submitting-your-work)
  * [The hall of fame](#the-hall-of-fame)

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

In this lab you will try to retrieve a DES last round key from a set of power traces. The target implementation is the hardware DES crypto-processor which architecture is depicted in the following figure:

![DES architecture]

As can be seen from the DES architecture a full encryption takes 32 cycles (in our case the secret key is input once and then never changes):

* 8 cycles to input the message to process, one byte at a time, in the IO register
* 16 cycles to compute the 16 rounds of DES, one round per cycle,
* 8 cycles to output the result, one byte at a time, from the IO register.

> Note: the DES engine could also run faster, thanks to its IO register, by parallelizing the input/output and the processing. The IO register can be loaded in parallel, 64 bits at a time or serially, one byte at a time. A throughput of one DES processing per 16 cycles could then be reached.

The DES engine runs at 32 MHz, delivering a processing power of up to 2 millions of DES encryptions per second (only one million of DES encryptions per second for this lab where I/O and processing are not parallelized).

Using this architecture, 10000 different 64 bits messages were encrypted with the same known secret key. During the encryptions the power traces were recorded by sampling the voltage drop across a small resistor inserted between the power supply and the crypto-processor. The input plain texts and the produced cipher texts were also recorded. Each power trace has been recorded 64 times and averaged in order to increase the voltage resolution. The sampling frequency of the digital oscilloscope was 20 Gs/s, but the power traces have been down-sampled by a factor of 25. Despite this quality loss it is indeed still perfectly feasible to recover the secret key. And because the traces only contain 800 points each (32 clock periods times 25 points per clock period), your attacks should run much faster than with the original time resolution (20000 points per power trace). Will you succeed in retrieving the secret key? How many power traces will you use?

# Some useful material

* The [DES standard]
* [Differential Power Analysis (Paul Kocher, Joshua Jaffe, and Benjamin Jun)]
* The [introduction lecture]
* The [lecture on side channel attacks]
* For the C language version:
    * [The **des** library, dedicated to the Data Encryption Standard (DES)][DES C library]
    * [The **traces** library, dedicated to power traces manipulations][traces C library]
    * [The **pcc** library, dedicated to the computation of Pearson Correlation Coefficients (PCC) between power traces and scalar random variables][pcc C library]
* For the python language version:
    * [The **des** module, dedicated to the Data Encryption Standard (DES)][DES python module]
    * [The **traces** module, dedicated to power traces manipulations][traces python module]
    * [The **pcc** module, dedicated to the computation of Pearson Correlation Coefficients (PCC) between power traces and scalar random variables][pcc python module]

# Directions

**Important**: the only files that you are asked to modify during the lab are:
* `pa.c`: a C source file that you will use as a starting point for your own attack if you chose the C language.
* `pa.py`: a python source file that you will use as a starting point for your own attack if you chose the python language.
* `language`: a file that you will add to select your preferred programming language (see below).

Any modification to other files or any new file that you could add will be ignored by the automated evaluation.

## Select your preferred programming language

The automatic evaluation system needs to know which programming language (C or python) you decided to use. To indicate this, simply add a file named `language` to the `ta` subdirectory containing either `c` or `py` and `git add-commit-push`. Example if you prefer python:

```bash
$ cd some-where/hwsec/pa
$ echo "py" > language
$ git add language
$ git commit -m 'Add language file'
$ git push
```

The evaluation will fail if this language file is not found or if it contains something else than `c` or `py`. If you started working with one language but decide to change, simply change the content of the language file:

```bash
$ cd some-where/hwsec/pa
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
$ cd some-where/hwsec/pa
$ python3 pa.py pa.hws 10 1
Average power trace stored in file 'average.dat'.
DPA traces stored in file 'dpa.dat'.
Target bit: 1
Target SBox: 4
Best guess: 52 (0x34)
Maximum of DPA trace: 2.367060e-01
Index of maximum in DPA trace: 730
Last round key (hex):
0x0123456789AB
```

If you prefer the C language first compile the provided example C attack program, run it and compare the output with the expected one:

```bash
$ cd some-where/hwsec/pa
$ make pa
gcc -Wall -c -O3 -I. pa.c -o pa.o
gcc -Wall -c -O3 -I. des.c -o des.o
gcc -Wall -c -O3 -I. utils.c -o utils.o
gcc -Wall -c -O3 -I. traces.c -o traces.o
gcc -Wall -c -O3 -I. pcc.c -o pcc.o
gcc  pa.o des.o utils.o traces.o pcc.o -o pa -lm
$ ./pa pa.hws 10 1
Average power trace stored in file 'average.dat'.
DPA traces stored in file 'dpa.dat'.
Target bit: 1
Target SBox: 4
Best guess: 52 (0x34)
Maximum of DPA trace: 2.367060e-01
Index of maximum in DPA trace: 730
Last round key (hex):
0x0123456789AB
```

If something goes wrong please try to install the missing software packages or select another computer. Do not hesitate to signal any problem that you cannot solve alone.

To generate the graphical representations of traces you will need the [gnuplot] utility. It is already installed on EURECOM's desktop computers. You can also install it on your personal computer, it is supported under GNU/Linux, Macos and Windows. Example of use for the DPA traces:

```bash
$ gnuplot dpa.cmd
```

The resulting graphics is stored in PNG format in `dpa.png`. Display it with your favourite image viewer (replace open by any image viewer you use):

```bash
$ open dpa.png
```

## Acquisitions phase

The power traces, plaintexts, ciphertexts and secret key are available in the binary `pa.hws` file. In the following we use the term _acquisition_ to designate a record in this file. The file contains some global parameters: number of acquisitions in the file (10000), number of samples per power trace (800), 64 bits secret key and 10000 acquisitions, each made of a 64 bits plain text, the corresponding 64 bits cipher text and a power trace. Power traces are 800 samples long and span over the 32 clock periods (25 samples per clock period) of a DES operation. The following figure represents such a power trace with the time as horizontal axis and the power (instantaneous voltage) as vertical axis:

![A power trace]

Software routines are provided to read this binary file.

The `pa.key` text file contains the 64-bits DES secret key, its 56-bits version (without the parity bits), the 16 corresponding 48-bits round keys and, for each round key, the eight 6-bits sub-keys. It also contains some information about the power traces.

## Attack phase

To design your power attack you will start from a provided example application (`pa.c` or `pa.py`). It shows how to use the most useful features of the provided software libraries. It is not a real power attack. In particular, it assumes that the last round key is `0x0123456789ab`, instead of trying to recover it from the power traces. Your job is thus to turn it into a real power attack and to retrieve the real last round key. Of course, you can easily find out the last round key by looking at the `pa.key` file, but in real life things would not be so easy. So, use the `pa.key` file for verification only. Open `pa.c` (`pa.py`) with your favorite editor and read it carefully. This example program takes 2 command line parameters: the name of a file containing acquisitions and a number of acquisitions to use. The acquisitions we will be using during this lab are stored in `pa.hws`. So, when running the program, you will provide this name as first parameter. The number of acquisitions to use (second parameter) must be between 1 and 10000 because the acquisitions file contains 10000 acquisitions only.

When run with these 2 parameters the program will:
* First checks the DES software library for correctness.
* Read the 2 command line parameters.
* Load the specified number of acquisitions in a dedicated data structure (`ctx`).
* Compute the average power trace, store it in a data file named `average.dat` and also create a command file named `average.cmd` for the `gnuplot` utility (we will see later what `gnuplot` can do).
* Attack the given number of acquisitions with the scenario described in P. Kocher's paper on DPA: assuming we know 6 bits of the 48 bits last round key (64 different possibilities), we can use the cipher text of an acquisition to compute the value of one bit of `L15`. By default (but this can be changed), the target bit is the leftmost bit in `L15` (numbered 1 according DES standard numbering convention). For each of the 64 candidate sub-keys we can do the same, leading to 64 values of the target bit. Let `d[i,g]` (scalar) be the computed value of the target bit for acquisition `i` and candidate sub-key value `g`. Let `T[i]` (vector) be the power trace of acquisition `i`. The algorithm of the attack is the following:

```
for g in 0 to 63 { // For all guesses on the 6-bits sub-key
  T0[g] <- {0,0,...,0}; // Initialize the zero-set trace
  T1[g] <- {0,0,...,0}; // Initialize the one-set trace
} // End for all guesses
for i in 0 to n-1 {  // For all acquisitions
  for g in 0 to 63 { // For all guesses on the 6-bits sub-key
    compute d[i,g]   // Compute target bit
    if d[i,g] = 0 {  // If target bit is zero for this guess
      T0[g] <- T0[g] + T[i]; // Accumulate power trace to zero-set
    }
    else { // Else, if target bit is one for this guess
      T1[g] <- T1[g] + T[i]; // Accumulate power trace to one-set
    }
  } // End for all guesses
} // End for all acquisitions
for g in 0 to 63 { // For all guesses on the 6-bits sub-key
  DPA[g] <- average (T1[g]) - average (T0[g]); // Compute DPA trace
  max[g] <- maximum (DPA[g]); // Search maximum of DPA trace
} // End for all guesses
best_guess <- argmax (max);  // Index of largest value in max array
return best_guess;
```

* The program stores the 64 computed DPA traces into a data file named `dpa.dat` and create a `gnuplot` command file named `dpa.cmd`. It also prints a summary indicating the index of the target bit, the index of the corresponding SBox (also index of the corresponding 6-bits sub-key), the best guess for the 6-bits sub-key, the amplitude of the highest peak in all DPA traces, the index of this maximum in the trace (that is, the time of the event that caused this peak).
* Finally, the program prints the `0x0123456789ab` last round key. It then frees the allocated memory and exits.

All printed messages are sent to the standard error (`stderr`) or one of the output files for `gnuplot`. The only message that is sent to the standard output (`stdout`) is the 48-bits last round key, in hexadecimal form.

Run the example program on the whole set of acquisitions (be patient, especially with python, it's a lot of data to process):

```bash
$ ./pa pa.hws 10000 # or python3 pa.py pa.hws 10000
Average power trace stored in file 'average.dat'.
DPA traces stored in file 'dpa.dat'.
Target bit: 1
Target SBox: 4
Best guess: 54 (0x36)
Maximum of DPA trace: 6.541252e-03
Index of maximum in DPA trace: 105
Last round key (hex):
0x0123456789ab
```

Then, have a look at the generated average power trace:

```bash
$ gnuplot average.cmd
$ open average.png
```

What do you think of this average trace? How many clock periods can you see? Can you identify the beginning and the end of the DES encipherment? Now, have a look at the summary the program printed. To which SBox does the target bit correspond? Is it correct? Note the found best guess and the height of the DPA peak. What do you think of this height, compared to the average power trace? Note also the index of this maximum. What do you think of it?

Let us now look at the DPA traces:

```bash
$ gnuplot dpa.cmd
$ open dpa.png
```

The red trace is the one with the highest peak, the one corresponding to the found best guess. All the other 63 DPA traces are plotted in blue. What do you think of this? Do you think we correctly guessed 6 bits of the last round key? Why?

Try to understand this example program and play with it. Note that it optionally takes a third parameter to specify the index of the target bit in `L15` (1 to 32, as in the DES standard, default: 1). So, running:

```bash
$ ./pa pa.hws 10000 # or python3 pa.py pa.hws 10000
```

is just like running:

```bash
$ ./pa pa.hws 10000 1 # or python3 pa.py pa.hws 10000 1
```

Here is an example of what you could try to do with this third optional parameter: use the provided DPA program example 32 times on the 32 different target bits. Have a look at some DPA traces with `gnuplot`, if you wish. For each of the 32 acquisitions fill a line in the [provided table on Framacalc][provided table (Framacalc)]<!-- --> (if you prefer you can also download a [PDF version][provided table (PDF)]). Note: the line corresponding to the example above is already filled.

Remark: because it takes some time to run all these 32 experiments (especially in python), join your efforts with others. Then, analyze your results: for the same group of 6 bits you should have 4 best guesses and they should be identical. Is it the case? Look at the maximum amplitudes and at the maximum positions. What do you think of this attack? Is it a Hamming distance or a Hamming weight one?

As is, the example application is not a complete, working attack, so, edit the `pa.c` (`pa.py`) file and transform it into a successful power attack to recover the 48-bits last round key.

**Important note**: whatever the changes you make, preserve the following **interface specification**:
* your program takes two arguments (the third optional parameter is not used during automatic evaluation): the name of a data file and a number of acquisitions to use,
* your program can output anything on the standard error (`stderr`), use it freely for debugging,
* your program must output only the last round key on the standard output (`stdout`), in hexadecimal format, with 12 hexits and preceded by `0x`, the hexadecimal format indicator (e.g. `0x0123456789ab`).

Imagine what your attack will be, what power model you will be using, what statistical tools and the general algorithm. Code your ideas and as soon as you think it should work, save the file and run again:

```bash
$ make pa
$ ./pa pa.hws 10000
0xaa0acc89efd5
```

or:

```bash
$ python3 pa.py pa.hws 10000
0xaa0acc89efd5
```

If the printed 48-bits last round key is the same as in `pa.key`, your attack works.

Once you successfully recovered the last round key, try to reduce the number of acquisitions you are using: the less acquisitions the more practical your attack.

# Automatic evaluation

## Submitting your work

When you will be satisfied with your attack program, check once more its compliance with the interface specifications (see above). If it does add-commit-push:

```bash
$ git add REPORT.md
$ git add pa.c                       # C version
$ git add pa.py                      # python version
$ git commit -m 'My power attack'
$ git push
```

The daemon responsible for the automatic evaluation will run until the written exam. You can configure your [GitLab account](https://gitlab.eurecom.fr/profile/notifications) to automatically receive an e-mail with your results for each new version you submit. Else you can visit the [GitLab pipelines page](https://gitlab.eurecom.fr/renaud.pacalet/hwsec/-/pipelines), search the entry corresponding to your last submission and click on the icon in the _Stages_ column. Be patient, if the server is heavily loaded, its response time can be significant. Note that there are time and memory limitations: attacks taking too long or consuming too much memory will be rejected. See the explanations in the [Hall of Fame].

Good luck.

## The hall of fame

Will your attack be the best all attacks, this year? Will it be the best of all times? Check the [Hall of Fame].

[initial setup]: https://gitlab.eurecom.fr/renaud.pacalet/hwsec#gitlab-and-git-set-up
[DES standard]: /doc/data/des.pdf
[A power trace]: /doc/data/trace.png
[provided table (PDF)]: /doc/data/des_pa_table.pdf
[provided table (Framacalc)]: https://lite.framacalc.org/9ns9-hwsec
[DES architecture]: /doc/data/des_architecture.png
[Differential Power Analysis (Paul Kocher, Joshua Jaffe, and Benjamin Jun)]: https://42xtjqm0qj0382ac91ye9exr-wpengine.netdna-ssl.com/wp-content/uploads/2015/08/DPA.pdf
[introduction lecture]: https://perso.telecom-paris.fr/pacalet/HWSec/lectures/introduction/main.pdf
[lecture on side channel attacks]: https://perso.telecom-paris.fr/pacalet/HWSec/lectures/side_channels/main.pdf
[DES C library]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/pa/c/des_8h.html
[traces C library]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/pa/c/traces_8h.html
[pcc C library]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/pa/c/pcc_8h.html
[DES python module]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/pa/py/des.html
[traces python module]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/pa/py/traces.html
[pcc python module]: https://perso.telecom-paris.fr/pacalet/HWSec/doc/pa/py/pcc.html
[Hall of Fame]: https://labsoc.eurecom.fr/hwsec/hf_pa.html
[gnuplot]: http://www.gnuplot.info/

<!-- vim: set tabstop=4 softtabstop=4 shiftwidth=4 noexpandtab textwidth=0: -->

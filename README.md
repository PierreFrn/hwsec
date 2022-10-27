<!-- MASTER-ONLY: DO NOT MODIFY THIS FILE

Copyright (C) Telecom Paris
Copyright (C) Renaud Pacalet (renaud.pacalet@telecom-paris.fr)

This file must be used under the terms of the CeCILL. This source
file is licensed as described in the file COPYING, which you should
have received as part of this distribution. The terms are also
available at:
https://cecill.info/licences/Licence_CeCILL_V2.1-en.html
-->

Hardware Security labs

---

- [Recommendations](#recommendations)
  * [Keep the git repository clean](#keep-the-git-repository-clean)
  * [DES notations](#des-notations)
  * [Programming languages](#programming-languages)
  * [Miscellaneous](#miscellaneous)
- [GitLab and git set-up](#gitlab-and-git-set-up)


---

# Recommendations

In order to attend the labs and get the full benefit of them you should:

* Be reasonably comfortable with a personal computer under GNU/Linux.
There are some good books available and a lot of on-line resources (manuals, tutorials, etc).
[bootlin], for instance, has a very useful one-page [memento of the most useful GNU/Linux commands]; there are even French, German and Italian versions.
Local copies can be found in the `/doc/data` directory.
Attending the _Software Development (SoftDev)_ EURECOM course or having a look at its companion material is also a very good option.
* Be able to use at least one of the text editors that can be found under GNU/Linux (`emacs`, `vim`, `nano`, `gedit`, `atom`, `sublime text`...)
* Have some knowledge of algorithm principles and basic programming skills either in C or Python.

## Keep the git repository clean

Remember that we all share the same git repository, reason why it is important to keep it reasonably clean.
To avoid a too fast grow of the size of the repository, please:

* Avoid adding full directories; it is sometimes convenient but also the best way to add a large number of large generated files that we do not want in the repository; try to `git add` only individual files, and only files that make sense (source files, reports in Mardown format, carefully selected images used in reports...).
* Try to use the right resolution for the (carefully selected) images that you add.
* Try to run your attack programs out of your local copy of the git repository; the large generated files will be kept out of the source tree and this will reduce the risk of accidental commits of unwanted files.

## DES notations

They are the same as in the [DES standard].
If you do not remember what `K16` or `C0D0` is, please have a look at the [DES standard].

## Programming languages

The labs are available in two versions: C language and Python language.
Select the language your are most comfortable with.
The labs are not programming labs, so if you encounter pure programming issues, do not waste time and ask for help.
C programmers should rely only on the C standard library.
Python programmers should rely only on the python standard library, plus `numpy` and `gmpy2`.
Importing any other module will fail.

## Miscellaneous

In the lab instructions you are asked to type commands.
These commands are preceded by a `$` sign representing the prompt of the current shell.
It is not a part of the command, do not type it.

# GitLab and git set-up

* If you never used git on your desktop or laptop, configure your name and email:

    ```bash
    $ git config --global user.name "John Doe"
    $ git config --global user.email johndoe@example.com
    ```
* Clone the project on your desktop or laptop:

    ```bash
    $ cd some/where
    $ git clone git@gitlab.eurecom.fr:renaud.pacalet/hwsec.git
    ```
   Note: if you did not add your SSH key to your GitLab account, you will be asked a password.
Abort and add your SSH key (see the [FAQ](FAQ.md)).
* The master branch is protected and will be used to provide instructions for the labs, code templates, documentation...
Never work in the master branch.
Create your personal branch instead.
Name it `Firstname.Lastname` where `Firstname` is your first name (or a part of it) and `Lastname` is your last name (or a part of it); please do not use pseudos or other fancy names, it is important that a branch owner is easily identified from the branch name.
Switch to your personal branch, push it to the `origin` remote and declare that the local branch will track the remote one, such that you will not have to specify the remote any more when pushing or pulling:

    ```bash
    $ cd hwsec
    $ git branch John.Doe
    $ git checkout John.Doe
    $ git push origin John.Doe
    $ git branch --set-upstream-to=origin/John.Doe John.Doe
    ```
* Remember that you will work in your personal branch.
You can check that you are on the correct branch with the `git branch` command.
* From time to time, when new material will be added to it, you will be asked to merge the master branch in your personal branch.
First fetch the remote changes and merge the master branch into your own branch:

    ```bash
    $ git fetch
    $ git merge origin/master
    ```
* Last but not least, do not forget to add, commit and push your own work in your personal branch.
As you declared that your personal local branch tracks the remote one, there is no need to redeclare it; simply `git push` or `git pull`.

[DES standard]: /doc/data/des.pdf
[bootlin]: https://bootlin.com/
[memento of the most useful GNU/Linux commands]: /doc/data/command_memento.pdf

<!-- vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab textwidth=0: -->

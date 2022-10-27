<!-- MASTER-ONLY: DO NOT MODIFY THIS FILE

Copyright (C) Telecom Paris
Copyright (C) Renaud Pacalet (renaud.pacalet@telecom-paris.fr)

This file must be used under the terms of the CeCILL. This source
file is licensed as described in the file COPYING, which you should
have received as part of this distribution. The terms are also
available at:
http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt
-->

Frequently asked questions

---

- [How to generate a `ssh` key pair?](#how-to-generate-a--ssh--key-pair-)
- [The `git` server asks for a password, how can I avoid this?](#the--git--server-asks-for-a-password--how-can-i-avoid-this-)

---

# How to generate a `ssh` key pair?

`ssh-keygen` is the command that generates `ssh` key pairs.
For this course I suggest that you generate a dedicated key pair and don't protect it with a passphrase; what we do is not critical and skipping the passphrase will significantly smooth your workflow (just type enter when asked twice for a passphrase).
Example under GNU/Linux:

```bash
john@mylaptop> ssh-keygen -t rsa -f ~/.ssh/eurecom.gitlab
Generating public/private rsa key pair.
Enter passphrase (empty for no passphrase):
Enter same passphrase again:
Your identification has been saved in /home/john/.ssh/eurecom.gitlab
Your public key has been saved in /home/john/.ssh/eurecom.gitlab.pub
The key fingerprint is:
SHA256:mP97zcv0/0FNzObUHVvnW8gQrP+4abvgWlurJUTq5/s john@mylaptop
The key's randomart image is:
+---[RSA 3072]----+
|           .o. .o|
|            .o =B|
|          ..  o.@|
|       o o.    =+|
|      o S ..   oo|
|       o .  . .  |
|        o = o=.. |
|         * *+=+..|
|        ..OBE+o.*|
+----[SHA256]-----+
```

Your public key is in `~/.ssh/eurecom.gitlab.pub` and your private key is in `~/.ssh/eurecom.gitlab`.
Of course, as the names indicate, you can (and must) disclose the former but you must keep the latter secret.
This is why `ssh` complains if your private key files have too open access permissions.

The next step consists in adding the public part to your GitLab account.
Copy the **content** of `~/.ssh/eurecom.gitlab.pub` to the clipboard, visit [the User Settings / SSH Keys section of your GitLab account](https://gitlab.eurecom.fr/-/profile/keys), paste the public key and click on the `Add Key` button.

Finally, tell your local `ssh` client that this key pair shall be used to authenticate against `gitlab.eurecom.fr`.
Edit or create `~/.ssh/config` and add these two lines:

```
Host gitlab.eurecom.fr
    IdentityFile ~/.ssh/eurecom.gitlab
```

Normally, you should never be asked again for a password when pulling, fetching or pushing to the git repository.

# The `git` server asks for a password, how can I avoid this?

If you are asked for a password when trying to access the remote repository (`git clone`, `git fetch`, `git pull`, `git push`...) there are several possible reasons:

1. You cloned the repository using the `https` protocol (https://gitlab.eurecom.fr/renaud.pacalet/hwsec.git) and your OS does not provide a `git` credential manager to handle the password for you.
Check the protocol:

   ```bash
   john@mylaptop> cd hwsec
   john@mylaptop> git remote -v
   origin   https://gitlab.eurecom.fr/renaud.pacalet/hwsec.git (fetch)
   origin   https://gitlab.eurecom.fr/renaud.pacalet/hwsec.git (push)
   ```

   If it is `https`, either set up a `git` credential manager or change the protocol for `ssh`:

   ```bash
   john@mylaptop> git remote set-url origin git@gitlab.eurecom.fr:renaud.pacalet/hwsec.git
   john@mylaptop> git remote -v
   origin   git@gitlab.eurecom.fr:renaud.pacalet/hwsec.git (fetch)
   origin   git@gitlab.eurecom.fr:renaud.pacalet/hwsec.git (push)
   ```

1. You use the `ssh` protocol but you did not add your `ssh` public key to your GitLab account.
Log in to the GitLab server (https://gitlab.eurecom.fr/), visit the [_SSH Keys_ section of your _User Settings_](https://gitlab.eurecom.fr/-/profile/keys) and add your public key.
If you do not have a `ssh` key yet generate one (see _How to generate a ssh key pair?_).

1. You do not have a `ssh` agent running or your shell does not know about it.
Check:

    ```bash
    john@mylaptop> ssh-add -l
    Could not open a connection to your authentication agent.
    ```
   If you really do not have a `ssh` agent launch one:

    ```bash
    john@mylaptop> eval $(ssh-agent -s)
    ```

1. You did not add your private key to your `ssh` agent.
Do it:

    ```bash
    john@mylaptop> ssh-add ~/.ssh/eurecom.gitlab # if you followed "How to generate a ssh key pair?"
    john@mylaptop> ssh-add                       # if your ssh private key file is one of the default
    ```

   > Note: if your private key is protected you will have to enter your pass phrase to unlock it.

<!-- vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab textwidth=0: -->

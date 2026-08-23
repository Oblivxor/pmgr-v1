# PMGR V1 — Basic Password Manager

**PMGR V1** is the first version of a lightweight command-line password manager written in **C**.

It provides a simple way to store and retrieve password entries locally using a master password and cryptographic protection provided by **libsodium**.

> **Status:** V1 / Phase 1

> **Platform:** Linux and macOS

> **Language:** C

> **Cryptography:** libsodium

---

## Features

* 🔐 Master password protection
* 🔑 Password-based key derivation using libsodium
* 🔒 Encrypted password entries using `crypto_secretbox`
* 💾 Local vault storage
* ➕ Add password entries
* 🔎 Retrieve password entries by service name
* 🖥️ Simple command-line interface
* ⚡ Automated installation script for Linux/macOS
* 🧹 Sensitive password buffers are cleared from memory after use

---

## How PMGR V1 Works

PMGR V1 uses a local file called:

```text
vault.dat
```

The first time PMGR creates a vault, it generates a random salt.

The user's master password is then processed using libsodium's password hashing functionality to derive an encryption key.

Conceptually:

```text
Master Password
       │
       ▼
   Random Salt
       │
       ▼
 libsodium crypto_pwhash
       │
       ▼
 Encryption Key
       │
       ▼
 crypto_secretbox
       │
       ▼
   vault.dat
```

When a password entry is added, PMGR creates a plaintext record containing:

```text
service|username|password
```

The record is then encrypted before being written to the vault.

Each entry also receives a randomly generated nonce.

When retrieving a password, PMGR derives the key again from the supplied master password and attempts to decrypt the stored entries.

---

## Requirements

### Linux

You need:

* GCC
* Git
* Make
* libsodium development package

On Debian/Ubuntu-based systems:

```bash
sudo apt update
sudo apt install gcc make libsodium-dev git
```

On Fedora/RHEL-based systems:

```bash
sudo dnf install gcc make libsodium-devel git
```

### macOS

You need:

* GCC/Clang
* Git
* Homebrew
* libsodium

Install Homebrew if necessary, then:

```bash
brew install gcc libsodium
```

---

# Installation

There are two ways to install PMGR V1.

## Option 1 — Automatic Installation

The repository includes an installation script:

```text
install-V1.sh
```

First clone the repository:

```bash
git clone https://github.com/oblivxor/pmgr-v1.git
```

Enter the project directory:

```bash
cd pmgr-v1
```

Make the installer executable:

```bash
chmod +x install-V1.sh
```

Run the installer:

```bash
./install-V1.sh
```

The installer will:

1. Detect Linux or macOS
2. Install the required dependencies
3. Compile PMGR V1
4. Install the resulting `pmgr` executable into:

```text
/usr/local/bin/
```

After installation, you can run:

```bash
pmgr
```

---

# Manual Installation

If you prefer to install everything yourself, install the required dependencies first.

Then compile PMGR with:

```bash
gcc pmgr-V1.0.c -o pmgr -lsodium
```

You can then run it directly from the current directory:

```bash
./pmgr
```

---

# Usage

PMGR V1 currently provides two commands:

```text
pmgr add
pmgr get
```

You can also run:

```bash
pmgr
```

to display the available commands.

---

## Adding a Password

To add a password entry:

```bash
pmgr add
```

PMGR will ask for:

```text
› Master password:
› Service:
› Username:
› Password:
```

For example:

```text
$ pmgr add

› Master password: ********
› Service: github
› Username: myusername
› Password: mypassword

✔ Entry stored securely
```

The password entry is encrypted before being stored in the local vault.

---

## Retrieving a Password

To retrieve an existing entry:

```bash
pmgr get
```

PMGR will ask for the master password and the service name:

```text
$ pmgr get

› Master password: ********
› Service name: github

Username: myusername
Password: mypassword
```

If the requested service does not exist, PMGR displays:

```text
✖ Entry not found
```

---


# Security

PMGR V1 uses cryptographic functionality provided by **libsodium**.

### Password-Based Key Derivation

The master password is processed using:

```c
crypto_pwhash()
```

A randomly generated salt is used when the vault is created.

The resulting key is used for encryption and decryption.

### Encryption

Password entries are encrypted using:

```c
crypto_secretbox_easy()
```

Each stored entry receives a randomly generated nonce.

Decryption is performed using:

```c
crypto_secretbox_open_easy()
```

If authentication fails, the encrypted entry is ignored.

### Memory Cleanup

After an entry is processed, PMGR clears sensitive buffers containing the master password and stored password using:

```c
sodium_memzero()
```

---

# Important Security Notice

PMGR V1 is an **early version of the project** and should be considered a learning/experimental password manager.

Although V1 uses real cryptographic primitives through libsodium, it has not undergone a professional security audit.

Do **not** rely on PMGR V1 as your primary password manager for highly sensitive real-world credentials.

Future versions of PMGR are intended to improve security, usability, and functionality.

---

# Current Limitations

PMGR V1 is intentionally simple.

### No password generator

Passwords must be entered manually.

### No clipboard integration

Retrieved passwords are displayed in the terminal and must be copied manually.

### Password input is visible

The current version does not hide the master password or password input while typing.

### No password editing

Existing entries cannot currently be edited.

### No password deletion

Entries cannot currently be deleted.

### No service listing

PMGR V1 does not provide a command to list all stored services.

### No cloud synchronization

The vault is stored locally.

### No automatic locking

The program does not provide an inactivity-based auto-lock mechanism.

### No graphical interface

PMGR V1 is completely command-line based.

---

# Building From Source

You can compile PMGR directly with GCC:

```bash
gcc pmgr-V1.0.c -o pmgr -lsodium
```

Then run:

```bash
./pmgr
```

For installation into your system path:

```bash
sudo mv pmgr /usr/local/bin/pmgr
```

You can then execute PMGR from anywhere:

```bash
pmgr
```

---

# Example Workflow

A basic PMGR V1 workflow looks like this:

```bash
# Install PMGR
./install-V1.sh

# Add a password
pmgr add

# Retrieve a password
pmgr get
```

The vault is stored locally in:

```text
vault.dat
```

---

# Contributing

Contributions, suggestions, bug reports, and security feedback are welcome.

If you find a security issue, please avoid publicly posting sensitive information or real credentials in an issue.

---

# License

This project is licensed under the terms specified in the `LICENSE` file.

---

## Author

**Muhammad Abdullah**  
GitHub: [@Oblivxor](https://github.com/Oblivxor)

**PMGR — Password Manager Project**

PMGR is an ongoing project focused on learning, experimentation, and building a password manager from the ground up in C.

---

⭐ If you find the project interesting, consider giving the repository a star.


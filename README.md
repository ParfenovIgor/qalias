# QAlias

*This is a very early version*

An IDE for [Alias](https://github.com/ParfenovIgor/alias-c) programming language with alias language server support.

## Installation

* building from source

## Building from Source

Required dependencies:

* Qt version 5.*
* make

To build:

```
qmake
make
```

## Get `qalias` using Nix

Required dependencies:

* [Nix](https://nixos.org)
    * Use [this](https://github.com/DeterminateSystems/nix-installer) or [this](https://nixos.org/download/) way to install Nix

Enter the development shell for `qalias`:

```
nix develop github:ParfenovIgor/qalias
```

Check that `qalias` is present:

```
qalias
```

## Usage

In your project's directory:

```
qalias
```

For full usage `ALTLIB` environmental variable is required.

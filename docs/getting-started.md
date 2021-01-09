---
title: Getting started
nav_order: 1
---

# Getting started

Currently, mime is available only for Linux.

## Download the prebuilt binary

An archive containing a prebuilt linux-amd64 binary can be downloaded from [here](https://github.com/shsms/mime/releases/latest).

Extract the file into `/usr/bin/` to make it available to all users,  or to `~/.local/bin/` for just the current user.

Checkout the [What next](#what-next) section.

## Build from source

### Install the prerequisites

I've tested only with gcc 10.2.0, but if you are using a recent enough version of your OS, gcc version shouldn't be a problem.

On Manjaro/Arch,  use:

	sudo pacman -S make cmake gcc git

On Fedora, use:

	sudo dnf install make cmake gcc-c++ git

And on Ubuntu/Linux Mint/etc, use:

	sudo apt install make cmake g++ git

### Download and build

Use the below command to download the latest version of the mime source code:

	git clone https://github.com/shsms/mime && cd mime

And build using:

	make init    ## to initialize the submodules
	make build   ## to build
	make test    ## to run the unit tests.
	
### Install

Install to the `systemd-path user-binaries` (which defaults to [~/.local/bin/](https://www.freedesktop.org/software/systemd/man/file-hierarchy.html#~/.local/bin/)) using:

	make install

## What next?

1. Go to the [examples](examples.md) page for some example scripts that you can play with.
2. Go to the [concepts](concepts.md) page for details on some of the concepts that mime is based on.
3. Go to the [reference](reference.md) page for a list of all mime functions.
4. Go to the ChaiScript [Getting started](https://codedocs.xyz/ChaiScript/ChaiScript/LangGettingStarted.html) and [Cheatsheet](https://github.com/ChaiScript/ChaiScript/blob/develop/cheatsheet.md#language-reference) pages for documentation on how to use ChaiScript,  which is the scripting language that mime is built on.

---
nav_order: 0
---
# Introduction

Mime is a text processing tool inspired by Emacs, written in C++.

Mime provides an editor-like abstraction for manipulating text files,
but in a scripting environment, without an editor.  This enables very
sophisticated transformations that are easy to do through tools like
Emacs Keyboard Macros, but hard to do in code.

Visit the [Getting started](getting-started.md) page for instructions.

## Design

Mime is designed to be familiar and easy to use for Emacs users.
Almost all programs you write with mime can also be expressed as Emacs
macros.  But here are some advantages and disadvantages with using
mime over Emacs:

## Advantages

1. You get reproducibility in evironments where Emacs might not be
   available.
2. The ability to document and check in your code to source control.
3. Write text manipulation scripts that are much larger than what an
   Emacs macro can be.
4. Runs much faster than an Emacs macro on large amounts of data,
   although I haven't done any measurements.

## Disadvantages

1. The biggest disadvantage is that mime is not Emacs, and it can
   never expect to match the amount of features provided by Emacs.
2. Not visual, so it is possibly hard to get an intuitive
   understanding of how to use mime, without a lot of experience using
   Emacs macros.

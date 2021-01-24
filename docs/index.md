---
title: Introduction
nav_order: 0
---
# Mime

Mime is a scripting language framework for text processing, inspired
by Emacs.

Mime provides an editor-like abstraction for manipulating text files,
but in a scripting environment, without an editor.  This enables very
sophisticated transformations that are easy to do through tools like
Emacs Keyboard Macros, but hard to do in code.

* Visit the [Getting started](getting-started.md) page for instructions
* Checkout some [examples](examples.md)
* Checkout the [Concepts](concepts.md) and [Reference](reference.md) pages
* Or the code from [github](https://github.com/shsms/mime).

## Design

Mime is designed to be familiar to Emacs users but also easy to learn
and use for everyone.  A lot of things you can do with Emacs Keyboard
Macros can also be done with mime, but mime can also do a lot more
than that.

Here are some advantages and disadvantages with using mime over Emacs:

## Advantages

1. You get reproducibility in evironments where Emacs might not be
   available.
2. The ability to document and check in your code to source control.
3. Write text manipulation scripts that are much larger and more
   complex, than what an Emacs macro can be.
4. Faster than an Emacs macros, especially when dealing with
   complex/unstructured data and for source code transformations.

## Disadvantages

1. The biggest disadvantage is that mime is not Emacs, and it can
   never expect to match the amount of features provided by Emacs.
2. Not visual, so it is possibly hard to get an intuitive
   understanding of how to use mime, without a lot of experience using
   Emacs macros.

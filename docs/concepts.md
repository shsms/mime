---
nav_order: 3
---
# Concepts

Mime is a scripting language, that provides a way to reimagine text processing tasks as operations a text editor.  Here are some concepts you should know to be able to use mime effectively.  These are written to be helpful to someone who is not familiar with Emacs.  If you are familiar with Emacs,  it is still useful to read the [multiple cursors](#multiple-cursors) section at least.

## Buffer

A buffer in mime,  is like an open file in a text editor.  A buffer can be created from the contents of a file,  or you can create empty buffers.  Inside a buffer,  you can do normal text-editor operations, like cut/copy/paste, navigation, search for text, replace, etc.

Every buffer starts with a single cursor that moves around the buffer based on the operations performed.  Additional cursors can also be created for more complex tasks ([see below](#multiple-cursors)).  There can be multiple buffers open in memory,  and text can be copied/moved across them.

*Similar to [Emacs Buffers](https://www.gnu.org/software/emacs/manual/html_node/emacs/Buffers.html).*

## Region

A region in mime is similar to selected/highlighted text in a text editor.  If you want to cut/copy parts of the buffer, for example,  you first need to make a region around the text you want to cut or copy.

A region can be created by calling the `set_mark()` function to mark one end of the region,  and use one of the navigation functions to navigate to the other end of the region.

*Similar to [Emacs Regions](https://www.gnu.org/software/emacs/manual/html_node/emacs/Mark.html#Mark). (That article mentions something called a `point`.  In Emacs terminology,  a `point` is the location of the cursor in a buffer,  so the text between the `mark` and the `point` is a region.)*

## Narrowing

In mime,  it is possible to narrow the view of a cursor to a specific part of the buffer,  such that it is not possible for the cursor to navigate outside of the narrowed view.  So a `find` call will not match text that's outside the narrowed view, for example.

This is done using the `narrow_to_region` or `narrow_to_block` functions.  The narrowed view can grow or shrink as text is added or removed inside it.  Once we are done with that part of the buffer,  we can call `widen()` to remove the restriction.

*Similar to [Narrowing in Emacs](https://www.gnu.org/software/emacs/manual/html_node/emacs/Narrowing.html#Narrowing)*

## Multiple cursors

Mime allows the creation of multiple cursors in the same buffer,  which can all move around the buffer independent of each other.  As changes are made to the contents of the buffer,  all the cursors try to retain their relative positions in the buffer as much as possible.

For example,  when some text is pasted into the beginning of the buffer,  all subsequent cursors would move forward along with the text around them,  so that they are in the same relative positions to the text around them.  But when a cursor deletes all the contents of a buffer,  then all cursors go to the beginning of the buffer.

Narrowing/widening only affects the current cursor.  In other words,  each cursor can have its own narrowed view,  and can be narrowed or widened independant of other cursors.  Narrowed views also try to retain their relative position to the text around them,  as changes are made to the buffer.

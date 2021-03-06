---
title: Multiple cursors
parent: Examples
nav_order: 1
---

# Multiple cursors

There are many situations where having multiple cursors in the same file can be useful.  In the below script,  we use two cursors. One to navigate the file and find function names,  and another that stays near the start of the file,  to insert a comment line for each function the other cursor finds.

### gofunc.mime
``` js
var b = buffer("main.go");
var doc_c = b.new_cursor();
var nav_c = b.new_cursor();

b.use_cursor(nav_c);

while(b.find("func ") >= 0) {
    b.set_mark();
    b.find("(");
    b.backward();

    var fname = b.copy();

    b.use_cursor(doc_c);
    b.paste("// FuncAlert: " + fname + "\n");

    b.use_cursor(nav_c);
}
b.use_cursor(doc_c);
b.paste("\n");

b.save_as("mimeout.go");
```

### main.go

If there is a file "main.go" in the same directory with the below
contents,

``` go
package main

import "fmt"

func main() {
	fmt.Print(hello(), world())
}

func world() string {
	return "world!"
}

func hello() string {
	return "Hello "
}
```

Then running the above mime script with:

``` shell
mime gofunc.mime
```

would generate a new file `mimeout.go` with below contents:

``` go
// FuncAlert: main
// FuncAlert: world
// FuncAlert: hello

package main

import "fmt"

func main() {
        fmt.Print(hello(), world())
}

func world() string {
        return "world!"
}

func hello() string {
        return "Hello "
}
```

Here's an explanation of the script:

   1. open file "main.go"
   2. create two cursors - one for adding documentation at the top,
      and one for navigating the file.
   3. switch to the navigation cursor.
   4. find and goto next occurence of the string "func ".  If found:
      1. set mark (emacs parlance for "start selecting")
      2. find next occurence of "(" (assuming we are operating on a
         valid go program,  we don't check if "(" was found, we assume
         it is there.)
      3. go back one character,  we don't want to copy the "("
      4. copy the text between mark (where we started selecting in
         4.1), and (current cursor) point, and store in a var called
         "fname".
      5. switch to the documentation cursor, which is still at the top
      6. paste the function name in "fname" with a comment prefix and
         a newline at the end.
      7. switch to navigation cursor
      8. goto step 4.
   5. switch to doc cursor to insert a final newline to introduce a
      gap between inserted text and original program.
   6. save file under a new name.

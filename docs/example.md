# Example

Here's an example mime script ([examples/gofunc.mime](github.com/shsms/mime/examples/gofunc.mime)):

``` js
var b = buffer("main.go");
var doc_c = b.new_cursor();
var nav_c = b.new_cursor();

b.use_cursor(nav_c);

while(b.find("func ")) {
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

If it is not already obvious, here's what the script does:

   1. open file "main.go"
   2. create two cursors - one for adding documentation at the top,
      and one for navigating the file.
   3. switch to the navigation cursor.
   4. find and goto next occurance of the string "func ".  If found:
      1. set mark (emacs parlance for "start selecting")
      2. find next occurance of "(" (assuming we are operating on a
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

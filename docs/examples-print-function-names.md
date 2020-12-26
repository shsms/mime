---
title: Print function names
parent: Examples
nav_order: 0
---

# Print function names

The below script reads a go source file and prints the names of the functions.

### func-names.mime
``` js
var b = buffer("main.go");

while(b.find("func ") >= 0) {
    b.set_mark();
    b.find("(");
    b.backward();

    print(b.copy());
}
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
mime func-names.mime
```

would print this output

```
main
world
hello
```

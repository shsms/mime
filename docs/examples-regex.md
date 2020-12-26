---
title: Find regex
parent: Examples
nav_order: 2
---

# Find regex

### regex-names.mime
``` js
var b = buffer("main.go");

while(b.find(regex("\n[a-z]+ [a-z]+\\(")) >= 0) {
    b.backward();
    b.set_mark();
	b.rfind(" ");
	b.forward();
	
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
mime regex-names.mime
```

would print this output

```
main
world
hello
```

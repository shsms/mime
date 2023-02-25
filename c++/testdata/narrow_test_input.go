package main

import "fmt"

func main() {
	if(true) {
		fmt.Print(hello())
	}
	
	fmt.Print(world())
	
	if (true) {
		fmt.Println()
	} else {
		fmt.Print()
	}
}

func world() string {
	return "world!"
}

func hello() string {
	return "Hello "
}

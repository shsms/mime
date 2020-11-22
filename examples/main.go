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

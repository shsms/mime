---
nav_order: 4
---
# Reference

This is the reference page for mime-specific functions.  For
documentation about the scripting language in general, checkout
ChaiScript's [Getting started](https://codedocs.xyz/ChaiScript/ChaiScript/LangGettingStarted.html)
and [Cheatsheet](https://github.com/ChaiScript/ChaiScript/blob/develop/cheatsheet.md#language-reference) pages.

If you haven't already done so, it would help to take a look at the
[concepts](concepts.md) page first.

# Buffers

## Creating a buffer

```js
// try to open "filename", if not found, create an empty buffer with the name "filename".
var buf = buffer("filename");
var buf = buffer("filename", try_open);  // same as above.

// if "filename" is not found, crash
var buf = buffer("filename", must_open);

// create an empty buffer.
var buf = buffer();
```

## Saving a buffer

```js
var buf = buffer("filename");

buf.save();               // save as "filename"
buf.save_as("new_file");  // save as "new_file"
```

## Navigation

Navigation functions move the currently active cursor in a buffer, relative to its current position in the buffer.

### character level
```js
var buf = buffer("filename");

buf.forward();     // move forward one character.
buf.forward(10);   // move forward 10 characters.

buf.backward();    // move backward one character.
buf.backward(10);  // move backward 10 characters.

buf.goto_pos(100); // goto position 100 bytes from the beginning of the file.
                   // returns false if 100 is outside the contents of the buffer.
```

### line level
```js
buf.next_line();   // move to the beginning of the next line.
buf.next_line(10); // move to beginning of 10th line from current position.

buf.prev_line();   // move to the beginning of the previous line.
buf.prev_line(10); // move to beginning of 10 lines before current position.
```

They return the number of lines they moved by.  For example, you can do something like this to add a line number on every line:

```js
var buf = buffer("filename");

var linenumber = 0;
buf.paste(to_string(++linenumber) + ". ");
while(buf.next_line() > 0) {
	buf.paste(to_string(++linenumber) + ". ");
}
```

There's also

```js
buf.start_of_line();   // move to the beginning of the current line.
buf.end_of_line();     // move to the end of the current line.
```

### block level

The block level navigation functions are specifically for use in source code files that use curly braces `{}` to define blocks.

```js
buf.start_of_block();  // move cursor one character before the '{' character that would take
                       // the outside the current nesting level.  So if there is a {} pair above 
					   // current position, before the current nesting level's opening brace,
					   // the cursor will move back past the {} pair to the current level's 
					   // opening brace.  Returns false if already at top level,  and true otherwise.

buf.end_of_block();    // similar to start_of_block,  but moves forward from current position,
                       // until just after the '}' character matching the current level.
```

So if cursor is inside some deep nested level,  we can go back to the top level with something like:

```js
var buf = buffer("filename");
buf.find("// nested for");   // go to some nested location.

while(buf.start_of_block()) {}
```

### buffer level

```js
buf.start_of_buffer();  // go to the start of the buffer.
buf.end_of_buffer();    // go to the end of the buffer.
```
## Searching

### find

The find function searches forward starting from the current position in the buffer,  for a specic string or regular expression.  If a match is found,  the cursor gets moved to the end of the match,  and the position of the beginning of the match is returned.  If there is no match,  the find function returns -1.

```js
buf.find("some text");       // go to the end of the next "some text" in the buffer.
buf.find(regex("<[a-z]+>")); // go to the end of the next html open tag.
```

The below example looks for a html tag that has atleast one attribute,  and if one is found,  prints just the tag.  So it should print "font".
```js
var buf = buffer();
buf.paste("some text <a> <font size=\"+2\"> hello </font> </a>");
buf.start_of_buffer();
if(var loc = buf.find(regex("<[a-z]+ [a-z]+=")); loc >= 0) {
	buf.goto_pos(loc);             // goto the beginning of the tag
	buf.forward();                 // go forward one character to skip the <
	buf.set_mark();                // mark one end of a region
	buf.goto_pos(buf.find(" "));   // goto the beginning of the next space character.
	print(buf.copy());             // copy from mark to point and print.
}
```

### rfind

rfind searches in reverse, starting from the current position in the buffer, for a specific string.  If a match is found,  cursor gets moved to the beginning of the match,  and the position of the end of the match is returned.  If there is no match,  it returns -1.

```js
buf.rfind("some text");   // go to the beginning of the previous "some text" in the buffer.
```

## Editing

### cut/copy/paste

You first need to create a region before you can cut or copy text.  To create a region,  go to one end of the region and call `<buf>.set_mark();` to start creating a region.  Then navigate to the other end of the region using one or more of the navigation or search functions mentioned above.  Now you have a region that you can cut or copy.

Paste inserts text at the current position, and moves the cursor to the end of the pasted text.

```js
var buf = buffer();
buf.paste("----some text----");
buf.rfind("s");
buf.set_mark();
buf.find("xt");
var t = buf.copy();
buf.end_of_buffer();
buf.paste("\ncopied: " +  t);
print(buf.get_contents());
```

should print:

```
----some text----
copied: some text
```

### replace

Replaces text in the buffer.  `<buffer>.replace(from, to, n)` replaces from with to, n times, starting from current position.  If n is 0,  replace all occurances starting from current position.  If n is missing,  assume n = 0.  Returns the number of instances replaced.

```js
var buf = buffer();
buf.paste("----some text----");
buf.rfind("t");
buf.replace("-", "+");
print(buf.get_contents());
```

should print:

```
----some text++++
```

### erase_region

erase_region is similar to cut,  except it doesn't return anything.

## Cursors
Read about how multiple cursors work in mime, in the [concepts](concepts.md#multiple-cursors) page.

```js
// create a new cursor at the beginning of the buffer and save its id
var new_c = buf.new_cursor();

// switch to a different cursor
buf.use_cursor(new_c);
```

## Narrowing

Read about how narrowing works in mime, in the [concepts](concepts.md#narrowing) page.

```js
// when there is a region, narrow to the region and return true
// else return false.
buf.narrow_to_region();

// when inside a block,  narrow to the block and return true,  else
// return false.
buf.narrow_to_block();

// check if view is currently narrowed
var n = buf.narrowed();
if(n) {
	print("narrowed");
}
```

And here's a small example:

```js
def print_leading(buf) {
	if (buf.narrowed()) {
		print("narrowed view:");
	} else {
		print("full view:")
	}
	var rex = regex("[a-z]+ line")
	var pos = buf.find(rex)
	while(pos >= 0) {
		buf.goto_pos(pos);
		buf.set_mark();
		buf.goto_pos(buf.find(" "));
		print(buf.copy());
		pos = buf.find(rex);
	}
	print("");
}

var buf = buffer();
buf.paste("first line\nsecond line\nthird line\n");
buf.start_of_buffer();

print_leading(buf);

buf.start_of_buffer();
buf.set_mark();
buf.end_of_line();
buf.narrow_to_region();  // narrow to first line

print_leading(buf);
```

running this would print:

```
full view:
first
second
third

narrowed view:
first
```
## Other buffer functions

### empty

Returns a boolean indicating whether the buffer is empty. If narrowed, just if the narrowed region is empty.

```js
if (buf.empty()) {
	print("empty");
}
```

### get_name

Returns the filename used to open/create the buffer.

```js
var n = buf.get_name();
print(n);
```

### get_contents

Returns the contents of the buffer.  If narrowed, just the contents of the narrowed view.

```js
var c = buf.get_contents();
print(c);
```

### size

Returns the number of characters in the buffer.  If narrowed, just the number of characters in the narrowed region.

```js
var s = buf.size();
print("size: " + to_string(s));
```

### get_mark

Returns the location of the current mark.  Returns -1 if there is no mark.

```js
var m = buf.get_mark();
```

### get_pos

Returns the location of the active cursor.

```js
var m = buf.get_pos();
```
# String/text functions
For strings,  these functions are available: `<string>.size()`, `<string>.empty()`, `==` to check equality, `+` to concatenate.  In addition to these,  there are:

### ltrim

Removes leading whitespace.

```js
print(ltrim("  \n  hello\n\n"));
```

would print

```
hello


```

### rtrim

Removes trailing whitespace.

```js
print(rtrim(buf.copy()))
```

### trim

Removes leading and trailing whitespace

```js
print(trim(buf.copy()))
```

# Parsing command line argument

Mime scripts can parse command line arguments passed to them.  Take a look at the [CSV to JSON](examples-csv-to-json.md) example.

```js
// We first acquire the arg_parser object like this:
var parser = args_parser();

// Then we can define arguments like this:

parser.string_opt("o,output", "name of the output file");
parser.int_opt("items", "number of items required",  "100");

// then parse and get the values like this:

var args = parser.parse();

print(args.output);
print(args.items);
```

There are 4 possible argument types that can be defined:

- int_opt
- float_opt
- bool_opt
- string_opt

They all take the same parameters:

```js
parser.int_opt(param_name, desc, default);
```

- param_name  -> "<shortform>,<longform>".  Short form is optional.  The long form will be the member name in the parsed result.
- desc  -> description of the param.
- default  -> optional default value.  Must always be string, for example, "1.2" for float,  and "true" for bool.

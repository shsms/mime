---
title: CSV to JSON
parent: Examples
nav_order: 3
---

# CSV to JSON

Here's an example script that converts simple csv files into json.

Download a sample csv file from here:  [mime-sample.csv](https://gist.githubusercontent.com/shsms/d6208388acd3503cde3a9c52279264c1/raw/80b0272bec6969d5675824a3bc364b863b0db66c/mime-sample.csv)

Save the script under file name `csv2json.mime`, and run using:

    chmod +x csv2json.mime
	./csv2json.mime -f mime-sample.csv

### csv2json.mime
``` js
#!/usr/bin/env mime

def cliargs() {
    var parser = args_parser();
    parser.string_opt("o,output", "name of the output file", "/dev/stdout");
    parser.string_opt("f,file", "name of input csv file, required");

    var args = parser.parse();
    if (args.file.empty()) {
        print(parser.help());
        exit(1);
    }

    return args;
}

def read_row(c) {
    c.start_of_line();    // go to the start of current line
    c.set_mark();         // start defining a region
    c.end_of_line();      // navigate to end of region
    c.narrow_to_region(); // narrow the view to current line.

    var ret = [];
    if(c.get_contents().empty()) {  // if line(narrowed view) is empty
        return ret;
    }
    c.set_mark();                // start a region
    while(c.find(",") >= 0) {    // while find and move to next comma is successful,
        c.backward();            // go back one char, so we don't copy the comma.
        ret.push_back(c.copy()); // copy region and push into vector "ret".
        c.forward();             // move forward one char, to ignore the comma.
        c.set_mark();            // start a new region
    }

    c.end_of_line();
    ret.push_back(c.copy());
    
    c.widen();
    
    return ret;
}

def main() {
    var args = cliargs();
    var csv = buffer(args.file, must_open);
    var json = buffer();

    var header = read_row(csv);

    json.paste("[");
    var first = true;
    while(csv.next_line() > 0) {
        var row = read_row(csv);

        if (row.empty()) {
            break;
        }

        if (!first) {
            json.paste(",\n");
        }
        first = false;
        json.paste("{");

        for(var ii = 0; ii < row.size(); ++ii) {
            if (ii > 0) {
                json.paste(",");
            }
            json.paste("\n  \"" + header[ii] + "\": \"" + row[ii] + "\"" );
        }
        json.paste("\n}");
    }
    json.paste("]\n");
    json.save_as(args.output);
}

main()
```

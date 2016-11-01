	SORT-COMMAND

Himanshu Chopra
111508074

Sort - Sort lines of text files

usage: ./project [OPTION]... [FILE]...
   or: ./project [OPTION]... --files0-from=F
Write sorted concatenation of all FILE(s) to standard output.
With no file, or when FILE is -, read standard output.

Ordering Options:
     -b, ignore leading blanks	Ignore leading blanks
     -f, ignore-case		Fold lower-case to upper-case characters
     -n, numersic sort		Compare according to string numeric value
     -r, reverse		Reverse the result of comparisions
Other Options: 
     -c, check			Check for sorted input, and report first bad line; Do not sort
     -k, key=KEYDEF		Sort via a key. KEYDEF gives location
     -m, merge			Merge already sorted files; Do not sort.
     -o, output=FILE		Write result to FILE instead of standard output
     -u, unique			Remove duplicates from result
     --files0-from=F,		Read input from the files specified by NUL-terminated names in file F
				If F is - then read names from standard input
     -h,--help			Print this usage and exit

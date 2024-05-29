cleancc is a C code cleaner with proper indentation.
Try it with a backup of the code .
Options can be seen with '-h/--help'
In-code declarations are three to skip cleaning portions
by 
#define D_NOCLEANCC and 
#define D_CLEANCC
by default D_CLEANCC is defined

Sometimes 'cleancc' may report 'brackets not matching'
It can be due to conditional code parts
add sufficient extra brackets with in
#if 0
}
...
#endif
It won't affect your code.

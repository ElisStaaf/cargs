# CArgs
CArgs is a command line argument parser for both C *and* C++ 
(dunno why you'd want to write C++ though -_-), it includes
such features such as... Command line argument parsing. Enjoy!

## Install
```sh
git clone https://github.com/ElisStaaf/cargs
cd cargs
sudo make
```

## Example
``C``:
```c
#include<cargs.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<limits.h>

#define HELP_EXPECT   1
#define STRING_EXPECT "itiomorbovina"
#define UINT_EXPECT   2976579765U

int main(int argc, char **argv)
{
    Cargs cargs;

    cargs_init(&cargs);

    printf("Running validity tests... ");

    int status;

    status = cargs_arg_add("help",   "-h", "--help", "show this message and exit",  1, &cargs);
    assert(status == CARGS_ADD_OK);

    status = cargs_arg_add("string", "-s", "--string", "test string", 0, &cargs);
    assert(status == CARGS_ADD_OK);

    status = cargs_arg_add("uint",   "-u", "--uint", "test uint",    0, &cargs);
    assert(status == CARGS_ADD_OK);

    cargs_args_parse(argv, argc, &cargs);

    /* check if "help" exists */
    assert(cargs_arg_present("help", &cargs) == HELP_EXPECT);
    /* "help" is a flag and doesn't have a value. */
    assert(cargs_arg_val_get_strlen("help", NULL, &cargs) == CARGS_GET_TYPE_MISSMATCH);

    /* check if "string" exists */
    assert(cargs_arg_present("string", &cargs) == 1);

    char *str_out = NULL;
    size_t str_out_len = 0;

    /* get "string"'s length */
    status = cargs_arg_val_get_strlen("string", &str_out_len, &cargs);
    assert(str_out_len == strlen(STRING_EXPECT));

    /* get "string"'s value */
    str_out = malloc(sizeof(char) * (str_out_len + 1));
    status = cargs_arg_val_get_str("string", str_out, &cargs);
    assert(status == CARGS_GET_OK);
    assert(strcmp(str_out, STRING_EXPECT) == 0);
    free(str_out);

    /* check if "uint" exists */
    assert(cargs_arg_present("uint", &cargs) == 1);

    unsigned int uint_out;

    status = cargs_arg_val_get_uint("uint", &uint_out, &cargs);
    assert(status == CARGS_GET_OK);
    assert(uint_out == UINT_EXPECT);

    printf("passed.\n");
    printf("Running invalidity tests... ");

    assert(cargs_arg_present("invalid", &cargs) == 0);

    str_out_len = 0;
    status = cargs_arg_val_get_strlen("invalid", &str_out_len, &cargs);
    assert(status == CARGS_GET_NOT_FOUND);
    assert(str_out_len == 0);

    str_out = NULL;
    status = cargs_arg_val_get_str("invalid", str_out, &cargs);
    assert(status == CARGS_GET_NOT_FOUND);
    assert(str_out == NULL);

    uint_out = 0;
    status = cargs_arg_val_get_uint("invalid", &uint_out, &cargs);
    assert(status == CARGS_GET_NOT_FOUND);
    assert(uint_out == 0);

    printf("passed.\n");

    cargs_help_msg_gen(
        "Usage: Blabla",
        "Blabla",
        &cargs
    );
    printf("%s\n", cargs_help_msg_get(&cargs));

    cargs_destroy(&cargs);
    return 0;
}
```
``C++``:
```cpp
#include <iostream.h>
using namespace std;

int main() {
    std::cout << "I'm to lazy rn -_-, I'll add an example later.\n"
    std::cout << "    Sincerely, Elis Staaf."
    return 0;
}
```

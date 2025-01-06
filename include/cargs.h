#ifndef CARGS_H
#define CARGS_H

#include<stddef.h>

typedef enum E_CargsHelpMsgGenStatus
{
    /**
     * The help message was generated successfully.
    */
    CARGS_HELP_MSG_GEN_OK            = 0,
    /**
     * The cargs struct contains invalid information.
    */
    CARGS_HELP_MSG_GEN_INVALID_CARGS  = 1,
    /**
     * The helps message exceeded CARGS_HELP_MSG_LEN
    */
    CARGS_HELP_MSG_GEN_BUF_LIMIT     = 2
}  CargsHelpMsgGenStatus;

typedef enum E_CargsAddStatus
{
    CARGS_ADD_OK             = 0,
    /** 
     * Another argument with the same S_CargsArgument::name
     * already exists.
     */
    CARGS_ADD_NAME_DUP       = 1,
    /** 
     * Another argument with the same S_CargsArgument::arg_short
     * already exists.
     */
    CARGS_ADD_SHORT_DUP      = 2,
    /** 
     * Another argument with the same S_CargsArgument::arg_long
     * already exists.
     */
    CARGS_ADD_LONG_DUP       = 3,
    /** 
     * Both S_CargsArgument::arg_short and S_CargsArgument::arg_long 
     * were NULL.
     */
    CARGS_ADD_INVALID_ARGS   = 4
} CargsAddStatus;

typedef enum E_CargsGetStatus
{
    /** Info was gathered successfully. */
    CARGS_GET_OK             = 0,
    /**
     * Argument was either not added by cargs_arg_add or 
     * cargs_args_parse didn't find it.
     */
    CARGS_GET_NOT_FOUND      = 1,
    /** An argument was interepreted in an invalid way. */
    CARGS_GET_TYPE_MISSMATCH = 2
} CargsGetStatus;

typedef struct S_CargsArgument
{
	char *name;
	char *arg_short;
	char *arg_long;
    char *value;
    char *description;
	int is_flag;
} CargsArgument;

typedef struct S_Cargs
{
    CargsArgument *args;
    size_t args_cnt;
#define CARGS_HELP_MSG_LEN 4096
    char *help_msg;
} Cargs;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Inits the cargs struct.
 * @param cargs Cargs handle.
 */
void cargs_init(Cargs *cargs);
/**
 * Destroys the cargs struct and frees any associated memory.
 * @param cargs Cargs handle.
 */
void cargs_destroy(Cargs *cargs);
/**
 * Adds a new argument to the cargs struct for later parsing. 
 * Note: At least either of arg_short/arg_long need to be specified.
 * @param name The name of the argument.
 * @param [arg_short] The short CLI version of the argument or NULL. 
 * @param [arg_long] The long CLI version of the argument or NULL. 
 * @param [description] Description of the argument, used for generating the help message or NULL. If NULL the description for this argument will be "--". 
 * @param is_flag Whether the argument is a flag.
 * @param cargs Cargs handle.
 * @return CargsAddStatus 
 */
CargsAddStatus cargs_arg_add(
    const char *name,
    const char *arg_short,
    const char *arg_long,
    const char *description,
    int is_flag,
    Cargs *cargs
);
/**
 * Parses based on the previously added arguments to the 'cargs' struct.
 * @param argv The array of arguments to parse.
 * @param argc The number of arguments to parse from 'argv'.
 * @param cargs Cargs handle.
 */
void cargs_args_parse(char **argv, int argc, Cargs *cargs);
/**
 * Searches for the argument with name 'name', interprets it's value as a string and stores it into 'out'. 
 * Note that 'out' should be properly allocated before calling thing function (see cargs_arg_get_str_len).
 * If any errors occur, the value of 'out' is not touched.
 * @param name The name of the argument.
 * @param out Where the value of the argument will be stored.
 * @param cargs Cargs handle.
 * @return CargsGetStatus
 */
CargsGetStatus cargs_arg_val_get_str(const char *name, char *out, Cargs *cargs);
/**
 * Searches for the argument with name 'name', interprets it's value as an unsigned int and stores it into 'out'. 
 * If any errors occur, the value of 'out' is not touched.
 * @param name The name of the argument.
 * @param out Where the value of the argument will be stored.
 * @param cargs Cargs handle.
 * @return CargsGetStatus
 */
CargsGetStatus cargs_arg_val_get_uint(const char *name, unsigned int *out, Cargs *cargs);
/**
 * Searches for the argument with name 'name', interprets it's value as a string, gets it's length and stores it into 'out'. 
 * If any errors occur, the value of 'out' is not touched.
 * @param name The name of the argument.
 * @param out Where the string's length will be stored.
 * @param cargs Cargs handle.
 * @return CargsGetStatus
 */
CargsGetStatus cargs_arg_val_get_strlen(const char *name, size_t *out, Cargs *cargs);
/**
 * Searches for the argument with name 'name'. 
 * If any errors occur, the value of 'out' is not touched.
 * @param name The name of the argument.
 * @param cargs Cargs handle.
 */
int cargs_arg_present(const char *name, Cargs *cargs);
/**
 * Generates an internal formatted help message string based on 
 * already added arguments via cargs_arg_add(). 
 * You can get a pointer to this string via cargs_help_msg_get().
 * @param cargs Cargs handle. 
 * @return 0 on success, 1 otherwise.
 */
CargsHelpMsgGenStatus cargs_help_msg_gen(
    const char *usage, 
    const char *description,
    Cargs *cargs
);
/**
 * Gets a pointer to the help message string generated by cargs_help_msg_gen(). 
 * @param cargs Cargs handle. 
 * @return NULL if no message was generated or a pointer to the message otherwise.
 */
const char *cargs_help_msg_get(Cargs *cargs);

/**
 * Gets a pointer to the CargsArgument with name 'name'.
 * @param name The name of the argument.
 * @param cargs Cargs handle.
 * @return Pointer the specified argument, NULL if none was found.
*/
CargsArgument *cargs_arg_get(const char *name, Cargs *cargs);

#ifdef __cplusplus
}
#endif

#endif // CARGS_H

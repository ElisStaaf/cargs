#include <cargs.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define UI unsigned int

static int is_number(const char *str)
{
    for(size_t i = 0; i < strlen(str); ++i)
    {
        if(!isdigit(str[i]))
            return 0;
    }

    return 1;
}

CargsArgument *cargs_arg_get(const char *name, Cargs *cargs)
{
    for(size_t i = 0; i < cargs->args_cnt; i++)
    {
        CargsArgument *tmp = &cargs->args[i];
        if(strcmp(tmp->name, name) == 0)
            return tmp;
    }
    return NULL;
}

void cargs_init(Cargs *cargs)
{
    cargs->args = NULL;
    cargs->args_cnt = 0;
    cargs->help_msg = NULL;
}

void cargs_destroy(Cargs *cargs)
{
    for(size_t i = 0; i < cargs->args_cnt; ++i)
    {
        free(cargs->args[i].value);
        free(cargs->args[i].name);
        free(cargs->args[i].arg_short);
        free(cargs->args[i].arg_long);
        free(cargs->args[i].description);
    }
    free(cargs->args);
    free(cargs->help_msg);
    cargs_init(cargs);
}

CargsAddStatus cargs_arg_add(
    const char *name,
    const char *arg_short,
    const char *arg_long,
    const char *description,
    int is_flag,
    Cargs *cargs
)
{
    if(!arg_short && !arg_long)
        return CARGS_ADD_INVALID_ARGS;
    
    for(size_t i = 0; i < cargs->args_cnt; ++i)
    {
        CargsArgument *tmp = &cargs->args[i];
        if(strcmp(name, tmp->name) == 0)
            return CARGS_ADD_NAME_DUP;
        if(arg_short && tmp->arg_short && strcmp(arg_short, tmp->arg_short) == 0)
            return CARGS_ADD_SHORT_DUP;
        if(arg_long && tmp->arg_long && strcmp(arg_long, tmp->arg_long) == 0)
            return CARGS_ADD_LONG_DUP;
    }

    /* make space for the new arg */
    cargs->args = realloc(
        cargs->args, 
        sizeof(CargsArgument) * (cargs->args_cnt + 1)
    );

    CargsArgument *head = &cargs->args[cargs->args_cnt];
    memset(head, 0, sizeof(CargsArgument));

    head->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(head->name, name);

    if(arg_short && strlen(arg_short) > 0)
    {
        head->arg_short = malloc(sizeof(char) * (strlen(arg_short) + 1));
        strcpy(head->arg_short, arg_short);
    }
    
    if(arg_long && strlen(arg_long) > 0)
    {
        head->arg_long = malloc(sizeof(char) * (strlen(arg_long) + 1));
        strcpy(head->arg_long, arg_long);
    }

    if(description&& strlen(description) > 0 )
    {
        head->description = malloc(sizeof(char) * (strlen(description) + 1));
        strcpy(head->description, description);
    }

    head->is_flag = is_flag;

    ++cargs->args_cnt;

    return CARGS_ADD_OK;
}

void cargs_args_parse(char **argv, int argc, Cargs *cargs)
{
    for(size_t args_i = 0; args_i < (size_t)argc; ++args_i)
    {
        for(size_t cargs_i = 0; cargs_i < cargs->args_cnt; ++cargs_i)
        {
            CargsArgument *tmp = &cargs->args[cargs_i];

            if(
                (tmp->arg_short && strcmp(tmp->arg_short, argv[args_i]) == 0) ||
                (tmp->arg_long && strcmp(tmp->arg_long, argv[args_i]) == 0)
            )
            {
                if(tmp->is_flag)
                {
                    /* Argument is a flag and isn't followed by a value. */
                    tmp->value = malloc(1);
                }
                else if(args_i + 1 < (size_t)argc)
                {
                    /* 
                     * Arg is supposed to have a following value.
                     */
                    tmp->value = malloc(sizeof(char) *  (strlen(argv[args_i + 1]) + 1));
                    strcpy(tmp->value, argv[args_i + 1]);

                    /* maybe increment args_i ? */
                    // ++args_i;
                }
                else
                {
                    printf("[cargs] Non flag argument: \"%s\" has no following value.\n", argv[args_i]);
                }
                break;
            }
        }
    }
}

CargsGetStatus cargs_arg_val_get_str(const char *name, char *out, Cargs *cargs)
{
    CargsArgument *arg = cargs_arg_get(name, cargs);
    if(!arg || !arg->value)
        return CARGS_GET_NOT_FOUND;

    strcpy(out, arg->value);
    return CARGS_GET_OK;
}

CargsGetStatus cargs_arg_val_get_uint(const char *name, UI *out, Cargs *cargs)
{
    CargsArgument *arg = cargs_arg_get(name, cargs);
    if(!arg || !arg->value)
        return CARGS_GET_NOT_FOUND;

    if(!is_number(arg->value))
        return CARGS_GET_TYPE_MISSMATCH;
        
    *out = (UI)atoll(arg->value);
    return CARGS_GET_OK;
}

CargsGetStatus cargs_arg_val_get_strlen(const char *name, size_t *out, Cargs *cargs)
{
    CargsArgument *arg = cargs_arg_get(name, cargs);
    if(!arg || !arg->value)
        return CARGS_GET_NOT_FOUND;

    if(arg->is_flag)
        return CARGS_GET_TYPE_MISSMATCH;

    *out = strlen(arg->value);
    return CARGS_GET_OK;
}

int cargs_arg_present(const char *name, Cargs *cargs)
{
    CargsArgument *arg = cargs_arg_get(name, cargs);
    if(!arg || !arg->value)
        return 0;

    return 1;
}

CargsHelpMsgGenStatus cargs_help_msg_gen(
    const char *usage, 
    const char *description,
    Cargs *cargs
)
{
    if(!cargs->args_cnt)
        return CARGS_HELP_MSG_GEN_INVALID_CARGS;

    size_t written = 0;
    size_t longest_arg_len = 0;

    if(strlen(usage) + strlen(description) + 3 > CARGS_HELP_MSG_LEN)
        return 2;

    cargs->help_msg = malloc(sizeof(char) * (CARGS_HELP_MSG_LEN + 1));

    written += sprintf(cargs->help_msg, "%s\n%s\n\n", usage, description);

    /* find the longest argument without description. */
    for(size_t i = 0; i < cargs->args_cnt; ++i)
    {
        const CargsArgument *tmp = &cargs->args[i];

        size_t len = 0;
        if(tmp->arg_short && tmp->arg_long)
        {
            len = strlen(tmp->arg_short) + strlen(tmp->arg_long) + 4;
        }
        else
        {
            len = strlen(tmp->arg_long ? tmp->arg_long : tmp->arg_short) + 2;
        }

        if(len > longest_arg_len)
            longest_arg_len = len;
    }

    /* minimum white-spaces between args and description. */
    longest_arg_len += 3;

    for(size_t i = 0; i < cargs->args_cnt; ++i)
    {
        const CargsArgument *tmp = &cargs->args[i];

        size_t len = 0;

        if(tmp->arg_short && tmp->arg_long)
        {
            len = strlen(tmp->arg_short) + strlen(tmp->arg_long) + 4;

            if(written + len > CARGS_HELP_MSG_LEN)
                goto die;
            
            written += sprintf(cargs->help_msg + written, "  %s, %s", tmp->arg_short, tmp->arg_long);
        }
        else
        {
            len = strlen(tmp->arg_long ? tmp->arg_long : tmp->arg_short) + 2;

            if(written + len > CARGS_HELP_MSG_LEN)
                goto die;
            
            written += sprintf(cargs->help_msg + written, "  %s", tmp->arg_long ? tmp->arg_long : tmp->arg_short);
        }

        /* space out arguments and their descriptions so they 
        all start on the same column. */
        for(size_t k = len; k < longest_arg_len; ++k)
        {
            if(++written > CARGS_HELP_MSG_LEN)
                goto die;
            strcat(cargs->help_msg, " ");  
        }

        if(tmp->description)
        {
            size_t desc_len = strlen(tmp->description);
            if(written + desc_len > CARGS_HELP_MSG_LEN)
                goto die;
            strcat(cargs->help_msg + written, tmp->description);
            written += desc_len;
        }
        else
        {
            if(written + 2 > CARGS_HELP_MSG_LEN)
                goto die;
            strcat(cargs->help_msg + written, "--");
            written += 2;
        }

        if(written + 1 > CARGS_HELP_MSG_LEN)
            goto die;
        strcat(cargs->help_msg + written, "\n");
        ++written;
    }

    return CARGS_HELP_MSG_GEN_OK;

die:
    free(cargs->help_msg);
    cargs->help_msg = NULL;
    return CARGS_HELP_MSG_GEN_BUF_LIMIT;
}

const char *cargs_help_msg_get(Cargs *cargs)
{
    return cargs->help_msg;
}

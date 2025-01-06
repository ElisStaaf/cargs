#pragma once

#include<string>
#include<sstream>

namespace cargs
{

namespace native
{
#include<cargs.h>
}

typedef struct native::S_CargsArgument  Argument;
typedef enum   native::E_CargsAddStatus AddStatus;
typedef enum   native::E_CargsGetStatus GetStatus;
typedef enum   native::E_CargsHelpMsgGenStatus HelpMsgGenStatus;

class Cargs
{
private:
    native::Cargs m_cargs_handle;

public:
    Cargs();
    ~Cargs();

    native::Cargs *get_native_cargs_handle();

    AddStatus arg_add(
        const std::string &name,
        const std::string &arg_short,
        const std::string &arg_long,
        const std::string &description,
        bool is_flag
    );

    void args_parse(char **argv, int argc);

    template<typename T>
    GetStatus arg_val_get(
        const std::string &name,
        T &out
    )
    {
        Argument *arg = native::cargs_arg_get(
            name.c_str(),
            &m_cargs_handle
        );
        if(!arg || !arg->value)
            return GetStatus::CARGS_GET_NOT_FOUND;

        if(arg->is_flag || !std::is_standard_layout<T>::value)
            return GetStatus::CARGS_GET_TYPE_MISSMATCH;

        std::stringstream ss(arg->value);
        T tmp;
        if(ss >> tmp)
        {
            out = tmp;
            return GetStatus::CARGS_GET_OK;
        }

        return GetStatus::CARGS_GET_TYPE_MISSMATCH;
    }

    Argument *arg_get(const std::string &name);

    bool arg_present(const std::string &name);

    HelpMsgGenStatus help_msg_gen(
        const std::string &usage,
        const std::string &description
    );

    std::string help_msg_get();
};

}

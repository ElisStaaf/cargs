#include <cargs.hpp>

namespace cargs
{

Cargs::Cargs()
{
    native::cargs_init(&m_cargs_handle);
}

Cargs::~Cargs()
{
    native::cargs_destroy(&m_cargs_handle);
}

native::Cargs* Cargs::get_native_cargs_handle()
{
    return &m_cargs_handle;
}

AddStatus Cargs::arg_add(
    const std::string &name,
    const std::string &arg_short,
    const std::string &arg_long,
    const std::string &description,
    bool is_flag
)
{
    return native::cargs_arg_add(
        name.c_str(),
        arg_short.c_str(),
        arg_long.c_str(),
        description.c_str(),
        is_flag,
        &m_cargs_handle
    );
}

void Cargs::args_parse(char **argv, int argc)
{
    native::cargs_args_parse(argv, argc, &m_cargs_handle);
}

bool Cargs::arg_present(const std::string &name)
{   
    return native::cargs_arg_present(
        name.c_str(),
        &m_cargs_handle
    );
}

Argument *Cargs::arg_get(const std::string &name)
{
    return native::cargs_arg_get(
        name.c_str(),
        &m_cargs_handle
    );
}

HelpMsgGenStatus Cargs::help_msg_gen(
    const std::string &usage,
    const std::string &description
)
{
    return native::cargs_help_msg_gen(
        usage.c_str(),
        description.c_str(),
        &m_cargs_handle
    );
}

std::string Cargs::help_msg_get()
{
    if(!m_cargs_handle.help_msg)
        return "";

    return std::string(m_cargs_handle.help_msg);
}


}

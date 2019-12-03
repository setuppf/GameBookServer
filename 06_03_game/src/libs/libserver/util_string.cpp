#include "util_string.h"

int strutil::stricmp(char const* c1, char const* c2)
{
    return engine_stricmp(c1, c2);
}

std::string strutil::format(const char* _format, ...)
{
    std::string str;
    va_list args1;
    va_start(args1, _format);

    va_list args2;
    va_copy(args2, args1);

    const size_t num_of_chars = std::vsnprintf(nullptr, 0, _format, args1);
    if (num_of_chars > str.capacity())
    {
        str.resize(num_of_chars + 1);
    }

    std::vsnprintf(const_cast<char*>(str.data()), str.capacity(), _format, args2);

    va_end(args1);
    va_end(args2);

    return str;
}

#pragma once

#include <string>
#include <vector>
#include <cstdarg>

#include "common.h"

namespace strutil
{
    int stricmp(char const* c1, char const* c2);

    std::string format(const char* _format, ...);

    inline std::string trim(const std::string& s) {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && isspace(*it))
            ++it;

        std::string::const_reverse_iterator rit = s.rbegin();
        while (rit.base() != it && isspace(*rit))
            ++rit;

        return std::string(it, rit.base());
    }

    // ×Ö·û´®Ìæ»»
    inline int replace(std::string& str, const std::string& pattern, const std::string& newpat) {
        int count = 0;
        const size_t nsize = newpat.size();
        const size_t psize = pattern.size();

        for (size_t pos = str.find(pattern, 0);
            pos != std::string::npos;
            pos = str.find(pattern, pos + nsize)) {
            str.replace(pos, psize, newpat);
            count++;
        }

        return count;
    }

    template<typename T>
    inline void split(const std::basic_string<T>& s, T c, std::vector< std::basic_string<T> >& v) {
        if (s.size() == 0)
            return;

        typename std::basic_string< T >::size_type i = 0;
        typename std::basic_string< T >::size_type j = s.find(c);

        while (j != std::basic_string<T>::npos) {
            std::basic_string<T> buf = s.substr(i, j - i);

            if (buf.size() > 0)
                v.push_back(buf);

            i = ++j; j = s.find(c, j);
        }

        if (j == std::basic_string<T>::npos) {
            std::basic_string<T> buf = s.substr(i, s.length() - i);
            if (buf.size() > 0)
                v.push_back(buf);
        }
    }
}

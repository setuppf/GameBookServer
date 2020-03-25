#pragma once

#include <string>
#include <vector>
#include <cstdarg>

#include "common.h"

namespace strutil {

    inline int stricmp( char const* c1, char const* c2 ) {
        return engine_stricmp( c1, c2 );
    }

    inline std::string format( const char * _format, ... ) {
        std::string str;
        va_list args1;
        va_start( args1, _format );

        va_list args2;
        va_copy( args2, args1 );

        const size_t num_of_chars = std::vsnprintf( nullptr, 0, _format, args1 );
        if ( num_of_chars > str.capacity( ) ) {
            str.resize( num_of_chars + 1 );
        }

        std::vsnprintf( const_cast<char *>( str.data( ) ), str.capacity( ), _format, args2 );

        va_end( args1 );
        va_end( args2 );

        return str;
    }

    inline std::string trim( const std::string &s ) {
        std::string::const_iterator it = s.begin( );
        while ( it != s.end( ) && isspace( *it ) )
            ++it;

        std::string::const_reverse_iterator rit = s.rbegin( );
        while ( rit.base( ) != it && isspace( *rit ) )
            ++rit;

        return std::string( it, rit.base( ) );
    }

    // ×Ö·û´®Ìæ»»
    inline int replace( std::string& str, const std::string& pattern, const std::string& newpat ) {
        int count = 0;
        const size_t nsize = newpat.size( );
        const size_t psize = pattern.size( );

        for ( size_t pos = str.find( pattern, 0 );
            pos != std::string::npos;
            pos = str.find( pattern, pos + nsize ) ) {
            str.replace( pos, psize, newpat );
            count++;
        }

        return count;
    }

    template<typename T>
    inline void split( const std::basic_string<T>& s, T c, std::vector< std::basic_string<T> > &v ) {
        if ( s.size( ) == 0 )
            return;

        typename std::basic_string< T >::size_type i = 0;
        typename std::basic_string< T >::size_type j = s.find( c );

        while ( j != std::basic_string<T>::npos ) {
            std::basic_string<T> buf = s.substr( i, j - i );

            if ( buf.size( ) > 0 )
                v.push_back( buf );

            i = ++j; j = s.find( c, j );
        }

        if ( j == std::basic_string<T>::npos ) {
            std::basic_string<T> buf = s.substr( i, s.length( ) - i );
            if ( buf.size( ) > 0 )
                v.push_back( buf );
        }
    }
}

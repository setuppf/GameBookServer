// TestArgs.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include <tuple>
#include <iostream>
#include <string>
#include <vector>

template<typename T>
int funcPrintf( T t ) {
    if ( typeid( T ) == typeid( int ) )
        std::cout << "int. value:" << (T)t <<std::endl;
    else
        std::cout << "string. value:" << (T)t <<std::endl;

    return 0;
}

template<typename T, typename...TArgs>
int funcPrintf( T t, TArgs...args ) {
    funcPrintf( t );
    funcPrintf( args... );
    return 0;
}

template<typename...TArgs>
int funcPrintf( TArgs...args ) {
    std::cout << "call func. param size:" << sizeof...( args ) << std::endl;
    return 0;
}


template <typename F1, size_t... I1, typename F2, size_t... I2>
void call( F1 f1, std::index_sequence<I1...>, F2 f2, std::index_sequence<I2...> ) {
    funcPrintf( f1( I1 )..., f2( I2 )... );
}

template <size_t N1, size_t N2, typename F1, typename F2>
void gen_call( F1 f1, F2 f2 ) {
    call( f1, std::make_index_sequence<N1>{}, f2, std::make_index_sequence<N2>{} );
}

template<int ICount, int SCount>
struct DynamicCall {
    static void HelpFun( const std::vector<int>& intargs, const std::vector<std::string>& strargs ) {
        if ( intargs.size( ) > ICount ) {
            return DynamicCall<ICount+1, SCount>( ).HelpFun( intargs, strargs );
        }

        if ( strargs.size( ) > SCount ) {
            return DynamicCall<ICount, SCount+1>( ).HelpFun( intargs, strargs );
        }

        gen_call<ICount, SCount>( [intargs]( size_t i ) { return intargs[i]; }, [strargs]( size_t i ) { return strargs[i]; } );
    }
};

template<int SCount>
struct DynamicCall<10, SCount> {
    static void HelpFun( const std::vector<int>& intargs, const std::vector<std::string>& strargs ) {
        return;
    }
};

template<int ICount>
struct DynamicCall<ICount, 10> {
    static void HelpFun( const std::vector<int>& intargs, const std::vector<std::string>& strargs ) {
        return;
    }
};

int main2( ) {

    std::vector<int> ints = { 1,2,3 };
    std::vector<std::string> strs = { "a", "b" };

    DynamicCall<0, 0>( ).HelpFun( ints, strs );

    return 0;
}
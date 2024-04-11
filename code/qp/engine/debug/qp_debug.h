#pragma once
#include "qp/common/utilities/qp_utility.h"
#include <iostream>
#include <utility>

class qpDebug {
public:
	static bool InitializeConsole();

	template< typename ... _args_ >
	static void Printf( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Log( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Info( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Warning( const char * format, _args_ &&... args );
	template< typename ... _args_ >
	static void Error( const char * format, _args_ &&... args );
private:
};

template< typename ... _args_ >
void qpDebug::Printf( const char * format, _args_ &&... args ) {
	std::cout << qpFormat( format, qpForward< _args_ >( args )... );
}

template< typename ... _args_ >
void qpDebug::Log( const char * format, _args_ &&... args ) {
	std::cout << qpFormat( format, qpForward< _args_ >( args )... ) << std::endl;
}

template< typename ... _args_ >
void qpDebug::Info( const char * format, _args_ &&... args ) {
	std::cout << "INFO: " << qpFormat(format, qpForward< _args_ >(args)...) << std::endl;
}

template< typename ... _args_ >
void qpDebug::Warning( const char * format, _args_ &&... args ) {
	std::cerr << "WARNING: " << qpFormat(format, qpForward< _args_ >(args)...) << std::endl;
}

template< typename ... _args_ >
void qpDebug::Error( const char * format, _args_ &&... args ) {
	std::cerr << "ERROR: " << qpFormat(format, qpForward< _args_ >(args)...) << std::endl;
}

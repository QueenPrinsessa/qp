#pragma once
#include "qp/common/utilities/qp_utility.h"
#include <iostream>
#include <utility>

class qpDebug {
public:
	static bool InitializeConsole();

	template< typename ... ARGS>
	static void Printf( const char * format, ARGS&&... args );
	template< typename ... ARGS>
	static void Log( const char * format, ARGS&&... args );
	template< typename ... ARGS>
	static void Info( const char * format, ARGS&&... args );
	template< typename ... ARGS>
	static void Warning( const char * format, ARGS&&... args );
	template< typename ... ARGS>
	static void Error( const char * format, ARGS&&... args );
private:
};

template< typename ... ARGS >
void qpDebug::Printf( const char * format, ARGS &&... args ) {
	std::cout << qpFormat( format, qpForward< ARGS >( args )... );
}

template< typename ... ARGS >
void qpDebug::Log( const char * format, ARGS &&... args ) {
	std::cout << qpFormat( format, qpForward< ARGS >( args )... ) << std::endl;
}

template< typename ... ARGS >
void qpDebug::Info( const char * format, ARGS &&... args ) {
	std::cout << "INFO: " << qpFormat(format, qpForward< ARGS >(args)...) << std::endl;
}

template< typename ... ARGS >
void qpDebug::Warning( const char * format, ARGS &&... args ) {
	std::cerr << "WARNING: " << qpFormat(format, qpForward< ARGS >(args)...) << std::endl;
}

template< typename ... ARGS >
void qpDebug::Error( const char * format, ARGS &&... args ) {
	std::cerr << "ERROR: " << qpFormat(format, qpForward< ARGS >(args)...) << std::endl;
}

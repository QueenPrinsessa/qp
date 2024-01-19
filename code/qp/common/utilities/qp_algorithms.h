#pragma once
#include "qp_utility.h"
#include "qp/common/debug/qp_debug.h"
#include <cstddef>
#include <cstring>

template < typename T >
T * qpBinarySearch( T * begin, T * end, const T & value ) {
	int left = 0;
	int right = static_cast< int >( end - begin );

	while( left <= right ) {
		int middle = ( left + right ) / 2;

		T * middlePtr = begin + middle;
		if( *middlePtr < value ) {
			left = ( middle + 1 );
		} else if( value < *middlePtr ) {
			right = ( middle - 1 );
		} else {
			return middlePtr;
		}
	}

	return end;
}

template < typename T >
int qpCopyUnchecked( T * to, const T * from, int num ) {
	if constexpr ( qpIsTrivialToCopy< T > ) {
		memcpy( to, from, num * sizeof( T ) );
	} else {
		for ( int index = 0; index < num; index++ ) {
			*to = *( from + index );
			to++;
		}
	}

	return num;
}

template < typename T >
int qpCopy( T * to, int toCount, const T * from, int fromCount ) {
	QP_ASSERT( toCount >= fromCount );

	if( toCount < fromCount ) {
		return 0;
	}

	if ( to == NULL || from == NULL ) {
		return 0;
	}

	return qpCopyUnchecked( to, from, fromCount );
}

template < typename T >
void qpSwap( T & a, T & b ) {
	const T temp = a;
	a = b;
	b = temp;
}

template < typename T >
void qpZeroMemory( T & memory ) {
	memset( &memory, 0, sizeof( T ) );
}

inline void qpZeroMemory( void * memory, int numBytes ) {
	memset( memory, 0, numBytes );
}

template < typename ... ARGS >
constexpr size_t qpSizeOfBiggestType_Internal() {
	size_t bytes = 0;
	( [ & ] () {
		if ( bytes < sizeof( ARGS ) ) {
			bytes = sizeof( ARGS );
		}
	}( ), ... );
	return bytes;
}

template < typename ... ARGS >
constexpr size_t qpSizeOfBiggestTypeValue = qpSizeOfBiggestType_Internal< ARGS... >();
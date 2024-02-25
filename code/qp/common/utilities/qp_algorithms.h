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
uint64 qpCopyUnchecked( T * to, const T * from, const uint64 num ) {
	if ( num == 0 ) {
		return num;
	}

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
uint64 qpCopy( T * to, uint64 toCount, const T * from, const uint64 fromCount ) {
	QP_ASSERT( toCount >= fromCount );

	if( toCount < fromCount ) {
		return 0;
	}

	if ( to == NULL || from == NULL ) {
		return 0;
	}

	return qpCopyUnchecked( to, from, fromCount );
}

static uint64 qpCopyBytesUnchecked( void * to, const void * from, const uint64 numBytes ) {
	if ( numBytes == 0 ) {
		return numBytes;
	}
	memcpy( to, from, numBytes );
	return numBytes;
}

static uint64 qpCopyBytes( void * to, uint64 toSizeBytes, const void * from, const uint64 numBytes ) {
	QP_ASSERT( toSizeBytes >= numBytes );

	if ( toSizeBytes < numBytes ) {
		return 0;
	}

	if ( to == NULL || from == NULL ) {
		return 0;
	}

	return qpCopyBytesUnchecked( to, from, numBytes );
}

static uint64 qpCopyBytesOverlappedUnchecked( void * to, const void * from, const uint64 numBytes ) {
	if ( numBytes == 0 ) {
		return numBytes;
	}
	memmove( to, from, numBytes );
	return numBytes;
}


static uint64 qpCopyBytesOverlapped( void * to, uint64 toSizeBytes, const void * from, const uint64 numBytes ) {
	QP_ASSERT( toSizeBytes >= numBytes );

	if ( toSizeBytes < numBytes ) {
		return 0;
	}

	if ( to == NULL || from == NULL ) {
		return 0;
	}

	return qpCopyBytesOverlappedUnchecked( to, from, numBytes );
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

template < typename TO, typename FROM >
static TO qpVerifyStaticCast( const FROM a ) {
	TO result = static_cast< TO >( a );
	QP_ASSERT_MSG( static_cast< FROM >( result ) == a, "Truncation resulted in loss of data" );
	return result;
}
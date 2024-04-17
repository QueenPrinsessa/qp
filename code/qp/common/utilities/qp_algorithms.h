#pragma once
#include "qp/common/core/qp_type_traits.h"
#include "qp/common/core/qp_types.h"
#include "qp/common/debug/qp_debug.h"
#include <cstddef>
#include <cstring>

template < typename _type_ >
_type_ * qpBinarySearch( _type_ * begin, _type_ * end, const _type_ & value ) {
	int left = 0;
	int right = static_cast< int >( end - begin );

	while( left <= right ) {
		int middle = ( left + right ) / 2;

		_type_ * middlePtr = begin + middle;
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

template < typename _type_ >
uint64 qpCopyUnchecked( _type_ * to, const _type_ * from, const uint64 num ) {
	if ( num == 0 ) {
		return num;
	}

	if constexpr ( IsTrivialToCopy< _type_ > ) {
		memcpy( to, from, num * sizeof( _type_ ) );
	} else {
		for ( int index = 0; index < num; index++ ) {
			*to = *( from + index );
			to++;
		}
	}

	return num;
}

template < typename _type_ >
uint64 qpCopy( _type_ * to, uint64 toCount, const _type_ * from, const uint64 fromCount ) {
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

template < typename _type_ >
void qpSwap( _type_ & a, _type_ & b ) {
	const _type_ temp = a;
	a = b;
	b = temp;
}

template < typename _type_ >
void qpZeroMemory( _type_ & memory ) {
	memset( &memory, 0, sizeof( _type_ ) );
}


inline void qpSetMemory( void * memory, int value, int numBytes ) {
	memset( memory, value, numBytes );
}

inline void qpZeroMemory( void * memory, int numBytes ) {
	memset( memory, 0, numBytes );
}

template < typename _type_, typename _from_ >
static _type_ qpVerifyStaticCast( const _from_ a ) {
	_type_ result = static_cast< _type_ >( a );
	QP_ASSERT_MSG( static_cast< _from_ >( result ) == a, "Truncation resulted in loss of data" );
	return result;
}
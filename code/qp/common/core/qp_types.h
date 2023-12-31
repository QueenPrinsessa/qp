#pragma once
#include <atomic>
#include <cstdint>

using int8 = signed char;
using int16 = signed short int;
using int32 = signed int ;
using int64 = long long int;
using uint8 = unsigned char;
using uint16 = unsigned short int;
using uint32 = unsigned int;
using uint64 = unsigned long long int;

using byte = unsigned char;

// Verify sizes.
static_assert( sizeof( int8 ) == 1, "sizeof int8 needs to be 1 byte." );
static_assert( sizeof( int16 ) == 2, "sizeof int16 needs to be 2 bytes." );
static_assert( sizeof( int32 ) == 4, "sizeof int32 needs to be 4 bytes." );
static_assert( sizeof( int64 ) == 8, "sizeof int64 needs to be 8 bytes." );
static_assert( sizeof( uint8 ) == 1, "sizeof uint8 needs to be 1 byte." );
static_assert( sizeof( uint16 ) == 2, "sizeof uint16 needs to be 2 bytes." );
static_assert( sizeof( uint32 ) == 4, "sizeof uint32 needs to be 4 bytes." );
static_assert( sizeof( uint64 ) == 8, "sizeof uint64 needs to be 8 bytes." );
static_assert( sizeof( byte ) == 1, "sizeof byte needs to be 1 byte." );

template< typename T >
using qpAtomic = std::atomic< T >;

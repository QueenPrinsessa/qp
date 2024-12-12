#pragma once
#include <atomic>

namespace qp {
	using int8 = signed char;
	using int16 = signed short int;
	using int32 = signed int;
	using int64 = long long int;
	using uint8 = unsigned char;
	using uint16 = unsigned short int;
	using uint32 = unsigned int;
	using uint64 = unsigned long long int;

	using byte = unsigned char;
	using nullptr_t = decltype( nullptr );

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

	template< typename _type_ >
	using atomic_t = std::atomic< _type_ >;
	using atomicBool_t = atomic_t< bool >;
	using atomicInt8_t = atomic_t< int8 >;
	using atomicUInt8_t = atomic_t< uint8 >;
	using atomicInt16_t = atomic_t< int16 >;
	using atomicUInt16_t = atomic_t< uint16 >;
	using atomicInt32_t = atomic_t< int32 >;
	using atomicUInt32_t = atomic_t< uint32 >;
	using atomicInt64_t = atomic_t< int64 >;
	using atomicUInt64_t = atomic_t< uint64 >;
}
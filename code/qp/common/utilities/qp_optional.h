#pragma once
#include "qp_utility.h"
#include <utility>

template < typename T >
class Optional {
public:
	Optional();
	Optional( const Optional< T > & other );
	Optional( Optional< T > && other ) noexcept;
	Optional( const T & other );
	Optional( T && other );
	~Optional();

	Optional< T > & operator=( const T & rhs );
	Optional< T > & operator=( const Optional< T > & rhs );
	Optional< T > & operator=( T && rhs );
	Optional< T > & operator=( Optional< T > && rhs ) noexcept;

	T & GetValue() & { return *reinterpret_cast< T * >( &m_value[ 0 ] ); }
	const T & GetValue() const & { return *reinterpret_cast< T * >( &m_value[ 0 ] ); }

	template < typename ... ARGS >
	void Emplace( ARGS... args );
	void Reset();

	bool HasValue() const { return m_hasValue; }
	operator bool() const { return m_hasValue; }
private:
	char m_value[ sizeof( T ) ] {};
	bool m_hasValue = false;
};

template< typename T >
Optional< T >::Optional() {
}

template< typename T >
Optional< T >::Optional( const Optional< T > & other ) {
	if( other.m_hasValue ) {
		Emplace( other.GetValue() );
	}
}

template< typename T >
Optional< T > & Optional< T >::operator=( const Optional< T > & rhs ) {
	if ( rhs.m_hasValue ) {
		Emplace( rhs.GetValue() );
	}
	return *this;
}

template< typename T >
Optional< T >::Optional( const T & other ) {
	Emplace( other );
}

template< typename T >
Optional< T >::Optional( T && other ) {
	Emplace( other );
}

template< typename T >
Optional<T>::Optional( Optional<T> && other ) noexcept {
	Reset();
	if ( other.m_hasValue ) {
		other.m_hasValue = false;
		memcpy( &m_value[ 0 ], &other.m_value[ 0 ], sizeof( T ) );
		m_hasValue = true;
	}
}

template< typename T >
Optional< T >::~Optional() {
	Reset();
}

template< typename T >
Optional< T > & Optional< T >::operator=( const T & rhs ) {
	Emplace( rhs );
	return *this;
}

template< typename T >
Optional< T > & Optional< T >::operator=( T && rhs ) {
	Emplace( rhs );
	return *this;
}

template< typename T >
Optional<T> & Optional<T>::operator=( Optional<T> && rhs ) noexcept {
	Reset();
	if( rhs.m_hasValue ) {
		m_hasValue = true;
		rhs.m_hasValue = false;
		memcpy( &m_value[ 0 ], &rhs.m_value[ 0 ], sizeof( T ) );
	}

	return *this;
}

template< typename T >
template< typename ... ARGS >
void Optional< T >::Emplace( ARGS... args ) {
	Reset();
	new ( &m_value[ 0 ] ) T( qpForward< ARGS >( args )... );
	m_hasValue = true;
}

template< typename T >
void Optional< T >::Reset() {
	if ( m_hasValue ) {
		reinterpret_cast< T * >( &m_value[ 0 ] )->~T();
		m_hasValue = false;
	}
}

template < typename T, typename ... ARGS >
Optional< T > qpCreateOptional( ARGS... args ) {
	return Optional< T >( qpMove( T( args ) ) );
}
#pragma once
#include "qp_utility.h"
#include <utility>

template < typename _type_ >
class Optional {
public:
	Optional();
	Optional( const Optional & other );
	Optional( Optional && other ) noexcept;
	Optional( const _type_ & other );
	Optional( _type_ && other );
	~Optional();

	Optional & operator=( const _type_ & rhs );
	Optional & operator=( const Optional & rhs );
	Optional & operator=( _type_ && rhs );
	Optional & operator=( Optional && rhs ) noexcept;

	_type_ & GetValue() & { return *reinterpret_cast< _type_ * >( &m_value[ 0 ] ); }
	const _type_ & GetValue() const & { return *reinterpret_cast< _type_ * >( &m_value[ 0 ] ); }

	template < typename ... _args_ >
	void Emplace( _args_... args );
	void Reset();

	bool HasValue() const { return m_hasValue; }
	operator bool() const { return m_hasValue; }
private:
	char m_value[ sizeof( _type_ ) ] {};
	bool m_hasValue = false;
};

template< typename _type_ >
Optional< _type_ >::Optional() {
}

template< typename _type_ >
Optional< _type_ >::Optional( const Optional & other ) {
	if( other.m_hasValue ) {
		Emplace( other.GetValue() );
	}
}

template< typename _type_ >
Optional< _type_ > & Optional< _type_ >::operator=( const Optional & rhs ) {
	if ( rhs.m_hasValue ) {
		Emplace( rhs.GetValue() );
	}
	return *this;
}

template< typename _type_ >
Optional< _type_ >::Optional( const _type_ & other ) {
	Emplace( other );
}

template< typename _type_ >
Optional< _type_ >::Optional( _type_ && other ) {
	Emplace( other );
}

template< typename _type_ >
Optional< _type_ >::Optional( Optional && other ) noexcept {
	Reset();
	if ( other.m_hasValue ) {
		other.m_hasValue = false;
		qpCopyBytesUnchecked( &m_value[ 0 ], &other.m_value[ 0 ], sizeof( _type_ ) );
		m_hasValue = true;
	}
}

template< typename _type_ >
Optional< _type_ >::~Optional() {
	Reset();
}

template< typename _type_ >
Optional< _type_ > & Optional< _type_ >::operator=( const _type_ & rhs ) {
	Emplace( rhs );
	return *this;
}

template< typename _type_ >
Optional< _type_ > & Optional< _type_ >::operator=( _type_ && rhs ) {
	Emplace( rhs );
	return *this;
}

template< typename _type_ >
Optional< _type_ > & Optional< _type_ >::operator=( Optional && rhs ) noexcept {
	Reset();
	if ( rhs.m_hasValue ) {
		m_hasValue = true;
		rhs.m_hasValue = false;
		qpCopyBytesUnchecked( &m_value[ 0 ], &rhs.m_value[ 0 ], sizeof( _type_ ) );
	}

	return *this;
}

template< typename _type_ >
template< typename ... _args_ >
void Optional< _type_ >::Emplace( _args_... args ) {
	Reset();
	new ( &m_value[ 0 ] ) _type_( qpForward< _args_ >( args )... );
	m_hasValue = true;
}

template< typename _type_ >
void Optional< _type_ >::Reset() {
	if ( m_hasValue ) {
		reinterpret_cast< _type_ * >( &m_value[ 0 ] )->~_type_();
		m_hasValue = false;
	}
}

template < typename _type_, typename ... _args_ >
Optional< _type_ > qpCreateOptional( _args_... args ) {
	return Optional< _type_ >( qpMove( _type_( args ) ) );
}
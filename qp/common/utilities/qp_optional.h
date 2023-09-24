#pragma once
#include <utility>

template < typename T >
class Optional
{
public:
	Optional();
	Optional( const T & other );
	Optional( T && other );
	~Optional();

	Optional< T > & operator=( const T & rhs );
	Optional< T > & operator=( T && rhs );

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
Optional<T>::Optional() {
}

template< typename T >
Optional<T>::Optional( const T & other ) {
	Emplace( other );
}

template< typename T >
Optional<T>::Optional( T && other ) {
	Emplace( other );
}

template< typename T >
Optional<T>::~Optional() {
	Reset();
}

template< typename T >
Optional<T> & Optional<T>::operator=( const T & rhs ) {
	Emplace( rhs );
	return *this;
}

template< typename T >
Optional<T> & Optional<T>::operator=( T && rhs ) {
	Emplace( rhs );
	return *this;
}

template< typename T >
template< typename ... ARGS >
void Optional<T>::Emplace( ARGS... args ) {
	Reset();
	new ( &m_value[ 0 ] ) T( std::forward< ARGS >( args )... );
	m_hasValue = true;
}

template< typename T >
void Optional<T>::Reset() {
	if ( m_hasValue ) {
		reinterpret_cast< T * >( &m_value[ 0 ] )->~T();
		m_hasValue = false;
	}
}

template < typename T, typename ... ARGS >
Optional< T > qpCreateOptional( ARGS... args ) {
	return Optional< T >( std::move( T( args ) ) );
}
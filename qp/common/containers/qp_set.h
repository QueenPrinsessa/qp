#pragma once
#include <set>
#include <utility>

template < typename T >
class qpSet {
public:
	qpSet();
	qpSet( std::initializer_list< T > initializerList );
	qpSet( const T * begin, const T * end );
	~qpSet();

	void Clear();

	template < typename ... ARGS >
	void Emplace( ARGS&&... args );
	void Erase( const T & value );

	typename std::set< T >::iterator Find( const T & value );

	bool IsEmpty() const { return m_set.empty(); }

	QP_ITERATORS( typename std::set< T >::iterator, m_set.begin(), m_set.end() )
private:
	std::set< T > m_set;
};

template< typename T >
qpSet< T >::qpSet() {
}

template< typename T >
qpSet< T >::qpSet( std::initializer_list< T > initializerList ) : qpSet( initializerList.begin(), initializerList.end() ) {
}

template< typename T >
qpSet< T >::qpSet( const T * begin, const T * end ) : m_set( begin, end ) {
}

template< typename T >
qpSet< T >::~qpSet() {
}

template< typename T >
void qpSet< T >::Clear() {
	m_set.clear();
}

template< typename T >
template< typename ... ARGS >
void qpSet< T >::Emplace( ARGS &&... args ) {
	m_set.emplace( qpForward< ARGS >( args )... );
}

template< typename T >
void qpSet< T >::Erase( const T & value ) {
	m_set.erase( value );
}

template< typename T >
typename std::set< T >::iterator qpSet< T >::Find( const T & value ) {
	return m_set.find( value );
}

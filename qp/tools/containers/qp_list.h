#pragma once
#include "qp/common/debug/qp_debug.h"
#include "qp/tools/math/qp_math.h"
#include <initializer_list>
#include <type_traits>
template< typename T >
class qpList {
public:
	qpList();
	qpList( int capacity );
	qpList( std::initializer_list< T > initializerList );

	void Push( const T & item );
	void Push( T && item );
	template < typename ... ARGS >
	T & Emplace( ARGS&&... args );
	void Pop();

	T & First();
	T & Last();

	T * Data() const { return m_data; }

	void ShrinkToFit();
	void Reserve( int capacity );
	void Resize( int length );
	void Clear();

	int Length() const { return m_length; }

	T & operator[]( int index );
	const T & operator[]( int index ) const;
private:
	int m_capacity = 0;
	int m_length = 0;
	T * m_data = NULL;
};

template< typename T >
qpList< T >::qpList() : qpList( 1 ) {
}

template< typename T >
qpList< T >::qpList( int capacity ) {
	Reserve( qpMath::Max( capacity, 1 ) );
}

template< typename T >
qpList< T >::qpList( std::initializer_list< T > initializerList ) : qpList( qpMath::Max( static_cast< int >( initializerList.size() ), 1 ) ) {
	m_length = initializerList.size();

	if constexpr ( std::is_trivially_copyable_v < T > ) {
		memcpy( m_data, initializerList.begin(), initializerList.size() * sizeof( T ) );
	} else {
		for ( auto it = initializerList.begin(); it != initializerList.end(); it++ ) {
			Push( *it );
		}
	}

}

template< typename T >
void qpList< T >::Push( const T & item ) {
	Emplace( item );
}

template< typename T >
void qpList< T >::Push( T && item ) {
	Emplace( item );
}

template< typename T >
template< typename ... ARGS >
T & qpList< T >::Emplace( ARGS &&... args ) {
	if ( ( m_length + 1 ) > m_capacity ) {
		Reserve( m_capacity * 2 );
	}

	return m_data[ m_length++ ] = std::move( T( std::forward<ARGS>( args )... ) );
}

template< typename T >
void qpList< T >::Pop() {
	if ( m_length > 0 ) {
		m_length--;
	}
}

template< typename T >
T & qpList< T >::First() {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename T >
T & qpList< T >::Last() {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename T >
void qpList< T >::ShrinkToFit() {
}

template< typename T >
void qpList< T >::Reserve( int capacity ) {
	if ( m_capacity < capacity ) {
		T * newData = new T[ capacity ] {};
		memcpy( newData, m_data, m_length * sizeof( T ) );
		delete m_data;
		m_data = newData;
		m_capacity = capacity;
	}
}

template< typename T >
void qpList< T >::Resize( int length ) {
	Reserve( length );
	int lengthDiff = m_length - length;

	if ( lengthDiff > 0 ) {
		for ( int index = 0; index < lengthDiff; index++ ) {
			m_data[ m_length + index ] = T();
		}
	} else if ( lengthDiff < 0 ) {
		for ( int index = 0; index < qpMath::Abs( lengthDiff ); index++ ) {
			m_data[ m_length - index ] = T();
		}
	}

	m_length = length;
}

template< typename T >
void qpList< T >::Clear() {
	m_length = 0;
}

template< typename T >
T & qpList< T >::operator[]( int index ) {
	QP_ASSERT_MSG( index >= 0 && index < m_length, "Index is out of bounds." );
	return m_data[ index ];
}

template< typename T >
const T & qpList< T >::operator[]( int index ) const {
	QP_ASSERT_MSG( index >= 0 && index < m_length, "Index is out of bounds." );
	return m_data[ index ];
}

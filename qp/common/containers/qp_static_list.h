#pragma once
#include "qp/common/core/qp_macros.h"
#include "qp/common/debug/qp_debug.h"
#include <iterator>

template < typename T, int SIZE >
class qpStaticList
{
public:
	QP_FORWARD_ITERATOR( Iterator, qpStaticList, T )

	qpStaticList();
	template < typename ... ARGS >
	explicit qpStaticList( ARGS&&... args ) requires ( sizeof ... ( ARGS ) <= SIZE );
	qpStaticList( const qpStaticList & other );

	void Push( const T & value );
	template< typename ... ARGS >
	void Emplace( ARGS&&... args );
	void Pop();

	T & First();
	T & Last();
	const T & First() const;
	const T & Last() const;

	T * Data() const { return m_data; }

	int Length() const { return m_length; }

	qpStaticList & operator=( const qpStaticList & other );

	T & operator[]( int index );
	const T & operator[] ( int index ) const;

	QP_ITERATORS( Iterator, Iterator( &m_data[ 0 ] ), Iterator( &m_data[ m_length ] ) )
private:
	T m_data[ SIZE ] {};
	int m_length = 0;
};

template< typename T, int SIZE >
qpStaticList< T, SIZE >::qpStaticList() {
}

template< typename T, int SIZE >
template< typename ... ARGS >
qpStaticList< T, SIZE >::qpStaticList( ARGS &&... args ) requires (sizeof...(ARGS) <= SIZE) {
	int index = 0;
	( [ & ] { m_data[ index++ ] = args; }(), ... );
}

template< typename T, int SIZE >
qpStaticList< T, SIZE >::qpStaticList( const qpStaticList & other ) {
	m_length = other.m_length;
	qpCopy( m_data, SIZE, other.m_data, other.m_length );
}

template< typename T, int SIZE >
void qpStaticList< T, SIZE >::Push( const T & value ) {
	Emplace( value );
}

template< typename T, int SIZE >
template< typename ... ARGS >
void qpStaticList< T, SIZE >::Emplace( ARGS &&... args ) {
	QP_ASSERT_MSG( m_length < SIZE, "List is already full!");
	m_data[ m_length++ ] = T( qpForward< ARGS >( args )... );
}

template< typename T, int SIZE >
void qpStaticList< T, SIZE >::Pop() {
	if ( m_length > 0 ) {
		m_length--;
	}
}

template< typename T, int SIZE >
T & qpStaticList< T, SIZE >::First() {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename T, int SIZE >
T & qpStaticList< T, SIZE >::Last() {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename T, int SIZE >
const T & qpStaticList< T, SIZE >::First() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename T, int SIZE >
const T & qpStaticList< T, SIZE >::Last() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename T, int SIZE >
qpStaticList< T, SIZE > & qpStaticList< T, SIZE >::operator=( const qpStaticList & other ) {
	m_length = other.m_length;
	qpCopy( m_data, SIZE, other.m_data, other.m_length );
	return *this;
}

template< typename T, int SIZE >
T & qpStaticList< T, SIZE >::operator[]( int index ) {
	QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
	QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
	return m_data[ index ];
}

template< typename T, int SIZE >
const T & qpStaticList< T, SIZE >::operator[]( int index ) const {
	QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
	QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
	return m_data[ index ];
}

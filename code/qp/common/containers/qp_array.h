#pragma once
#include "qp/common/core/qp_macros.h"
#include "qp/common/debug/qp_debug.h"
#include <cstddef>
#include <iterator>

template < typename T, int SIZE >
class qpArray {
public:
	static_assert( SIZE > 0, "qpArray: " QP_STRINGIFY( SIZE ) " needs to be greater than 0." );
	friend class qpArrayView< T >;
	QP_FORWARD_ITERATOR( Iterator, qpArray, T )

	qpArray();
	template < typename ... ARGS >
	explicit qpArray( ARGS&&... args ) requires ( sizeof ... ( ARGS ) <= SIZE );
	qpArray( const qpArray & other );

	T & First() { return m_data[ 0 ]; }
	T & Last() { return m_data[ ( SIZE - 1 ) ]; }
	const T & First() const { return m_data[ 0 ]; }
	const T & Last() const { return m_data[ ( SIZE - 1 ) ]; }

	int Length() const { return SIZE; }

	T * Data() { return &m_data[ 0 ]; }
	const T * Data() const { return &m_data[ 0 ]; }

	T & operator[]( int index );
	const T & operator[]( int index ) const;

	qpArray & operator=( const qpArray & other );

	QP_ITERATORS( Iterator, Iterator( &m_data[ 0 ] ), Iterator( &m_data[ SIZE ] ) )
private:
	T m_data[ SIZE ] {};
};

template< typename T, int SIZE >
qpArray< T, SIZE >::qpArray() {
}

template< typename T, int SIZE >
template< typename ... ARGS >
qpArray< T, SIZE >::qpArray( ARGS&&... args ) requires ( sizeof ... ( ARGS ) <= SIZE ) {
	int index = 0;
	( [ & ] { m_data[ index++ ] = static_cast< T >( args ); }(), ... );
}

template< typename T, int SIZE >
qpArray< T, SIZE >::qpArray( const qpArray & other ) {
	qpCopy( m_data, SIZE, other.m_data, SIZE );
}

template< typename T, int SIZE >
T & qpArray< T, SIZE >::operator[]( int index ) {
	QP_ASSERT_MSG( index >= 0 && index < SIZE, "Index is out of bounds." );
	return m_data[ index ];
}

template< typename T, int SIZE >
const T & qpArray< T, SIZE >::operator[]( int index ) const {
	QP_ASSERT_MSG( index >= 0 && index < SIZE, "Index is out of bounds." );
	return m_data[ index ];
}

template< typename T, int SIZE >
qpArray< T, SIZE > & qpArray< T, SIZE >::operator=( const qpArray & other ) {
	qpCopy( m_data, SIZE, other.m_data, SIZE );
	return *this;
}

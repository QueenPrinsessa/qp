#pragma once
#include "qp/common/debug/qp_debug.h"
#include "qp/common/math/qp_math.h"
#include "qp/common/utilities/qp_algorithms.h"
#include "qp_array_view.h"
#include <initializer_list>
#include <type_traits>

template< typename T >
class qpList {
	friend class qpArrayView< T >;
public:
	QP_FORWARD_ITERATOR( Iterator, qpList, T )

	qpList();
	qpList( int size );
	qpList( int size, const T & initValue );
	qpList( std::initializer_list< T > initializerList );
	qpList( const qpList & other );
	qpList( qpList && other ) noexcept;

	void Push( const T & item );
	void Push( T && item );
	template < typename ... ARGS >
	T & Emplace( ARGS&&... args );
	void Pop();

	T & First();
	T & Last();
	const T & First() const;
	const T & Last() const;

	T * Data() const { return m_data; }

	void ShrinkToFit();
	void Reserve( const uint64 capacity );
	void Resize( const uint64 length );
	void Clear();

	uint64 Length() const { return m_length; }
	bool IsEmpty() const { return m_length == 0; }

	T & operator[]( const uint64 index );
	const T & operator[]( const uint64 index ) const;

	qpList & operator=( const qpList & other );
	qpList & operator=( qpList && other ) noexcept;

	QP_ITERATORS( Iterator, Iterator( &m_data[ 0 ] ), Iterator( &m_data[ m_length ] ) )
private:
	uint64 m_capacity = 0;
	uint64 m_length = 0;
	T * m_data = NULL;
};

template< typename T >
qpList< T >::qpList() {
	Reserve( 1 );
}

template< typename T >
qpList< T >::qpList( int size ) {
	Resize( size );
}

template< typename T >
qpList<T>::qpList( int size, const T & initValue ) {
	Resize( size );
	for ( int index = 0; index < size; ++index ) {
		m_data[ index ] = initValue;
	}
}

template< typename T >
qpList< T >::qpList( std::initializer_list< T > initializerList ) {
	Reserve( static_cast< int >( initializerList.size() ) );
	m_length = qpCopy( m_data, static_cast< uint64 >( m_capacity ), initializerList.begin(), initializerList.size() );
}

template< typename T >
qpList<T>::qpList( const qpList & other ) {
	Reserve( other.m_length );
	qpCopy( m_data, m_capacity, other.m_data, other.m_length );
}

template< typename T >
qpList<T>::qpList( qpList && other ) noexcept {
	m_data = other.m_data;
	m_capacity = other.m_capacity;
	m_length = other.m_length;
	other.m_data = NULL;
	other.m_capacity = 0;
	other.m_length = 0;
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

	return m_data[ m_length++ ] = qpMove( T( qpForward<ARGS>( args )... ) );
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
const T & qpList< T >::First() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename T >
const T & qpList< T >::Last() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}


template< typename T >
void qpList< T >::ShrinkToFit() {
}

template< typename T >
void qpList< T >::Reserve( const uint64 capacity ) {
	if ( m_capacity < capacity ) {
		T * newData = new T[ capacity ] {};
		memcpy( newData, m_data, m_length * sizeof( T ) );
		delete[] m_data;
		m_data = newData;
		m_capacity = capacity;
	}
}

template< typename T >
void qpList< T >::Resize( const uint64 length ) {
	Reserve( length );
	int64 lengthDiff = qpVerifyStaticCast< int64 >( m_length ) - qpVerifyStaticCast< int64 >( length );

	if ( lengthDiff < 0 ) {
		for ( uint64 index = 0; index < qpVerifyStaticCast< uint64 >( qpMath::Abs( lengthDiff ) ); index++ ) {
			m_data[ m_length + index ] = T();
		}
	} else if ( lengthDiff > 0 ) {
		for ( uint64 index = 0; index < qpVerifyStaticCast< uint64 >( lengthDiff ); index++ ) {
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
T & qpList< T >::operator[]( const uint64 index ) {
	QP_ASSERT_MSG( index < m_length, "Index is out of bounds." );
	return m_data[ index ];
}

template< typename T >
const T & qpList< T >::operator[]( const uint64 index ) const {
	QP_ASSERT_MSG( index < m_length, "Index is out of bounds." );
	return m_data[ index ];
}

template< typename T >
qpList< T > & qpList< T >::operator=( const qpList & other ) {
	Reserve( other.m_length );
	qpCopy( m_data, m_capacity, other.m_data, other.m_length );
	return *this;
}

template< typename T >
qpList< T > & qpList< T >::operator=( qpList && other ) noexcept {
	m_data = other.m_data;
	m_capacity = other.m_capacity;
	m_length = other.m_length;
	other.m_data = NULL;
	other.m_capacity = 0;
	other.m_length = 0;
	return *this;
}

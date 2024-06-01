#pragma once
#include "qp/common/debug/qp_debug.h"
#include "qp/common/math/qp_math.h"
#include "qp/common/utilities/qp_algorithms.h"
#include "qp_array_view.h"
#include <initializer_list>
#include <type_traits>

template< typename _type_ >
class qpList {
	friend class qpArrayView< _type_ >;
public:
	QP_FORWARD_ITERATOR( Iterator, qpList, _type_ )

	qpList();
	qpList( int size );
	qpList( int size, const _type_ & initValue );
	qpList( std::initializer_list< _type_ > initializerList );
	qpList( const qpList & other );
	qpList( qpList && other ) noexcept;

	void Push( const _type_ & item );
	void Push( _type_ && item );
	template < typename ... _args_ >
	_type_ & Emplace( _args_ &&... args );
	void Pop();
	void PopFirst();

	_type_ & First();
	_type_ & Last();
	const _type_ & First() const;
	const _type_ & Last() const;

	_type_ * Data() const { return m_data; }

	void RemoveIndex( const uint64 index );

	void ShrinkToFit();
	void Reserve( const uint64 capacity );
	void Resize( const uint64 length );
	void Clear();

	uint64 Length() const { return m_length; }
	uint64 Capacity() const { return m_capacity; }
	bool IsEmpty() const { return m_length == 0; }

	_type_ & operator[]( const uint64 index );
	const _type_ & operator[]( const uint64 index ) const;

	qpList & operator=( const qpList & other );
	qpList & operator=( qpList && other ) noexcept;

	QP_ITERATORS( Iterator, Iterator( &m_data[ 0 ] ), Iterator( &m_data[ m_length ] ) )
private:
	uint64 m_capacity = 0;
	uint64 m_length = 0;
	_type_ * m_data = NULL;
};

template< typename _type_ >
qpList< _type_ >::qpList() {
	Reserve( 1 );
}

template< typename _type_ >
qpList< _type_ >::qpList( int size ) {
	Resize( size );
}

template< typename _type_ >
qpList<_type_>::qpList( int size, const _type_ & initValue ) {
	Resize( size );
	for ( int index = 0; index < size; ++index ) {
		m_data[ index ] = initValue;
	}
}

template< typename _type_ >
qpList< _type_ >::qpList( std::initializer_list< _type_ > initializerList ) {
	Reserve( static_cast< int >( initializerList.size() ) );
	m_length = qpCopy( m_data, static_cast< uint64 >( m_capacity ), initializerList.begin(), initializerList.size() );
}

template< typename _type_ >
qpList<_type_>::qpList( const qpList & other ) {
	Reserve( other.m_length );
	qpCopy( m_data, m_capacity, other.m_data, other.m_length );
}

template< typename _type_ >
qpList<_type_>::qpList( qpList && other ) noexcept {
	m_data = other.m_data;
	m_capacity = other.m_capacity;
	m_length = other.m_length;
	other.m_data = NULL;
	other.m_capacity = 0;
	other.m_length = 0;
}

template< typename _type_ >
void qpList< _type_ >::Push( const _type_ & item ) {
	Emplace( item );
}

template< typename _type_ >
void qpList< _type_ >::Push( _type_ && item ) {
	Emplace( item );
}

template< typename _type_ >
template< typename ... _args_ >
_type_ & qpList< _type_ >::Emplace( _args_ &&... args ) {
	if ( ( m_length + 1 ) > m_capacity ) {
		Reserve( m_capacity * 2 );
	}

	return m_data[ m_length++ ] = qpMove( _type_( qpForward< _args_ >( args )... ) );
}

template< typename _type_ >
void qpList< _type_ >::Pop() {
	if ( m_length > 0 ) {
		m_length--;
	}
}

template< typename _type_ >
void qpList<_type_>::PopFirst () {
	RemoveIndex( 0 );
}

template< typename _type_ >
_type_ & qpList< _type_ >::First() {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename _type_ >
_type_ & qpList< _type_ >::Last() {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename _type_ >
const _type_ & qpList< _type_ >::First() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename _type_ >
const _type_ & qpList< _type_ >::Last() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename _type_ >
void qpList<_type_>::RemoveIndex ( const uint64 index ) {
	if ( index >= m_length ) {
		return;
	}
	qpCopyOverlapped( &m_data[ index ], m_length - index, &m_data[ index + 1 ], m_length - index - 1);
	--m_length;
}

template< typename _type_ >
void qpList< _type_ >::ShrinkToFit() {
}

template< typename _type_ >
void qpList< _type_ >::Reserve( const uint64 capacity ) {
	if ( m_capacity < capacity ) {
		_type_ * newData = new _type_[ capacity ] {};
		qpCopy( newData, capacity, m_data, m_length );
		delete[] m_data;
		m_data = newData;
		m_capacity = capacity;
	}
}

template< typename _type_ >
void qpList< _type_ >::Resize( const uint64 length ) {
	Reserve( length );
	int64 lengthDiff = qpVerifyStaticCast< int64 >( m_length ) - qpVerifyStaticCast< int64 >( length );

	if ( lengthDiff < 0 ) {
		for ( uint64 index = 0; index < qpVerifyStaticCast< uint64 >( qpMath::Abs( lengthDiff ) ); index++ ) {
			m_data[ m_length + index ] = _type_();
		}
	} else if ( lengthDiff > 0 ) {
		for ( uint64 index = 0; index < qpVerifyStaticCast< uint64 >( lengthDiff ); index++ ) {
			m_data[ m_length - index ] = _type_();
		}
	}

	m_length = length;
}

template< typename _type_ >
void qpList< _type_ >::Clear() {
	m_length = 0;
}

template< typename _type_ >
_type_ & qpList< _type_ >::operator[]( const uint64 index ) {
	QP_ASSERT_MSG( index < m_length, "Index is out of bounds." );
	return m_data[ index ];
}

template< typename _type_ >
const _type_ & qpList< _type_ >::operator[]( const uint64 index ) const {
	QP_ASSERT_MSG( index < m_length, "Index is out of bounds." );
	return m_data[ index ];
}

template< typename _type_ >
qpList< _type_ > & qpList< _type_ >::operator=( const qpList & other ) {
	Reserve( other.m_length );
	qpCopy( m_data, m_capacity, other.m_data, other.m_length );
	return *this;
}

template< typename _type_ >
qpList< _type_ > & qpList< _type_ >::operator=( qpList && other ) noexcept {
	m_data = other.m_data;
	m_capacity = other.m_capacity;
	m_length = other.m_length;
	other.m_data = NULL;
	other.m_capacity = 0;
	other.m_length = 0;
	return *this;
}

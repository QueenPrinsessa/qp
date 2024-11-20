#pragma once
#include "qp/common/core/qp_macros.h"
#include "qp/common/debug/qp_debug.h"
#include <iterator>

template < typename _type_, int _size_ >
class qpStaticList
{
public:
	QP_FORWARD_ITERATOR( Iterator, qpStaticList, T )

	qpStaticList();
	template < typename ... _args_ >
	explicit qpStaticList( _args_ &&... args ) requires ( sizeof ... ( _args_ ) <= _size_ );
	qpStaticList( const qpStaticList & other );

	void Push( const _type_ & value );
	template< typename ... _args_ >
	void Emplace( _args_&&... args );
	void Pop();

	_type_ & First();
	_type_ & Last();
	const _type_ & First() const;
	const _type_ & Last() const;

	_type_ * Data() const { return m_data; }

	int Length() const { return m_length; }

	qpStaticList & operator=( const qpStaticList & other );

	_type_ & operator[]( int index );
	const _type_ & operator[] ( int index ) const;

	QP_ITERATORS( Iterator, Iterator( &m_data[ 0 ] ), Iterator( &m_data[ m_length ] ) )
private:
	_type_ m_data[ _size_ ] {};
	int m_length = 0;
};

template< typename _type_, int _size_ >
qpStaticList< _type_, _size_ >::qpStaticList() {
}

template< typename _type_, int _size_ >
template< typename ... _args_ >
qpStaticList< _type_, _size_ >::qpStaticList( _args_ &&... args ) requires ( sizeof...( _args_ ) <= _size_ ) {
	int index = 0;
	( [ & ] { m_data[ index++ ] = args; }(), ... );
}

template< typename _type_, int _size_ >
qpStaticList< _type_, _size_ >::qpStaticList( const qpStaticList & other ) {
	m_length = other.m_length;
	qpCopy( m_data, _size_, other.m_data, other.m_length );
}

template< typename _type_, int _size_ >
void qpStaticList< _type_, _size_ >::Push( const _type_ & value ) {
	Emplace( value );
}

template< typename _type_, int _size_ >
template< typename ... _args_ >
void qpStaticList< _type_, _size_ >::Emplace( _args_ &&... args ) {
	QP_ASSERT_MSG( m_length < _size_, "List is already full!");
	m_data[ m_length++ ] = T( qpForward< _args_ >( args )... );
}

template< typename _type_, int _size_ >
void qpStaticList< _type_, _size_ >::Pop() {
	if ( m_length > 0 ) {
		m_length--;
	}
}

template< typename _type_, int _size_ >
T & qpStaticList< _type_, _size_ >::First() {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename _type_, int _size_ >
T & qpStaticList< _type_, _size_ >::Last() {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename _type_, int _size_ >
const T & qpStaticList< _type_, _size_ >::First() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename _type_, int _size_ >
const T & qpStaticList< _type_, _size_ >::Last() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename _type_, int _size_ >
qpStaticList< _type_, _size_ > & qpStaticList< _type_, _size_ >::operator=( const qpStaticList & other ) {
	m_length = other.m_length;
	qpCopy( m_data, _size_, other.m_data, other.m_length );
	return *this;
}

template< typename _type_, int _size_ >
T & qpStaticList< _type_, _size_ >::operator[]( int index ) {
	QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
	QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
	return m_data[ index ];
}

template< typename _type_, int _size_ >
const T & qpStaticList< _type_, _size_ >::operator[]( int index ) const {
	QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
	QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
	return m_data[ index ];
}

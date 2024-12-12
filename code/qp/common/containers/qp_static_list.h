#pragma once
#include "qp/common/core/qp_macros.h"
#include "qp/common/debug/qp_debug.h"
#include <iterator>

namespace qp {
	template < typename _type_, int _size_ >
	class StaticList {
	public:
		QP_FORWARD_ITERATOR( Iterator, StaticList, _type_ )

		StaticList();
		template < typename ... _args_ >
		explicit StaticList( _args_ &&... args ) requires ( sizeof ... ( _args_ ) <= _size_ );
		StaticList( const StaticList & other );

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

		StaticList & operator=( const StaticList & other );

		_type_ & operator[]( int index );
		const _type_ & operator[] ( int index ) const;

		QP_ITERATORS( Iterator, Iterator( &m_data[ 0 ] ), Iterator( &m_data[ m_length ] ) )
	private:
		_type_ m_data[ _size_ ] {};
		int m_length = 0;
	};

	template< typename _type_, int _size_ >
	StaticList< _type_, _size_ >::StaticList() {
	}

	template< typename _type_, int _size_ >
	template< typename ... _args_ >
	StaticList< _type_, _size_ >::StaticList( _args_ &&... args ) requires ( sizeof...( _args_ ) <= _size_ ) {
		int index = 0;
		( [ & ] { m_data[ index++ ] = args; }( ), ... );
	}

	template< typename _type_, int _size_ >
	StaticList< _type_, _size_ >::StaticList( const StaticList & other ) {
		m_length = other.m_length;
		qpCopy( m_data, _size_, other.m_data, other.m_length );
	}

	template< typename _type_, int _size_ >
	void StaticList< _type_, _size_ >::Push( const _type_ & value ) {
		Emplace( value );
	}

	template< typename _type_, int _size_ >
	template< typename ... _args_ >
	void StaticList< _type_, _size_ >::Emplace( _args_ &&... args ) {
		QP_ASSERT_MSG( m_length < _size_, "List is already full!" );
		m_data[ m_length++ ] = T( qpForward< _args_ >( args )... );
	}

	template< typename _type_, int _size_ >
	void StaticList< _type_, _size_ >::Pop() {
		if ( m_length > 0 ) {
			m_length--;
		}
	}

	template< typename _type_, int _size_ >
	_type_ & StaticList< _type_, _size_ >::First() {
		QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
		return m_data[ 0 ];
	}

	template< typename _type_, int _size_ >
	_type_ & StaticList< _type_, _size_ >::Last() {
		QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
		return m_data[ m_length - 1 ];
	}

	template< typename _type_, int _size_ >
	const _type_ & StaticList< _type_, _size_ >::First() const {
		QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
		return m_data[ 0 ];
	}

	template< typename _type_, int _size_ >
	const _type_ & StaticList< _type_, _size_ >::Last() const {
		QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
		return m_data[ m_length - 1 ];
	}

	template< typename _type_, int _size_ >
	StaticList< _type_, _size_ > & StaticList< _type_, _size_ >::operator=( const StaticList & other ) {
		m_length = other.m_length;
		qpCopy( m_data, _size_, other.m_data, other.m_length );
		return *this;
	}

	template< typename _type_, int _size_ >
	_type_ & StaticList< _type_, _size_ >::operator[]( int index ) {
		QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
		QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
		return m_data[ index ];
	}

	template< typename _type_, int _size_ >
	const _type_ & StaticList< _type_, _size_ >::operator[]( int index ) const {
		QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
		QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
		return m_data[ index ];
	}
}
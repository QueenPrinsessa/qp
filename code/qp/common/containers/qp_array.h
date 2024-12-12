#pragma once
#include "qp/common/core/qp_macros.h"
#include "qp/common/debug/qp_debug.h"
#include <cstddef>
#include <iterator>

namespace qp {
	template < typename _type_ >
	class ArrayView;

	template < typename _type_, int _size_ >
	class Array {
	public:
		static_assert( _size_ > 0, "qpArray: " QP_STRINGIFY( SIZE ) " needs to be greater than 0." );
		friend class ArrayView< _type_ >;
		QP_FORWARD_ITERATOR( Iterator, Array, _type_ )

			Array();
		template < typename ... _args_ >
		explicit Array( _args_ &&... args ) requires ( sizeof ... ( _args_ ) <= _size_ );
		Array( const Array & other );

		_type_ & First() { return m_data[ 0 ]; }
		_type_ & Last() { return m_data[ ( _size_ - 1 ) ]; }
		const _type_ & First() const { return m_data[ 0 ]; }
		const _type_ & Last() const { return m_data[ ( _size_ - 1 ) ]; }

		int Length() const { return _size_; }

		_type_ * Data() { return &m_data[ 0 ]; }
		const _type_ * Data() const { return &m_data[ 0 ]; }

		_type_ & operator[]( int index );
		const _type_ & operator[]( int index ) const;

		Array & operator=( const Array & other );

		QP_ITERATORS( Iterator, Iterator( &m_data[ 0 ] ), Iterator( &m_data[ _size_ ] ) )
	private:
		_type_ m_data[ _size_ ] {};
	};

	template< typename _type_, int _size_ >
	Array< _type_, _size_ >::Array() {
	}

	template< typename _type_, int _size_ >
	template< typename ... _args_ >
	Array< _type_, _size_ >::Array( _args_&&... args ) requires ( sizeof ... ( _args_ ) <= _size_ ) {
		int index = 0;
		( [ & ] { m_data[ index++ ] = static_cast< _type_ >( args ); }( ), ... );
	}

	template< typename _type_, int _size_ >
	Array< _type_, _size_ >::Array( const Array & other ) {
		Copy( m_data, _size_, other.m_data, _size_ );
	}

	template< typename _type_, int _size_ >
	_type_ & Array< _type_, _size_ >::operator[]( int index ) {
		QP_ASSERT_MSG( index >= 0 && index < _size_, "Index is out of bounds." );
		return m_data[ index ];
	}

	template< typename _type_, int _size_ >
	const _type_ & Array< _type_, _size_ >::operator[]( int index ) const {
		QP_ASSERT_MSG( index >= 0 && index < _size_, "Index is out of bounds." );
		return m_data[ index ];
	}

	template< typename _type_, int _size_ >
	Array< _type_, _size_ > & Array< _type_, _size_ >::operator=( const Array & other ) {
		qpCopy( m_data, _size_, other.m_data, _size_ );
		return *this;
	}
}
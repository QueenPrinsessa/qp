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
	struct Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T *;
		using reference = T &;

		Iterator( pointer ptr ) : m_ptr( ptr ) {
		}

		reference operator *() const { return *m_ptr; }

		pointer operator->() { return m_ptr; }
		Iterator & operator++() {
			m_ptr++;
			return *this;
		}
		Iterator operator++( int ) {
			Iterator it = m_ptr++;
			return it;
		}

		operator pointer ( ) { return m_ptr; }

		auto operator<=>( const qpList< T >::Iterator & ) const = default;
		bool operator==( const qpList< T >::Iterator & ) const = default;
	private:
		pointer m_ptr = NULL;
	};

	qpList();
	qpList( int size );
	qpList( std::initializer_list< T > initializerList );

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
	void Reserve( int capacity );
	void Resize( int length );
	void Clear();

	int Length() const { return m_length; }
	bool IsEmpty() const { return m_length == 0; }

	T & operator[]( int index );
	const T & operator[]( int index ) const;

	Iterator Begin() { return Iterator( &m_data[ 0 ]); }
	Iterator End() { return Iterator( &m_data[ m_length ] ); }
	Iterator Begin() const { return Iterator( &m_data[ 0 ]); }
	Iterator End() const { return Iterator( &m_data[ m_length ] ); }
	Iterator begin() { return Begin(); }
	Iterator end() { return End(); }
	Iterator begin() const { return Begin(); }
	Iterator end() const { return End(); }
private:
	int m_capacity = 0;
	int m_length = 0;
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
qpList< T >::qpList( std::initializer_list< T > initializerList ) {
	Reserve( static_cast< int >( initializerList.size() ) );
	m_length = qpCopy( m_data, m_capacity, initializerList.begin(), static_cast< int >( initializerList.size() ) );
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
void qpList< T >::Reserve( int capacity ) {
	if ( m_capacity < capacity ) {
		T * newData = new T[ capacity ] {};
		memcpy( newData, m_data, m_length * sizeof( T ) );
		delete[] m_data;
		m_data = newData;
		m_capacity = capacity;
	}
}

template< typename T >
void qpList< T >::Resize( int length ) {
	Reserve( length );
	int lengthDiff = m_length - length;

	if ( lengthDiff < 0 ) {
		for ( int index = 0; index < qpMath::Abs( lengthDiff ); index++ ) {
			m_data[ m_length + index ] = T();
		}
	} else if ( lengthDiff > 0 ) {
		for ( int index = 0; index < lengthDiff; index++ ) {
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

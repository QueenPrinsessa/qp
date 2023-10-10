#pragma once
#include "qp/common/debug/qp_debug.h"
#include <iterator>

template < typename T, int SIZE >
class qpStaticList
{
public:
	struct Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T *;
		using reference = T &;

		Iterator( pointer ptr ) : m_ptr( ptr ) { }

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

		operator pointer () { return m_ptr; }

		auto operator<=>( const qpStaticList< T, SIZE >::Iterator & ) const = default;
		bool operator==( const qpStaticList< T, SIZE >::Iterator & ) const = default;
	private:
		pointer m_ptr = NULL;
	};

	qpStaticList();
	template < typename ... ARGS >
	explicit qpStaticList( ARGS&&... args ) requires ( sizeof ... ( ARGS ) <= SIZE );

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

	T & operator[]( int index );
	const T & operator[] ( int index ) const;

	Iterator Begin() { return Iterator( &m_data[ 0 ] ); }
	Iterator End() { return Iterator( &m_data[ m_length ] ); }
	Iterator Begin() const { return Iterator( &m_data[ 0 ] ); }
	Iterator End() const { return Iterator( &m_data[ m_length ] ); }
	Iterator begin() { return Begin(); }
	Iterator end() { return End(); }
	Iterator begin() const { return Begin(); }
	Iterator end() const { return End(); }
private:
	T m_data[ SIZE ] {};
	int m_length = 0;
};

template< typename T, int SIZE >
qpStaticList<T, SIZE>::qpStaticList() {
}

template< typename T, int SIZE >
template< typename ... ARGS >
qpStaticList<T, SIZE>::qpStaticList( ARGS &&... args ) requires (sizeof...(ARGS) <= SIZE) {
	int index = 0;
	( [ & ] { m_data[ index++ ] = args; }(), ... );
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
void qpStaticList<T, SIZE>::Pop() {
	if ( m_length > 0 ) {
		m_length--;
	}
}

template< typename T, int SIZE >
T & qpStaticList<T, SIZE>::First() {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename T, int SIZE >
T & qpStaticList<T, SIZE>::Last() {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename T, int SIZE >
const T & qpStaticList<T, SIZE>::First() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing first element but the list is empty." );
	return m_data[ 0 ];
}

template< typename T, int SIZE >
const T & qpStaticList<T, SIZE>::Last() const {
	QP_ASSERT_MSG( m_length != 0, "Accessing last element but the list is empty." );
	return m_data[ m_length - 1 ];
}

template< typename T, int SIZE >
T & qpStaticList<T, SIZE>::operator[]( int index ) {
	QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
	QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
	return m_data[ index ];
}

template< typename T, int SIZE >
const T & qpStaticList<T, SIZE>::operator[]( int index ) const {
	QP_ASSERT_MSG( index >= 0, "Index out of bounds!" );
	QP_ASSERT_MSG( index >= m_length, "Index out of bounds!" );
	return m_data[ index ];
}

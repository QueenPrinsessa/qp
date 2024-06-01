#pragma once
#include "common/containers/qp_list.h"

// a dynamic circular queue
template < typename _type_ >
class qpQueue {
public:
	qpQueue() = default;
	qpQueue( const uint64 initialCapacity );
	void Push( const _type_ & item );
	void Push( _type_ && item );
	template < typename ... _args_ >
	_type_ & Emplace( _args_ &&... args );
	const _type_ & Peek() const;
	bool Pop( _type_ & outItem );
	bool Pop();

	void Reserve( const uint64 newCapacity );

	uint64 Length() const;

	bool IsEmpty() const { return m_tail == m_head; }
	bool IsFull() const { return m_tail == m_items.Length(); }
private:
	qpList< _type_ > m_items;
	uint64 m_head = 0;
	uint64 m_tail = 0;
	
};

template< typename _type_ >
qpQueue< _type_ >::qpQueue ( const uint64 initialCapacity ) 
	: m_items( initialCapacity ) {}

template< typename _type_ >
void qpQueue< _type_ >::Push( const _type_ & item ) {
	Emplace( item );
}

template< typename _type_ >
void qpQueue< _type_ >::Push( _type_ && item ) {
	Emplace( qpMove( item ) );
}

template< typename _type_ >
const _type_ & qpQueue< _type_ >::Peek() const {
	QP_ASSERT_RELEASE_MSG( m_tail != m_head, "Peeking empty queue!");
	return m_items[ m_head ];
}

template< typename _type_ >
bool qpQueue< _type_ >::Pop ( _type_ & outItem ) {
	const uint64 index = m_head;
	if ( Pop() ) {
		outItem = qpMove( m_items[ index ] );
		return true;
	}
	return false;
}

template< typename _type_ >
bool qpQueue< _type_ >::Pop() {
	if ( m_tail == m_head ) {
		return false;
	}
	if ( m_tail == m_items.Length() ) {
		m_tail = m_head;
	}
	m_head = ( m_head + 1 ) % m_items.Length();
	return true;
}

template< typename _type_ >
void qpQueue< _type_ >::Reserve( const uint64 newCapacity ) {
	m_items.Reserve( newCapacity );
}

template< typename _type_ >
uint64 qpQueue<_type_>::Length () const {
	if ( m_tail == m_head ) {
		return 0;
	}
	if ( m_tail == m_items.Length() ) {
		return m_items.Length();
	}
	if ( m_tail > m_head ) {
		return m_tail - m_head;
	}
	return m_tail + m_items.Length() - m_head;
}

template< typename _type_ >
template< typename ... _args_ >
_type_ & qpQueue< _type_ >::Emplace ( _args_ &&... args ) {
	if ( m_tail == m_items.Length() ) {
		++m_tail;
		return m_items.Emplace( qpForward< _args_ >( args )... );
	}

	const uint64 insertIndex = m_tail;
	m_items[ insertIndex ] = qpMove( _type_( qpForward< _args_ >( args )... ) );
	m_tail = ( m_tail + 1 ) % m_items.Length();
	return m_items[ insertIndex ];
}

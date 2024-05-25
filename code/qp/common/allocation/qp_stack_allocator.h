#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/debug/qp_debug.h"

class qpStackAllocator {
public:
	qpStackAllocator( const uint64 capacity )
		: m_capacity( capacity ) {
		m_memory = new byte[ m_capacity ];
	}
	~qpStackAllocator() {
		QP_ASSERT( m_bytesAllocated == 0 );
		delete[] m_memory;
	}

	template < typename _type_, typename ... _args_ >
	const _type_ * Alloc( _args_&& ... args ) {
		QP_ASSERT( ( m_memory + m_bytesAllocated + sizeof( _type_ ) ) <= ( m_memory + m_capacity ) );
		const _type_ * data = new ( m_memory + m_bytesAllocated ) _type_( qpForward< _args_ >( args )...);
		m_bytesAllocated += sizeof( _type_ );
		return data;
	}

	template < typename _type_ >
	void Free() {
		m_bytesAllocated -= sizeof( _type_ );
		QP_ASSERT( ( m_memory + m_bytesAllocated ) >= m_memory );
		reinterpret_cast< _type_ * >( m_memory + m_bytesAllocated )->~_type_();
	}

	uint64 Capacity() const { return m_capacity; }
	uint64 BytesAllocated() const { return m_bytesAllocated; }

private:
	byte * m_memory = NULL;
	uint64 m_bytesAllocated = 0;
	uint64 m_capacity = 0;
};
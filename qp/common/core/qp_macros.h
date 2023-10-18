#pragma once

#define QP_UNUSED_PARAMETER( x ) x;
#define QP_STRINGIFY( x ) #x
#define QP_BIT( x ) ( 1u << ( x ) )

#define QP_NO_DISCARD [[ nodiscard ]]
#define QP_INLINE inline
#define QP_FORCE_INLINE __forceinline

#define QP_FORWARD_ITERATOR( iteratorType, type, containerType ) \
	struct iteratorType { \
	public: \
		using iterator_category = std::forward_iterator_tag; \
		using difference_type = std::ptrdiff_t; \
		using value_type = T; \
		using pointer = T *; \
		using reference = T &; \
 \
		iteratorType( pointer ptr ) : m_ptr( ptr ) { } \
 \
		reference operator *() const { return *m_ptr; } \
 \
		pointer operator->() { return m_ptr; } \
		Iterator & operator++() { \
			m_ptr++; \
			return *this; \
		} \
		Iterator operator++( int ) { \
			Iterator it = m_ptr++; \
			return it; \
		} \
 \
		operator pointer () { return m_ptr; } \
 \
		auto operator<=>( const type::iteratorType & ) const = default; \
		bool operator==( const type::iteratorType & ) const = default; \
	private: \
		pointer m_ptr = NULL; \
	};

#define QP_ITERATORS( type, beginImpl, endImpl ) \
		type Begin() { return beginImpl; } \
		type End() { return endImpl; } \
		type Begin() const { return beginImpl; } \
		type End() const { return endImpl; } \
		type begin() { return Begin(); } \
		type end() { return End(); } \
		type begin() const { return Begin(); } \
		type end() const { return End(); } 
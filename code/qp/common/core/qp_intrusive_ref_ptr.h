#pragma once
#include "qp_types.h"
#include "qp/common/utilities/qp_utility.h"

#define QP_INTRUSIVE_INCREMENT_REF INTRUSIVE_ÎncrementRef
#define QP_INTRUSIVE_DECREMENT_REF INTRUSIVE_DecrementRef
#define QP_INTRUSIVE_GET_COUNTER INTRUSIVE_GetCounter
#define QP_INTRUSIVE_COUNTER_MEMBER m_intrusiveRefCounter

#define QP_INTRUSIVE_REF_COUNTER \
public: \
	void QP_INTRUSIVE_INCREMENT_REF() const { QP_INTRUSIVE_COUNTER_MEMBER++; } \
	void QP_INTRUSIVE_DECREMENT_REF() const { QP_INTRUSIVE_COUNTER_MEMBER--; } \
	uint32 QP_INTRUSIVE_GET_COUNTER() const { return QP_INTRUSIVE_COUNTER_MEMBER; } \
private: \
	mutable qpAtomic< uint32 > QP_INTRUSIVE_COUNTER_MEMBER = 0

template< typename T >
constexpr bool qpHasIntrusiveRefCounter = (
	requires ( const T & t ) {
	t.QP_INTRUSIVE_INCREMENT_REF();
} && requires ( const T & t ) {
	t.QP_INTRUSIVE_DECREMENT_REF();
} && requires ( const T & t ) {
	t.QP_INTRUSIVE_GET_COUNTER();
} );

template < typename T > requires qpHasIntrusiveRefCounter< T >
class qpIntrusiveRefPtr {
public:
	qpIntrusiveRefPtr();
	explicit qpIntrusiveRefPtr( T * ptr );
	qpIntrusiveRefPtr( nullptr_t );
	qpIntrusiveRefPtr( const qpIntrusiveRefPtr< T > & other );
	qpIntrusiveRefPtr( qpIntrusiveRefPtr< T > && other ) noexcept;
	template< typename D >
	qpIntrusiveRefPtr( const qpIntrusiveRefPtr< D > & other ) requires ( qpIsBaseOf< T, D > );
	template< typename D >
	qpIntrusiveRefPtr( qpIntrusiveRefPtr< D > && other ) noexcept requires ( qpIsBaseOf< T, D > );
	~qpIntrusiveRefPtr();

	qpIntrusiveRefPtr< T > & operator=( const qpIntrusiveRefPtr< T > & rhs );
	qpIntrusiveRefPtr< T > & operator=( qpIntrusiveRefPtr< T > && rhs ) noexcept;
	template< typename D >
	qpIntrusiveRefPtr< T > & operator=( const qpIntrusiveRefPtr< D > & rhs ) requires ( qpIsBaseOf< T, D > );
	template< typename D >
	qpIntrusiveRefPtr< T > & operator=( qpIntrusiveRefPtr< D > && rhs ) noexcept requires ( std::is_base_of_v< T, D > );
	qpIntrusiveRefPtr< T > & operator=( nullptr_t null );

	auto operator<=>( const qpIntrusiveRefPtr< T > & rhs ) const;
	auto operator<=>( const T * rhs ) const;
	bool operator==( const qpIntrusiveRefPtr< T > & rhs ) const;
	bool operator==( const T * rhs ) const;

	T * operator->() { return m_ptr; }
	const T * operator->() const { return m_ptr; }
	T & operator*() { return *m_ptr; }
	const T & operator*() const { return *m_ptr; }

	T * Raw() const { return m_ptr; }

	void Reset();
	void Reset( T * data );

	uint32 GetRefCount() const { return m_ptr ? m_ptr->QP_INTRUSIVE_GET_COUNTER() : 0; }

private:
	void IncrementRef();
	void DecrementRef();

	T * m_ptr = NULL;
};

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T >::qpIntrusiveRefPtr() {
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T >::qpIntrusiveRefPtr( T * ptr ) {
	m_ptr = ptr;
	IncrementRef();
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T >::qpIntrusiveRefPtr( nullptr_t null ) {
	m_ptr = null;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T >::qpIntrusiveRefPtr( const qpIntrusiveRefPtr< T > & other ) {
	m_ptr = other.m_ptr;
	IncrementRef();
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T >::qpIntrusiveRefPtr( qpIntrusiveRefPtr< T > && other ) noexcept {
	m_ptr = other.m_ptr;
	other.m_ptr = NULL;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
template< typename D >
qpIntrusiveRefPtr< T >::qpIntrusiveRefPtr( const qpIntrusiveRefPtr< D > & other ) requires ( qpIsBaseOf< T, D >) {
	m_ptr = other.Raw();
	IncrementRef();
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
template< typename D >
qpIntrusiveRefPtr< T >::qpIntrusiveRefPtr( qpIntrusiveRefPtr< D > && other ) noexcept requires ( qpIsBaseOf< T, D >) {
	m_ptr = other.Raw();
	IncrementRef();
	other.Reset( NULL );
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T >::~qpIntrusiveRefPtr() {
	DecrementRef();
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T > & qpIntrusiveRefPtr< T >::operator=( const qpIntrusiveRefPtr< T > & rhs ) {
	m_ptr = rhs.m_ptr;
	IncrementRef();
	return *this;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T > & qpIntrusiveRefPtr< T >::operator=( qpIntrusiveRefPtr< T > && rhs ) noexcept {
	m_ptr = rhs.m_ptr;
	rhs.m_ptr = NULL;
	return *this;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
qpIntrusiveRefPtr< T > & qpIntrusiveRefPtr< T >::operator=( nullptr_t null ) {
	DecrementRef();
	m_ptr = null;
	return *this;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
template< typename D >
qpIntrusiveRefPtr< T > & qpIntrusiveRefPtr< T >::operator=( const qpIntrusiveRefPtr <D > & rhs ) requires ( qpIsBaseOf< T, D > ) {
	m_ptr = rhs.Raw();
	IncrementRef();
	return *this;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
template< typename D >
qpIntrusiveRefPtr< T > & qpIntrusiveRefPtr< T >::operator=( qpIntrusiveRefPtr< D > && rhs ) noexcept requires ( std::is_base_of_v< T, D > ) {
	m_ptr = rhs.Raw();
	IncrementRef();
	rhs.Reset( NULL );
	return *this;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
auto qpIntrusiveRefPtr< T >::operator<=>( const qpIntrusiveRefPtr< T > & rhs ) const {
	return ( m_ptr - rhs.m_ptr );
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
auto qpIntrusiveRefPtr< T >::operator<=>( const T * rhs ) const {
	return ( m_ptr - rhs );
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
bool qpIntrusiveRefPtr< T >::operator==( const qpIntrusiveRefPtr< T > & rhs ) const {
	return m_ptr == rhs.m_ptr;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
bool qpIntrusiveRefPtr< T >::operator==( const T * rhs ) const {
	return m_ptr == rhs;
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
void qpIntrusiveRefPtr< T >::Reset() {
	Reset( NULL );
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
void qpIntrusiveRefPtr< T >::Reset( T * data ) {
	DecrementRef();
	m_ptr = data;
	IncrementRef();
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
void qpIntrusiveRefPtr< T >::IncrementRef() {
	if ( m_ptr ) {
		m_ptr->QP_INTRUSIVE_INCREMENT_REF();
	}
}

template< typename T > requires qpHasIntrusiveRefCounter< T >
void qpIntrusiveRefPtr< T >::DecrementRef() {
	if ( m_ptr ) {
		m_ptr->QP_INTRUSIVE_DECREMENT_REF();

		if ( m_ptr->QP_INTRUSIVE_GET_COUNTER() == 0 ) {
			delete m_ptr;
			m_ptr = NULL;
		}
	}
}

template < typename T, typename ... ARGS >
constexpr static inline qpIntrusiveRefPtr< T > qpCreateIntrusiveRef( ARGS&&... args ) {
	return qpIntrusiveRefPtr< T >( new T( qpForward< ARGS >( args )... ) );
}

template < typename T, typename D >
constexpr static inline qpIntrusiveRefPtr< T > qpIntrusiveRefStaticCast( const qpIntrusiveRefPtr< D > & intrusivePtr ) {
	return qpIntrusiveRefPtr< T >( static_cast< T * >( intrusivePtr.Raw() ) );
}

template < typename T, typename D >
constexpr static inline qpIntrusiveRefPtr< T > qpIntrusiveRefDynamicCast( const qpIntrusiveRefPtr< D > & intrusivePtr ) {
	return qpIntrusiveRefPtr< T >( dynamic_cast< T * >( intrusivePtr.Raw() ) );
}

template < typename T, typename D >
constexpr static inline qpIntrusiveRefPtr< T > qpIntrusiveRefReinterpretCast( const qpIntrusiveRefPtr< D > & intrusivePtr ) {
	return qpIntrusiveRefPtr< T >( reinterpret_cast< T * >( intrusivePtr.Raw() ) );
}

template < typename T, typename D >
constexpr static inline qpIntrusiveRefPtr< T > qpIntrusiveRefConstCast( const qpIntrusiveRefPtr< D > & intrusivePtr ) {
	return qpIntrusiveRefPtr< T >( const_cast< T * >( intrusivePtr.Raw() ) );
}
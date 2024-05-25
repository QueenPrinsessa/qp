#pragma once
#include "qp_types.h"
#include "qp/common/utilities/qp_utility.h"

#define QP_INTRUSIVE_INCREMENT_REF INTRUSIVE_IncrementRef
#define QP_INTRUSIVE_DECREMENT_REF INTRUSIVE_DecrementRef
#define QP_INTRUSIVE_GET_COUNTER INTRUSIVE_GetCounter
#define QP_INTRUSIVE_COUNTER_MEMBER m_intrusiveRefCounter

#define QP_INTRUSIVE_REF_COUNTER \
public: \
	void QP_INTRUSIVE_INCREMENT_REF() const { QP_INTRUSIVE_COUNTER_MEMBER++; } \
	void QP_INTRUSIVE_DECREMENT_REF() const { QP_INTRUSIVE_COUNTER_MEMBER--; } \
	uint32 QP_INTRUSIVE_GET_COUNTER() const { return QP_INTRUSIVE_COUNTER_MEMBER; } \
private: \
	mutable atomic_t< uint32 > QP_INTRUSIVE_COUNTER_MEMBER = 0

template< typename _type_ >
constexpr bool qpHasIntrusiveRefCounter = (
	requires ( const _type_ & t ) {
	t.QP_INTRUSIVE_INCREMENT_REF();
} && requires ( const _type_ & t ) {
	t.QP_INTRUSIVE_DECREMENT_REF();
} && requires ( const _type_ & t ) {
	t.QP_INTRUSIVE_GET_COUNTER();
} );

template < typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
class qpIntrusiveRefPtr {
public:
	qpIntrusiveRefPtr();
	explicit qpIntrusiveRefPtr( _type_ * ptr );
	qpIntrusiveRefPtr( nullptr_t );
	qpIntrusiveRefPtr( const qpIntrusiveRefPtr & other );
	qpIntrusiveRefPtr( qpIntrusiveRefPtr && other ) noexcept;
	template< typename _derived_ >
	qpIntrusiveRefPtr( const qpIntrusiveRefPtr< _derived_ > & other ) requires ( IsBaseOf< _type_, _derived_ > );
	template< typename _derived_ >
	qpIntrusiveRefPtr( qpIntrusiveRefPtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ > );
	~qpIntrusiveRefPtr();

	qpIntrusiveRefPtr & operator=( const qpIntrusiveRefPtr & rhs );
	qpIntrusiveRefPtr & operator=( qpIntrusiveRefPtr && rhs ) noexcept;
	template< typename _derived_ >
	qpIntrusiveRefPtr & operator=( const qpIntrusiveRefPtr< _derived_ > & rhs ) requires ( IsBaseOf< _type_, _derived_ > );
	template< typename _derived_ >
	qpIntrusiveRefPtr & operator=( qpIntrusiveRefPtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > );
	qpIntrusiveRefPtr & operator=( nullptr_t null );

	auto operator<=>( const qpIntrusiveRefPtr & rhs ) const;
	auto operator<=>( const _type_ * rhs ) const;
	bool operator==( const qpIntrusiveRefPtr & rhs ) const;
	bool operator==( const _type_ * rhs ) const;

	_type_ * operator->() { return m_ptr; }
	const _type_ * operator->() const { return m_ptr; }
	_type_ & operator*() { return *m_ptr; }
	const _type_ & operator*() const { return *m_ptr; }

	_type_ * Raw() const { return m_ptr; }

	void Reset();
	void Reset( _type_ * data );

	uint32 GetRefCount() const { return m_ptr ? m_ptr->QP_INTRUSIVE_GET_COUNTER() : 0; }

private:
	void IncrementRef();
	void DecrementRef();

	_type_ * m_ptr = NULL;
};

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ >::qpIntrusiveRefPtr() {
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ >::qpIntrusiveRefPtr( _type_ * ptr ) {
	m_ptr = ptr;
	IncrementRef();
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ >::qpIntrusiveRefPtr( nullptr_t null ) {
	m_ptr = null;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ >::qpIntrusiveRefPtr( const qpIntrusiveRefPtr & other ) {
	m_ptr = other.m_ptr;
	IncrementRef();
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ >::qpIntrusiveRefPtr( qpIntrusiveRefPtr && other ) noexcept {
	m_ptr = other.m_ptr;
	other.m_ptr = NULL;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
template< typename _derived_ >
qpIntrusiveRefPtr< _type_ >::qpIntrusiveRefPtr( const qpIntrusiveRefPtr< _derived_ > & other ) requires ( IsBaseOf< _type_, _derived_ >) {
	m_ptr = other.Raw();
	IncrementRef();
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
template< typename _derived_ >
qpIntrusiveRefPtr< _type_ >::qpIntrusiveRefPtr( qpIntrusiveRefPtr< _derived_ > && other ) noexcept requires ( IsBaseOf< _type_, _derived_ >) {
	m_ptr = other.Raw();
	IncrementRef();
	other.Reset( NULL );
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ >::~qpIntrusiveRefPtr() {
	DecrementRef();
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ > & qpIntrusiveRefPtr< _type_ >::operator=( const qpIntrusiveRefPtr & rhs ) {
	m_ptr = rhs.m_ptr;
	IncrementRef();
	return *this;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ > & qpIntrusiveRefPtr< _type_ >::operator=( qpIntrusiveRefPtr && rhs ) noexcept {
	m_ptr = rhs.m_ptr;
	rhs.m_ptr = NULL;
	return *this;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
qpIntrusiveRefPtr< _type_ > & qpIntrusiveRefPtr< _type_ >::operator=( nullptr_t null ) {
	DecrementRef();
	m_ptr = null;
	return *this;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
template< typename _derived_ >
qpIntrusiveRefPtr< _type_ > & qpIntrusiveRefPtr< _type_ >::operator=( const qpIntrusiveRefPtr < _derived_ > & rhs ) requires ( IsBaseOf< _type_, _derived_ > ) {
	m_ptr = rhs.Raw();
	IncrementRef();
	return *this;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
template< typename _derived_ >
qpIntrusiveRefPtr< _type_ > & qpIntrusiveRefPtr< _type_ >::operator=( qpIntrusiveRefPtr< _derived_ > && rhs ) noexcept requires ( IsBaseOf< _type_, _derived_ > ) {
	m_ptr = rhs.Raw();
	IncrementRef();
	rhs.Reset( NULL );
	return *this;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
auto qpIntrusiveRefPtr< _type_ >::operator<=>( const qpIntrusiveRefPtr & rhs ) const {
	return ( m_ptr - rhs.m_ptr );
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
auto qpIntrusiveRefPtr< _type_ >::operator<=>( const _type_ * rhs ) const {
	return ( m_ptr - rhs );
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
bool qpIntrusiveRefPtr< _type_ >::operator==( const qpIntrusiveRefPtr & rhs ) const {
	return m_ptr == rhs.m_ptr;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
bool qpIntrusiveRefPtr< _type_ >::operator==( const _type_ * rhs ) const {
	return m_ptr == rhs;
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
void qpIntrusiveRefPtr< _type_ >::Reset() {
	Reset( NULL );
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
void qpIntrusiveRefPtr< _type_ >::Reset( _type_ * data ) {
	DecrementRef();
	m_ptr = data;
	IncrementRef();
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
void qpIntrusiveRefPtr< _type_ >::IncrementRef() {
	if ( m_ptr ) {
		m_ptr->QP_INTRUSIVE_INCREMENT_REF();
	}
}

template< typename _type_ > requires qpHasIntrusiveRefCounter< _type_ >
void qpIntrusiveRefPtr< _type_ >::DecrementRef() {
	if ( m_ptr ) {
		m_ptr->QP_INTRUSIVE_DECREMENT_REF();

		if ( m_ptr->QP_INTRUSIVE_GET_COUNTER() == 0 ) {
			delete m_ptr;
			m_ptr = NULL;
		}
	}
}

template < typename _type_, typename ... _args_ >
constexpr static inline qpIntrusiveRefPtr< _type_ > qpCreateIntrusiveRef( _args_&&... args ) {
	return qpIntrusiveRefPtr< _type_ >( new _type_( qpForward< _args_ >( args )... ) );
}

template < typename _type_, typename _derived_ >
constexpr static inline qpIntrusiveRefPtr< _type_ > qpIntrusiveRefStaticCast( const qpIntrusiveRefPtr< _derived_ > & intrusivePtr ) {
	return qpIntrusiveRefPtr< _type_ >( static_cast< _type_ * >( intrusivePtr.Raw() ) );
}

template < typename _type_, typename _derived_ >
constexpr static inline qpIntrusiveRefPtr< _type_ > qpIntrusiveRefDynamicCast( const qpIntrusiveRefPtr< _derived_ > & intrusivePtr ) {
	return qpIntrusiveRefPtr< _type_ >( dynamic_cast< _type_ * >( intrusivePtr.Raw() ) );
}

template < typename _type_, typename _derived_ >
constexpr static inline qpIntrusiveRefPtr< _type_ > qpIntrusiveRefReinterpretCast( const qpIntrusiveRefPtr< _derived_ > & intrusivePtr ) {
	return qpIntrusiveRefPtr< _type_ >( reinterpret_cast< _type_ * >( intrusivePtr.Raw() ) );
}

template < typename _type_, typename _derived_ >
constexpr static inline qpIntrusiveRefPtr< _type_ > qpIntrusiveRefConstCast( const qpIntrusiveRefPtr< _derived_ > & intrusivePtr ) {
	return qpIntrusiveRefPtr< _type_ >( const_cast< _type_ * >( intrusivePtr.Raw() ) );
}
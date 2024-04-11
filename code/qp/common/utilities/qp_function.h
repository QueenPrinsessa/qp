#pragma once
#include "qp/common/core/qp_smart_pointers.h"
#include "qp/common/debug/qp_debug.h"

template < typename >
class qpFunction;

template < typename _return_, typename ... _args_ >
class qpFunction< _return_( _args_... ) > {
public:
	qpFunction() = default;
	qpFunction( nullptr_t ){}
	template < typename _type_ >
	qpFunction( _type_ func );

	qpFunction( const qpFunction & other ) noexcept;

	qpFunction( qpFunction && other ) noexcept;
	~qpFunction() = default;

	template < typename _type_ >
	qpFunction & operator=( _type_ func );
	qpFunction & operator=( nullptr_t );
	qpFunction & operator=( const qpFunction & rhs );
	qpFunction & operator=( qpFunction && rhs ) noexcept;

	bool operator==( nullptr_t ) { return m_functor != NULL; }

	_return_ operator()( _args_ ... args ) const;

	operator bool() const { return m_functor != NULL; }
private:
	class qpFunctorBase {
	public:
		virtual ~qpFunctorBase() = default;
		virtual _return_ Invoke( _args_... ) const = 0;

		QP_INTRUSIVE_REF_COUNTER;
	};
	template < typename _type_ >
	class qpFunctor : public qpFunctorBase {
	public:
		qpFunctor( const _type_ & func ) : m_func( func ) { }
		virtual ~qpFunctor() override = default;
		virtual _return_ Invoke( _args_ ... args ) const override { return m_func( args... ); }
	private:
		_type_ m_func;
	};

	qpIntrusiveRefPtr< qpFunctorBase > m_functor;
};

template< typename _return_, typename ... _args_ >
template< typename _type_ >
qpFunction< _return_( _args_... ) >::qpFunction( _type_ func ) {
	m_functor = qpCreateIntrusiveRef< qpFunctor< _type_ > >( func );
}

template< typename _return_, typename ... _args_ >
qpFunction< _return_( _args_... ) >::qpFunction( const qpFunction & other ) noexcept {
	m_functor = other.m_functor;
}

template< typename _return_, typename ... _args_ >
qpFunction< _return_( _args_... ) >::qpFunction( qpFunction && other ) noexcept {
	m_functor = qpMove( other.m_functor );
}

template< typename _return_, typename ... _args_ >
template< typename _type_ >
qpFunction< _return_( _args_... ) > & qpFunction< _return_( _args_... ) >::operator=( _type_ func ) {
	m_functor = qpCreateIntrusiveRef< qpFunctor< _type_ > >( func );
	return *this;
}

template< typename _return_, typename ... _args_ >
qpFunction< _return_( _args_... ) > & qpFunction< _return_( _args_... ) >::operator=( nullptr_t ) {
	m_functor.Reset();
	return *this;
}

template< typename _return_, typename ... _args_ >
qpFunction< _return_( _args_... ) > & qpFunction< _return_( _args_... ) >::operator=( const qpFunction & rhs ) {
	m_functor = rhs.m_functor;
	return *this;
}

template< typename _return_, typename ... _args_ >
qpFunction< _return_( _args_... ) > & qpFunction< _return_( _args_... ) >::operator=( qpFunction && rhs ) noexcept {
	m_functor = qpMove( rhs.m_functor );
	return *this;
}

template< typename _return_, typename ... _args_ >
_return_ qpFunction< _return_( _args_... ) >::operator()( _args_... args ) const {
	QP_ASSERT( m_functor != NULL );
	return m_functor->Invoke( args... );
}

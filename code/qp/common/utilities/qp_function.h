#pragma once
#include "qp/common/core/qp_smart_pointers.h"
#include "qp/common/debug/qp_debug.h"

namespace qp {
	template < typename >
	class Function;

	template < typename _return_, typename ... _args_ >
	class Function< _return_( _args_... ) > {
	public:
		Function() = default;
		Function( nullptr_t ) {}
		template < typename _type_ >
		Function( _type_ func );

		Function( const Function & other ) noexcept;

		Function( Function && other ) noexcept;
		~Function() = default;

		template < typename _type_ >
		Function & operator=( _type_ func );
		Function & operator=( nullptr_t );
		Function & operator=( const Function & rhs );
		Function & operator=( Function && rhs ) noexcept;

		bool operator==( nullptr_t ) { return m_functor != NULL; }

		_return_ operator()( _args_ ... args ) const;

		operator bool() const { return m_functor != NULL; }
	private:
		class qpFunctorBase {
		public:
			qpFunctorBase() = default;
			virtual ~qpFunctorBase() = default;
			virtual _return_ Invoke( _args_... ) const = 0;

			QP_INTRUSIVE_REF_COUNTER;
		};
		template < typename _type_ >
		class qpFunctor : public qpFunctorBase {
		public:
			qpFunctor( const _type_ & func ) : m_func( func ) {}
			qpFunctor( _type_ && func ) : m_func( qpMove( func ) ) {}
			virtual ~qpFunctor() override = default;
			virtual _return_ Invoke( _args_ ... args ) const override { return m_func( args... ); }
		private:
			_type_ m_func;
		};

		IntrusiveRefPtr< qpFunctorBase > m_functor;
	};

	template< typename _return_, typename ... _args_ >
	template< typename _type_ >
	Function< _return_( _args_... ) >::Function( _type_ func ) {
		m_functor = CreateIntrusiveRef< qpFunctor< _type_ > >( func );
	}

	template< typename _return_, typename ... _args_ >
	Function< _return_( _args_... ) >::Function( const Function & other ) noexcept {
		m_functor = other.m_functor;
	}

	template< typename _return_, typename ... _args_ >
	Function< _return_( _args_... ) >::Function( Function && other ) noexcept
		: m_functor( Move( other.m_functor ) ) {
	}

	template< typename _return_, typename ... _args_ >
	template< typename _type_ >
	Function< _return_( _args_... ) > & Function< _return_( _args_... ) >::operator=( _type_ func ) {
		m_functor = CreateIntrusiveRef< qpFunctor< _type_ > >( func );
		return *this;
	}

	template< typename _return_, typename ... _args_ >
	Function< _return_( _args_... ) > & Function< _return_( _args_... ) >::operator=( nullptr_t ) {
		m_functor.Reset();
		return *this;
	}

	template< typename _return_, typename ... _args_ >
	Function< _return_( _args_... ) > & Function< _return_( _args_... ) >::operator=( const Function & rhs ) {
		m_functor = rhs.m_functor;
		return *this;
	}

	template< typename _return_, typename ... _args_ >
	Function< _return_( _args_... ) > & Function< _return_( _args_... ) >::operator=( Function && rhs ) noexcept {
		m_functor = Move( rhs.m_functor );
		return *this;
	}

	template< typename _return_, typename ... _args_ >
	_return_ Function< _return_( _args_... ) >::operator()( _args_... args ) const {
		QP_ASSERT( m_functor != NULL );
		return m_functor->Invoke( args... );
	}
}
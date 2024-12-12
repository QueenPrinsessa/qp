#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/utilities/qp_comparison_macros.h"
#include "qp/common/utilities/qp_initializer_list.h"
#include "qp/common/utilities/qp_algorithms.h"

namespace qp {
	template < typename _type_ >
	class Set {
	public:
		class Node {
			friend Set;
		public:
			enum color_t {
				BLACK,
				RED
			};
			Node() = delete;

			_type_ & GetValue() { return m_value; }
			const _type_ & GetValue() const { return m_value; }
			Node * GetRight() const { return m_right; }
			Node * GetLeft() const { return m_left; }
			Node * GetParent() const { return m_parent; }
			color_t GetColor() const { return m_color; }
		private:
			Node( const color_t color, const _type_ & value ) : m_color( color ), m_value( value ) {};
			~Node() = default;
			Node * m_right = NULL;
			Node * m_left = NULL;
			Node * m_parent = NULL;
			color_t m_color = RED;
			_type_ m_value {};

			Node * GetUncle();
			Node * GetSibling();
			Node * GetParent() { return m_parent; }
			Node * GetGrandparent() { return m_parent ? m_parent->m_parent : NULL; }
			void MoveDown( Node * newParent );
			bool IsOnLeft() const { return ( m_parent != NULL ) && ( m_parent->m_left == this ); }
			bool IsOnRight() const { return ( m_parent != NULL ) && ( m_parent->m_right == this ); }
			bool HasRedChild() const { return ( ( m_left != NULL ) && ( m_left->m_color == color_t::RED ) ) || ( ( m_right != NULL ) && ( m_right->m_color == color_t::RED ) ); }
		};
		struct Iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			Iterator( Node * node ) : m_node( node ) {}
			_type_ & operator *() const { return m_node->m_value; }
			_type_ * operator->() { return &m_node->m_value; }
			Iterator & operator++() {
				if ( m_node->m_right != NULL ) {
					m_node = Min( m_node->m_right );
				} else if ( ( m_node->m_parent != NULL ) && ( m_node->m_parent->m_left == m_node ) ) {
					m_node = m_node->m_parent;
				} else {
					Node * next = m_node;
					do {
						next = next->m_parent;
					} while ( ( next != NULL ) && ( ( next->m_parent != NULL ) && ( next->m_parent->m_right == next ) ) );

					if ( next != NULL ) {
						m_node = next->m_parent;
					} else {
						m_node = NULL;
					}
				}
				return *this;
			}
			Iterator operator++( int ) {
				Iterator it = m_node;
				this->operator++();
				return it;
			}
			operator _type_ * ( ) { return &m_node->m_value; }
			auto operator<=>( const Iterator & ) const = default;
			bool operator==( const Iterator & ) const = default;
		private:
			Node * m_node = NULL;
		};
		Set();
		Set( InitializerList< _type_ > initializerList );
		Set( const _type_ * begin, const _type_ * end );
		~Set();

		void Clear();

		template < typename ... _args_ >
		void Emplace( _args_ &&... args );
		void Erase( const _type_ & value );

		Iterator Find( const _type_ & value ) const;

		const Node * GetRoot() const { return m_root; }

		int Length() const { return m_length; }
		bool IsEmpty() const { return m_root == NULL; }

		QP_ITERATORS( Iterator, Iterator( Set::Min( m_root ) ), Iterator( NULL ) );
	private:
		typedef typename Node::color_t color_t;
		Node * m_root = NULL;
		int m_length = 0;
		bool m_leftLeftRotationFlag = false;
		bool m_rightRightRotationFlag = false;
		bool m_leftRightRotationFlag = false;
		bool m_rightLeftRotationFlag = false;

		Node * BinarySearch_r( Node * node, const _type_ & value );
		void Insert_r( Node *& node, const _type_ & value );
		void RotateLeft( Node * node );
		void RotateRight( Node * node );
		void SwapColors( Node * nodeA, Node * nodeB );
		void SwapValues( Node * nodeA, Node * nodeB );
		void FixRedRed_r( Node * node );
		void FixDoubleBlack_r( Node * node );
		void DeleteNode_r( Node * nodeB );
		void DeleteTree_r( Node *& node );
		Node * GetSuccessor( Node * node );
		Node * FindReplacement( Node * node );
		static Node * Min( Node * node );
	};

	template< typename _type_ >
	typename Set< _type_ >::Node * Set< _type_ >::Node::GetUncle() {
		if ( m_parent == NULL || m_parent->m_parent == NULL ) {
			return NULL;
		}

		if ( m_parent->IsOnLeft() ) {
			return m_parent->m_parent->m_right;
		}

		return m_parent->m_parent->m_left;
	}

	template< typename _type_ >
	typename Set< _type_ >::Node * Set< _type_ >::Node::GetSibling() {
		if ( m_parent == NULL ) {
			return NULL;
		}

		if ( IsOnLeft() ) {
			return m_parent->m_right;
		}

		return m_parent->m_left;
	}

	template< typename _type_ >
	void Set< _type_ >::Node::MoveDown( Node * newParent ) {
		if ( m_parent != NULL ) {
			if ( IsOnLeft() ) {
				m_parent->m_left = newParent;
			} else {
				m_parent->m_right = newParent;
			}
		}

		newParent->m_parent = m_parent;
		m_parent = newParent;
	}

	template< typename _type_ >
	Set< _type_ >::Set() {}

	template< typename _type_ >
	Set< _type_ >::Set( InitializerList< _type_ > initializerList ) : Set( initializerList.begin(), initializerList.end() ) {}

	template< typename _type_ >
	Set< _type_ >::Set( const _type_ * begin, const _type_ * end ) {
		for ( auto it = begin; it != end; it++ ) {
			Emplace( *it );
		}
	}

	template< typename _type_ >
	Set< _type_ >::~Set() {
		DeleteTree_r( m_root );
	}

	template< typename _type_ >
	void Set< _type_ >::Clear() {
		DeleteTree_r( m_root );
		m_length = 0;
	}

	template< typename _type_ >
	template< typename ... _args_ >
	void Set< _type_ >::Emplace( _args_ &&... args ) {
		_type_ value( Forward< _args_ >( args )... );
		if ( BinarySearch_r( m_root, value ) == NULL ) {
			Insert_r( m_root, value );
		}
	}

	template< typename _type_ >
	void Set< _type_ >::Erase( const _type_ & value ) {
		Node * node = BinarySearch_r( m_root, value );
		DeleteNode_r( node );
	}

	template< typename _type_ >
	typename Set< _type_ >::Iterator Set< _type_ >::Find( const _type_ & value ) const {
		return Iterator( BinarySearch_r( m_root, value ) );
	}

	template< typename _type_ >
	typename Set< _type_ >::Node * Set< _type_ >::BinarySearch_r( Node * node, const _type_ & value ) {
		if ( node == NULL ) {
			return NULL;
		}

		if ( QP_COMPARE_LESS_THAN( node->m_value, value ) ) {
			return BinarySearch_r( node->m_left, value );
		}

		if ( QP_COMPARE_GREATER_THAN( node->m_value, value ) ) {
			return BinarySearch_r( node->m_right, value );
		}

		return node;
	}

	template< typename _type_ >
	void Set< _type_ >::Insert_r( Node *& node, const _type_ & value ) {
		if ( node == NULL ) {
			node = new Node( color_t::RED, value );
			if ( node == m_root ) {
				node->m_color = color_t::BLACK;
			}
			++m_length;
			return;
		}

		if ( QP_COMPARE_LESS_THAN( value, node->m_value ) ) {
			bool wasInserted = ( node->m_left == NULL );
			Insert_r( node->m_left, value );
			if ( wasInserted ) {
				node->m_left->m_parent = node;
				FixRedRed_r( node->m_left );
			}
		} else {
			bool wasInserted = ( node->m_right == NULL );
			Insert_r( node->m_right, value );
			if ( wasInserted ) {
				node->m_right->m_parent = node;
				FixRedRed_r( node->m_right );
			}
		}
	}

	template< typename _type_ >
	void Set< _type_ >::RotateLeft( Node * node ) {
		Node * newParent = node->m_right;

		if ( node == m_root ) {
			m_root = newParent;
		}

		node->MoveDown( newParent );
		node->m_right = newParent->m_left;
		if ( newParent->m_left != NULL ) {
			newParent->m_left->m_parent = node;
		}
		newParent->m_left = node;
	}

	template< typename _type_ >
	void Set< _type_ >::RotateRight( Node * node ) {
		Node * newParent = node->m_left;

		if ( node == m_root ) {
			m_root = newParent;
		}

		node->MoveDown( newParent );
		node->m_left = newParent->m_right;
		if ( newParent->m_right != NULL ) {
			newParent->m_right->m_parent = node;
		}
		newParent->m_right = node;
	}

	template< typename _type_ >
	void Set< _type_ >::SwapColors( Node * nodeA, Node * nodeB ) {
		Swap( nodeA->m_color, nodeB->m_color );
	}

	template< typename _type_ >
	void Set< _type_ >::SwapValues( Node * nodeA, Node * nodeB ) {
		Swap( nodeA->m_value, nodeB->m_value );
	}

	template< typename _type_ >
	void Set< _type_ >::FixRedRed_r( Node * node ) {
		if ( node == m_root ) {
			node->m_color = color_t::BLACK;
			return;
		}

		Node * parent = node->GetParent();
		Node * grandparent = node->GetGrandparent();
		Node * uncle = node->GetUncle();

		if ( parent->m_color != color_t::BLACK ) {
			if ( uncle != NULL && uncle->m_color == color_t::RED ) {
				parent->m_color = color_t::BLACK;
				uncle->m_color = color_t::BLACK;
				grandparent->m_color = color_t::RED;
				FixRedRed_r( grandparent );
			} else {
				if ( parent->IsOnLeft() ) {
					if ( node->IsOnLeft() ) {
						SwapColors( parent, grandparent );
					} else {
						RotateLeft( parent );
						SwapColors( node, grandparent );
					}
					RotateRight( grandparent );
				} else {
					if ( node->IsOnLeft() ) {
						RotateRight( parent );
						SwapColors( node, grandparent );
					} else {
						SwapColors( parent, grandparent );
					}
					RotateLeft( grandparent );
				}
			}
		}
	}

	template< typename _type_ >
	void Set< _type_ >::FixDoubleBlack_r( Node * node ) {
		if ( node == m_root ) {
			return;
		}

		Node * parent = node->m_parent;
		Node * sibling = node->GetSibling();
		if ( sibling == NULL ) {
			FixDoubleBlack_r( parent );
			return;
		}

		if ( sibling->m_color == color_t::RED ) {
			parent->m_color = color_t::RED;
			sibling->m_color = color_t::BLACK;
			if ( sibling->IsOnLeft() ) {
				RotateRight( parent );
			} else {
				RotateLeft( parent );
			}
			FixDoubleBlack_r( node );
		} else {
			if ( sibling->HasRedChild() ) {
				if ( ( sibling->m_left != NULL ) && ( sibling->m_left->m_color == color_t::RED ) ) {
					if ( sibling->IsOnLeft() ) {
						sibling->m_left->m_color = sibling->m_color;
						sibling->m_color = parent->m_color;
						RotateRight( parent );
					} else {
						sibling->m_left->m_color = parent->m_color;
						RotateRight( sibling );
						RotateLeft( parent );
					}
				} else {
					if ( sibling->IsOnLeft() ) {
						sibling->m_right->m_color = parent->m_color;
						RotateLeft( sibling );
						RotateRight( parent );
					} else {
						sibling->m_right->m_color = sibling->m_color;
						sibling->m_color = parent->m_color;
						RotateLeft( parent );
					}
				}
				parent->m_color = color_t::BLACK;
			} else {
				sibling->m_color = color_t::RED;
				if ( parent->m_color == color_t::BLACK )
					FixDoubleBlack_r( parent );
				else
					parent->m_color = color_t::BLACK;
			}
		}
	}

	template< typename _type_ >
	void Set< _type_ >::DeleteNode_r( Node * nodeB ) {
		if ( nodeB == NULL ) {
			return;
		}

		Node * nodeA = FindReplacement( nodeB );

		bool bothBlack = ( ( nodeA == NULL || nodeA->m_color == color_t::BLACK ) && ( nodeB->m_color == color_t::BLACK ) );
		Node * parent = nodeB->m_parent;

		if ( nodeA == NULL ) {
			if ( nodeB == m_root ) {
				m_root = NULL;
			} else {
				if ( bothBlack ) {
					FixDoubleBlack_r( nodeB );
				} else {
					if ( nodeB->GetSibling() != NULL ) {
						nodeB->GetSibling()->m_color = color_t::RED;
					}
				}

				if ( nodeB->IsOnLeft() ) {
					parent->m_left = NULL;
				} else {
					parent->m_right = NULL;
				}
			}
			delete nodeB;
			return;
		}

		if ( nodeB->m_left == NULL or nodeB->m_right == NULL ) {
			if ( nodeB == m_root ) {
				nodeB->m_value = nodeA->m_value;
				nodeB->m_left = nodeB->m_right = NULL;
				delete nodeA;
			} else {
				if ( nodeB->IsOnLeft() ) {
					parent->m_left = nodeA;
				} else {
					parent->m_right = nodeA;
				}
				delete nodeB;
				nodeA->m_parent = parent;
				if ( bothBlack ) {
					FixDoubleBlack_r( nodeA );
				} else {
					nodeA->m_color = color_t::BLACK;
				}
			}
			return;
		}

		SwapValues( nodeA, nodeB );
		DeleteNode_r( nodeA );
	}

	template< typename _type_ >
	void Set< _type_ >::DeleteTree_r( Node *& node ) {
		if ( node != NULL ) {
			DeleteTree_r( node->m_left );
			DeleteTree_r( node->m_right );
		}

		delete node;
		node = NULL;
	}

	template< typename _type_ >
	typename Set< _type_ >::Node * Set< _type_ >::GetSuccessor( Node * node ) {
		Node * temp = node;

		while ( temp->m_left != NULL ) {
			temp = temp->m_left;
		}

		return temp;
	}

	template< typename _type_ >
	typename Set< _type_ >::Node * Set< _type_ >::FindReplacement( Node * node ) {
		if ( ( node->m_left != NULL ) && ( node->m_right != NULL ) ) {
			return GetSuccessor( node->m_right );
		}

		if ( node->m_left == NULL && node->m_right == NULL ) {
			return NULL;
		}

		if ( node->m_left != NULL ) {
			return node->m_left;
		}

		return node->m_right;
	}

	template< typename _type_ >
	typename Set< _type_ >::Node * Set< _type_ >::Min( Node * node ) {
		if ( node == NULL ) {
			return NULL;
		}

		Node * min = node;
		Node * next = node->m_left;
		while ( next != NULL ) {
			min = next;
			next = next->m_left;
		}

		return min;
	}
}
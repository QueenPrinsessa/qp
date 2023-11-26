#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/utilities/qp_comparison_macros.h"
#include "qp/common/utilities/qp_initializer_list.h"
#include "qp/common/utilities/qp_algorithms.h"

template < typename T >
class qpSet {
public:
	class qpNode {
		friend qpSet;
	public:
		enum color_t {
			BLACK,
			RED
		};
		qpNode() = delete;

		T & GetValue() { return m_value; }
		const T & GetValue() const { return m_value; }
		qpNode * GetRight() const { return m_right; }
		qpNode * GetLeft() const { return m_left; }
		qpNode * GetParent() const { return m_parent; }
		color_t GetColor() const { return m_color; }
	private:
		qpNode( const color_t color, const T & value ) : m_color( color ), m_value( value ){};
		~qpNode() = default;
		qpNode * m_right = NULL;
		qpNode * m_left = NULL;
		qpNode * m_parent = NULL;
		color_t m_color = RED;
		T m_value {};

		qpNode * GetUncle();
		qpNode * GetSibling();
		qpNode * GetParent() { return m_parent; }
		qpNode * GetGrandparent() { return m_parent ? m_parent->m_parent : NULL; }
		void MoveDown( qpNode * newParent );
		bool IsOnLeft() const { return ( m_parent != NULL ) && ( m_parent->m_left == this ); }
		bool IsOnRight() const { return ( m_parent != NULL ) && ( m_parent->m_right == this ); }
		bool HasRedChild() const { return ( ( m_left != NULL ) && ( m_left->m_color == color_t::RED ) ) || ( ( m_right != NULL ) && ( m_right->m_color == color_t::RED ) ); }
	};
	struct Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		Iterator( qpNode * node ) : m_node( node ) { }
		T & operator *() const { return m_node->m_value; }
		T * operator->() { return &m_node->m_value; }
		Iterator & operator++() {
			if ( m_node->m_right != NULL ) {
				m_node = Min( m_node->m_right );
			} else if( ( m_node->m_parent != NULL ) && ( m_node->m_parent->m_left == m_node )) {
				m_node = m_node->m_parent;
			} else {
				qpNode * next = m_node;
				do {
					next = next->m_parent;
				} while ( ( next != NULL ) && ( ( next->m_parent != NULL ) && ( next->m_parent->m_right == next ) ) );

				if( next != NULL ) {
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
		operator T * () { return &m_node->m_value; }
		auto operator<=>( const Iterator & ) const = default;
		bool operator==( const Iterator & ) const = default;
	private:
		qpNode * m_node = NULL;
	};
	qpSet();
	qpSet( qpInitializerList< T > initializerList );
	qpSet( const T * begin, const T * end );
	~qpSet();

	void Clear();

	template < typename ... ARGS >
	void Emplace( ARGS&&... args );
	void Erase( const T & value );

	Iterator Find( const T & value ) const;

	const qpNode * GetRoot() const { return m_root; }

	int Length() const { return m_length; }
	bool IsEmpty() const { return m_root == NULL; }

	QP_ITERATORS( Iterator, Iterator( qpSet::Min( m_root ) ), Iterator( NULL ) );
private:
	typedef typename qpNode::color_t color_t;
	qpNode * m_root = NULL;
	int m_length = 0;
	bool m_leftLeftRotationFlag = false;
	bool m_rightRightRotationFlag = false;
	bool m_leftRightRotationFlag = false;
	bool m_rightLeftRotationFlag = false;

	qpNode * BinarySearch_r( qpNode * node, const T & value );
	void Insert_r( qpNode *& node, const T & value );
	void RotateLeft( qpNode * node );
	void RotateRight( qpNode * node );
	void SwapColors( qpNode * nodeA, qpNode * nodeB );
	void SwapValues( qpNode * nodeA, qpNode * nodeB );
	void FixRedRed_r( qpNode * node );
	void FixDoubleBlack_r( qpNode * node );
	void DeleteNode_r( qpNode * nodeB );
	void DeleteTree_r( qpNode *& node );
	qpNode * GetSuccessor( qpNode * node );
	qpNode * FindReplacement( qpNode * node );
	static qpNode * Min( qpNode * node );
};

template< typename T >
typename qpSet< T >::qpNode * qpSet< T >::qpNode::GetUncle() {
	if ( m_parent == NULL || m_parent->m_parent == NULL ) {
		return NULL;
	}

	if ( m_parent->IsOnLeft() ) {
		return m_parent->m_parent->m_right;
	}

	return m_parent->m_parent->m_left;
}

template< typename T >
typename qpSet< T >::qpNode * qpSet< T >::qpNode::GetSibling() {
	if( m_parent == NULL ) {
		return NULL;
	}

	if( IsOnLeft() ) {
		return m_parent->m_right;
	}

	return m_parent->m_left;
}

template< typename T >
void qpSet< T >::qpNode::MoveDown( qpNode * newParent ) {
	if( m_parent != NULL ) {
		if( IsOnLeft() ) {
			m_parent->m_left = newParent;
		} else {
			m_parent->m_right = newParent;
		}
	}

	newParent->m_parent = m_parent;
	m_parent = newParent;
}

template< typename T >
qpSet< T >::qpSet() {
}

template< typename T >
qpSet< T >::qpSet( qpInitializerList< T > initializerList ) : qpSet( initializerList.begin(), initializerList.end() ) {
}

template< typename T >
qpSet< T >::qpSet( const T * begin, const T * end ) {
	for( auto it = begin; it != end; it++ ) {
		Emplace( *it );
	}
}

template< typename T >
qpSet< T >::~qpSet() {
	DeleteTree_r( m_root );
}

template< typename T >
void qpSet< T >::Clear() {
	DeleteTree_r( m_root );
	m_length = 0;
}

template< typename T >
template< typename ... ARGS >
void qpSet< T >::Emplace( ARGS &&... args ) {
	T value( qpForward< ARGS >( args )... );
	if( BinarySearch_r( m_root, value ) == NULL ) {
		Insert_r( m_root, value );
	}
}

template< typename T >
void qpSet< T >::Erase( const T & value ) {
	qpNode * node = BinarySearch_r( m_root, value );
	DeleteNode_r( node );
}

template< typename T >
typename qpSet< T >::Iterator qpSet< T >::Find( const T & value ) const {
	return Iterator( BinarySearch_r( m_root, value ) );
}

template< typename T >
typename qpSet< T >::qpNode * qpSet<T>::BinarySearch_r( qpNode * node, const T & value ) {
	if( node == NULL ) {
		return NULL;
	}

	if( QP_COMPARE_LESS_THAN( node->m_value, value ) ) {
		return BinarySearch_r( node->m_left, value );
	}

	if ( QP_COMPARE_GREATER_THAN( node->m_value, value ) ) {
		return BinarySearch_r( node->m_right, value );
	}

	return node;
}

template< typename T >
void qpSet<T>::Insert_r( qpNode *& node, const T & value ) {
	if( node == NULL ) {
		node = new qpNode( color_t::RED, value );
		if( node == m_root ) {
			node->m_color = color_t::BLACK;
		}
		++m_length;
		return;
	}

	if( QP_COMPARE_LESS_THAN( value, node->m_value ) ) {
		bool wasInserted = ( node->m_left == NULL );
		Insert_r( node->m_left, value );
		if( wasInserted ) {
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

template< typename T >
void qpSet< T >::RotateLeft( qpNode * node ) {
	qpNode * newParent = node->m_right;

	if( node == m_root ) {
		m_root = newParent;
	}

	node->MoveDown( newParent );
	node->m_right = newParent->m_left;
	if ( newParent->m_left != NULL ) {
		newParent->m_left->m_parent = node;
	}
	newParent->m_left = node;
}

template< typename T >
void qpSet< T >::RotateRight( qpNode * node ) {
	qpNode * newParent = node->m_left;

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

template< typename T >
void qpSet< T >::SwapColors( qpNode * nodeA, qpNode * nodeB ) {
	qpSwap( nodeA->m_color, nodeB->m_color );
}

template< typename T >
void qpSet< T >::SwapValues( qpNode * nodeA, qpNode * nodeB ) {
	qpSwap( nodeA->m_value, nodeB->m_value );
}

template< typename T >
void qpSet< T >::FixRedRed_r( qpNode * node ) {
	if( node == m_root ) {
		node->m_color = color_t::BLACK;
		return;
	}

	qpNode * parent = node->GetParent();
	qpNode * grandparent = node->GetGrandparent();
	qpNode * uncle = node->GetUncle();

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

template< typename T >
void qpSet<T>::FixDoubleBlack_r( qpNode * node ) {
	if ( node == m_root ) {
		return;
	}

	qpNode * parent = node->m_parent;
	qpNode * sibling = node->GetSibling();
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

template< typename T >
void qpSet< T >::DeleteNode_r( qpNode * nodeB ) {
	if( nodeB == NULL ) {
		return;
	}

	qpNode * nodeA = FindReplacement( nodeB );

	bool bothBlack = ( ( nodeA == NULL || nodeA->m_color == color_t::BLACK ) && ( nodeB->m_color == color_t::BLACK ) );
	qpNode * parent = nodeB->m_parent;

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

template< typename T >
void qpSet< T >::DeleteTree_r( qpNode *& node ) {
	if( node != NULL ) {
		DeleteTree_r( node->m_left );
		DeleteTree_r( node->m_right );
	}

	delete node;
	node = NULL;
}

template< typename T >
typename qpSet<T>::qpNode * qpSet<T>::GetSuccessor( qpNode * node ) {
	qpNode * temp = node;

	while( temp->m_left != NULL ) {
		temp = temp->m_left;
	}

	return temp;
}

template< typename T >
typename qpSet< T >::qpNode * qpSet< T >::FindReplacement( qpNode * node ) {
	if( ( node->m_left != NULL ) && ( node->m_right != NULL ) ) {
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

template< typename T >
typename qpSet< T >::qpNode * qpSet<T>::Min( qpNode * node ) {
	if( node == NULL ) {
		return NULL;
	}

	qpNode * min = node;
	qpNode * next = node->m_left;
	while ( next != NULL ) {
		min = next;
		next = next->m_left;
	}

	return min;
}

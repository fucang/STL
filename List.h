#pragma once
#ifndef __LIST_H__
#define __LIST_H__

#include "Iterator.h"

template <class T>
struct __ListNode
{
public:
	T _data;
	__ListNode<T>* _prev;
	__ListNode<T>* _next;
	__ListNode(const T& x = T()) :_data(x), _prev(NULL), _next(NULL)
	{}
private:
	__ListNode<T>& operator=(const __ListNode<T>& x);
};

//T T& T*
template <class T, class Ref, class Ptr>
struct __ListIterator
{
public:
	typedef __ListIterator<T, T&, T*> Iterator;
	typedef __ListIterator<T, Ref, Ptr> Self;
	typedef __ListNode<T> ListNode;

	//五种迭代器的型别
	typedef BidirectionalIteratorTag IteratorCategory;//因为这里的链表为双向链表，所以采用双向迭代器
	typedef T ValueType;
	typedef Ref Reference;
	typedef Ptr Pointer;
	typedef ptrdiff_t DifferenceType;

public:
	__ListIterator() :_node(NULL)
	{}
	__ListIterator(ListNode* x) :_node(x)
	{}
	__ListIterator(const Iterator& x) :_node(x._node)
	{}
	bool operator==(const Self& x)
	{
		return this->_node == x.node;
	}
	bool operator!=(const Self& x)
	{
		return this->_node != x._node;
	}
	Reference operator*()const
	{
		return this->_node->_data;
	}
	Pointer operator->()const
	{
		return &(this->operator*());
	}
	Self& operator++()
	{
		this->_node = this->_node->_next;
		return *this;
	}
	Self operator++(int)
	{
		Self tmp = this->_node;
		++*this;
		return tmp;
	}
	Self& operator--()
	{
		this->_node = this->_node->_prev;
		return *this;
	}
	Self operator--(int)
	{
		Self tmp = this->_node;
		--*this;
		return tmp;
	}
public:
	ListNode* _node;
};

template <class T>
class List
{
	typedef __ListNode<T> ListNode;
public:
	typedef T ValueType;
	typedef __ListIterator<T, T&, T*> Iterator;
	typedef __ListIterator<T, const T&, const T*> ConstIterator;

public:
	List()
	{
		_head = new ListNode();
		this->_head->_prev = this->_head;
		this->_head->_next = this->_head;
	}
	~List()
	{
		this->Clear();
		delete this->_head;

	}
	void Clear()
	{
		Iterator it = this->Begin();
		while (it != this->End())
		{
			Iterator del = it;
			++it;
			delete del._node;
		}
		this->_head->_next = this->_head;
		this->_head->_prev = this->_head;
	}
	void PushBack(const T& x)
	{
		this->Insert(this->End(), x);
	}
	void PushFront(const T& x)
	{
		this->Insert(this->Begin(), x);
	}
	void PopBack()
	{
		this->Erase(--this->End());
	}
	void PopFront()
	{
		this->Erase(this->Begin());
	}
	// 在pos前插入一个节点
	void Insert(Iterator pos, const T& x)
	{
		ListNode* newNode = new ListNode(x);
		ListNode* cur = pos._node;
		ListNode* prev = cur->_prev;
		newNode->_prev = prev;
		prev->_next = newNode;
		newNode->_next = cur;
		cur->_prev = newNode;
	}
	//删除pos位置的数据，返回下一个结点的迭代器
	Iterator Erase(Iterator pos)
	{
		ListNode* prev = pos._node->_prev;
		ListNode* next = pos._node->_next;
		prev->_next = next;
		next->_prev = prev;
		delete pos._node;
		return next;
	}

	Iterator Begin()
	{
		return this->_head->_next;
	}
	
	Iterator End()
	{
		return this->_head;
	}

	ConstIterator Begin()const
	{
		return this->_head->_next;
	}
	ConstIterator End()const
	{
		return this->_head;
	}

protected:
	ListNode* _head;
};


// const类型的迭代器
void PrintList1(const List<int>& l)
{
	List<int>::ConstIterator it = l.Begin();
	while (it != l.End())
	{
		//	(*it)++;
		cout << *it << " ";
		++it;
	}
	cout << endl;
}

void PrintList2(List<int>& l)
{
	List<int>::Iterator it = l.Begin();
	while (it != l.End())
	{
		(*it)++;
		cout << *it << " ";
		++it;
	}
	cout << endl;
}

void TestList()
{
	List<int> l;
	l.PushBack(1);
	l.PushBack(2);
	l.PushBack(3);
	l.PushBack(4);

	l.PopBack();
	l.PopBack();

	PrintList1(l);
	PrintList2(l);
}

#endif /*__LIST_H__*/

#pragma once
#ifndef __DEQUE_H__
#define __DEQUE_H__

#include "Iterator.h"
#include "Allocator.h"

/*deque是分段连续空间。维持其“整体连续”的假象，落在了迭代器的operator++和operator--身上*/

/*
*deque采用一块所谓的map（不是STL中的map容器）作为主控。这里所谓的map是一块小的连续的空间，
*其中每个元素每个元素（此处称为一个结点）都是指针，指向另一段（较大的）连续的线性空间，称为
*缓冲区。缓冲区才是deque的储存空间主体。SGI STL允许指定缓冲区的大小，默认值0表示将使用512bytes缓冲区
*
*map其实是一个T**，也就是说他是一个指针，所指之物又是一个指针，指向型别为T的一块空间
*
*/

//#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG
//template <class T, class Ref, class Ptr, size_t BufSiz>
//struct __deque_iterator {
//  typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
//  typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
//  static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T)); }
//#else /* __STL_NON_TYPE_TMPL_PARAM_BUG*/
//
//template <class T, class Ref, class Ptr>
//struct __deque_iterator {
//	typedef __deque_iterator<T, T&, T*>             iterator;
//	typedef __deque_iterator<T, const T&, const T*> const_iterator;
//	static size_t buffer_size() { return __deque_buf_size(0, sizeof(T)); }
//#endif*/

//deque的迭代器
inline size_t __DequeBufSize(size_t n, size_t sz)
{
	//sz == sizeof(T)
	//如果n不为0，表示buffer size由用户自定义
	//如果n为0，表示buffer size使用默认值
	//那么如果sz（元素大小sizeof(T)）小于512，传回512/sz，否则传回1
	return n != 0 ? n : (sz < 512 ? (size_t)(512 / sz) : (size_t)1);
}
template <class T,class Ref,class Ptr,size_t BufSiz = 0>
struct __DequeIterator
{
	typedef __DequeIterator<T, Ref, Ptr, BufSiz> Self;
	typedef __DequeIterator<T, T&, T*, BufSiz> Iterator;
	typedef __DequeIterator<T, const T&, const T*, BufSiz> ConstIterator;
	static size_t BufferSize() //获得缓冲区的大小
	{ 
		//调用全局函数__DequeBufSize，inline size_t __DequeBufSize(size_t n, size_t sz)
		//如果n不为0，表示buffer size由用户自定义
		//如果n为0，表示buffer size使用默认值
		//那么如果sz（元素大小sizeof(T)）小于512，传回512/sz，否则传回1
		return __DequeBufSize(BufSiz, sizeof(T));
	}

	//五种迭代器的型别
	typedef RandomAccessIteratorTag IteratorCategory;//随机迭代器
	typedef T ValueType;
	typedef Ref Reference;
	typedef Ptr Pointer;
	typedef ptrdiff_t DifferenceType;//两个迭代器之间的距离

	typedef T** MapPointer;

	//保持与容器的联结
	T* _cur;	//此迭代器指向缓冲区中现行的元素
	T* _first;	//此迭代器指向缓冲区的开头
	T* _last;	//此迭代器指向缓冲区的结尾（含备用空间）
	MapPointer _node;//指向管控中心  T** node; 连续

	//构造函数
	__DequeIterator(T* x, MapPointer y)
		: _cur(x), _first(*y), _last(*y + BufferSize()), _node(y) 
	{}
	__DequeIterator() : _cur(0), _first(0), _last(0), _node(0) 
	{}
	__DequeIterator(const Iterator& x)
		: _cur(x._cur), _first(x._first), _last(x._last), _node(x._node) 
	{}

	/*
	*由于迭代器内对各种指针运算都进行了重载操作，所以各种指针运算如加、减、前进、后退
	*都不能直观视之。其中最关键的就是一旦行进时遇到边缘，要特别当心，视前进或后退而定，
	*可能需要调用set_node()跳一个缓冲区
	*/
	void SetNode(MapPointer newNode)//调到正确的缓冲区
	{
		this->_node = newNode;
		this->_first = *newNode; 
		//this->_last = this->_first + BufferSize();
		this->_last = this->_first + (DifferenceType)BufferSize();
	}
	DifferenceType operator-(const Self& x)
	{
		return (DifferenceType)((this->_node - x._node - 1) * BufferSize() + 
			(this->_cur - this->_first) + (x._last - x._cur));
	}
	Self& operator++()
	{
		++this->_cur;
		if (this->_cur == this->_last)//到达末尾，需跳到下一个缓冲区
		{
			SetNode(this->_node + 1);//切换到下一个缓冲区
			this->_cur = this->_first;//的第一个元素
		}
		return *this;
	}
	Self operator++(int)
	{
		Self tmp = *this;
		++(*this);
		return tmp;
	}
	Self& operator--()
	{
		if (this->_cur == this->_first)//已经是第一个元素
		{
			SetNode(this->_node - 1);
			this->_cur = this->_last;
		}
		--this->_cur;
		return *this;
	}
	Self operator--(int)
	{
		Self tmp = *this;
		--(*this);
		return tmp;
	}

	//以实现随机存取，迭代器可以直接跳跃n个距离
	Self& operator+=(DifferenceType n)
	{
		DifferenceType offset = n + (this->_cur - this->_first);//从当前缓冲区的开始位置需跳offset步	
		if (offset >= 0 && offset < (DifferenceType)(BufferSize()))//目标位置在同一个缓冲区
		{
			this->_cur += n;
		}
		else //目标位置不在同一个缓冲区
		{	
			DifferenceType nodeOffset;//当前缓冲区与目标缓冲区相差几个缓冲区
			if (offset > 0)//往后面走
			{
				nodeOffset = offset / (DifferenceType)(BufferSize());
			}
			else//往以前走
			{
				nodeOffset = -(DifferenceType)((-offset - 1) / (DifferenceType)(BufferSize())) - 1;
			}
			//切换到正确的缓冲区
			SetNode(this->_node + nodeOffset);
			//切换到正确的元素
			this->_cur = this->_first + (offset - nodeOffset * (DifferenceType)(BufferSize()));
			return *this;
		}
	}
	Self operator+(DifferenceType n)const
	{
		Self tmp = *this;
		return tmp += n;
	}
	Self& operator-=(DifferenceType n)
	{
		return *this += (-n);
	}
	Self operator-(DifferenceType n)const
	{
		Self tmp = *this;
		return tmp -= n;
	}
	//实现随机存取 Reference T&
	Reference operator[](DifferenceType n)
	{
		return *(*this + n);
	}
	Reference operator*()
	{
		return *(this->_cur);
	}
	Pointer operator->()
	{
		return &(operator*());
	}

	bool operator==(const Self& x) const
	{
		return this->_cur == x._cur;
	}
	bool operator!=(const Self& x) const
	{
		return this->_cur != x._cur;
	}
	bool operator<(const Self& x) const
	{
		return (this->_node == x._node) ? (this->_cur < x._cur) : (this->_node < x._node);
	}
};

template <class T, class Ref, class Ptr, size_t BufSiz = 0>
inline RandomAccessIteratorTag IteratorCategory(const __DequeIterator<T, Ref, Ptr, BufSiz>&)
{
	return RandomAccessIteratorTag();
}

template <class T, class Ref, class Ptr, size_t BufSiz = 0>
inline T* ValueType(const __DequeIterator<T, Ref, Ptr, BufSiz>&) 
{
	return 0;
}

template <class T, class Ref, class Ptr, size_t BufSiz = 0>
inline ptrdiff_t* DistanceType(const __DequeIterator<T, Ref, Ptr, BufSiz>&) 
{
	return 0;
}


//deque的数据结构
/*
*deque除了要维护一个指向map的指针外，还需维护start与finish两个迭代器，分别指向
*第一个缓冲区的第一个元素和最后一个缓冲区的最后一个元素（的下一个位置）。另外，
*还需知道map的大小，因为一旦map所提供的节点不足，就必须重新配置一个更大的map
*/
template <class T,class Allocat = Alloc,size_t BufSiz = 0>
class Deuqe
{
public:
	typedef T ValueType;
	typedef ValueType* Pointer;
	typedef const ValueType* ConstPointer;
	typedef ValueType& Reference;
	typedef const ValueType& ConstReference;
	typedef ptrdiff_t DifferenceType;
	typedef __DequeIterator<T, T&, T*, BufSiz> Iterator;
	typedef __DequeIterator<T, const T&, const T&, BufSiz>  ConstIterator;
protected:
	typedef Pointer* MapPointer;

protected:
	Iterator _start;
	Iterator _finish;
	MapPointer _map;
	size_t _mapSize;

public:
	Iterator Begin()
	{
		return this->_start;
	}
	Iterator End()
	{
		return this->_finish;
	}
	ConstIterator Begin() const 
	{ 
		return this->_start; 
	}
	ConstIterator End() const 
	{ 
		return this->_finish;
	}
	Reference operator[](size_t n)
	{
		return this->_start[(DifferenceType)n];
	}
	Reference Front()
	{
		return *this->_start;
	}
	Reference Back()
	{
		Iterator tmp = this->_finish;
		--tmp;
		return *tmp;
		//return *(this->_finish - 1);错误：因为没有为(_finish - 1)定义运算子
		//return *(this->_finish -= 1);错误：return *(--this->_finish);return *(this->_finish--);均会改变this->_finish
	}
	size_t Size() const
	{
		DifferenceType diff = this->_finish - this->_start;
		return (size_t)diff;
	}
	bool Empty()const
	{
		return this->_finish == this->_start;
	}
};



#endif /*__DEQUE_H__*/

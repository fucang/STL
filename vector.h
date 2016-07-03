#pragma once
#ifndef __VECTOR_H__
#define __VECTOR_H__
#include <assert.h>
#include "Iterator.h"

template <class T>
class Vector
{
public:
	typedef T ValueType;
	typedef ValueType* Iterator; //迭代器为T*，不用重新定义新的迭代器，如List需重写新的迭代器ListIterator
	typedef const ValueType* ConstIterator;//const类型的迭代器
	typedef T& Reference;
	typedef T* Pointer;
public:
	Vector() :_start(0), _finish(0), _endOfStorage(0)
	{}
	~Vector()
	{
		delete[] this->_start;
		_start = _finish = _endOfStorage = 0;
	}
	Iterator Begin()
	{
		return this->_start;
	}
	Iterator End()
	{
		return this->_finish;
	}
	ConstIterator Begin()const
	{
		return this->_start;
	}
	ConstIterator End()const
	{
		return this->_finish;
	}
	ValueType& operator[](size_t index)
	{
		assert(this->_start + index < this->_finish);
		return this->_start[index];
	}
	size_t Size()const
	{
		return this->_finish - this->_start;
	}
	void PushBack(const T& x)
	{
		_CheckStorage();
		*this->_finish = x;
		++this->_finish;
	}
	void PopBack()
	{
		assert(this->_finish != this->_start);
		--this->_finish;
	}
protected:
	void _CheckStorage()
	{
		if (this->_finish == this->_endOfStorage)//虚增容
		{
			size_t size = this->_finish - this->_start;
			size_t newSize = size * 2 + 3;
			ValueType* tmp = new ValueType[newSize];
			for (size_t i = 0; i < size; ++i)
			{
				tmp[i] = this->_start[i];
			}
			delete[] this->_start;
			this->_start = tmp;
			this->_finish = this->_start + size;
			this->_endOfStorage = this->_start + newSize;
		}
	}
protected:
	Iterator _start; //T*
	Iterator _finish;
	Iterator _endOfStorage;
};

void PrintVector1(const Vector<int>& v)
{
	Vector<int>::ConstIterator it = v.Begin();
	while (it != v.End())
	{
		//(*it)++;
		cout << *it << " ";
		++it;
	}
	cout << endl;
}

void PrintVector2(Vector<int>& v)
{
	Vector<int>::Iterator it = v.Begin();
	while (it != v.End())
	{
		(*it)++;
		cout << *it << " ";
		++it;
	}
	cout << endl;
}

void TestVector()
{
	Vector<int> v;
	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	v.PushBack(4);

	PrintVector1(v);
	PrintVector2(v);

	for (size_t i = 0; i < v.Size(); ++i)
	{
		v[i]++;
		cout << v[i] << " ";
	}
	cout << endl;
}

#endif /*__VECTOR_H__*/

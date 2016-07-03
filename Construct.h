#pragma once
#ifndef __CONSTRUCT_H__
#define __CONSTRUCT_H__

#include "Iterator.h"
#include "TypeTraits.h"

//负责构造和析构对象，即调用构造函数和析构函数

/*
*destory()有两个版本，第一个版本接受一个指针，准备将该指针所指之物析构掉，直接调用
*该对象的析构函数即可。
*第二个版本接收first和last两个迭代器，准备将[first,last)（左闭右开）范围内的所有对象析构掉。
*如果每个对象的析构函数都无关痛痒，那么一次次调用这些无关痛痒的析构函数，对效率是一种伤害，
*因此，首先用ValueType()获得迭代所指对象的型别，再利用TypeTraits判断该型别的析构函数是否无
*关痛痒，若为__TrueType,则什么也不用做，若为__FalseType，则在循环中每经历一个对象就调用第一
*个版本的destory();
*/


//构造和析构单个对象
template <class T1, class T2>
inline void Construct(T1* p, const T2& x)
{
	//构造函数不能显示调用，需用new的定位表达式调用
	new(p)T1(x);//new的定位表达式，将p（T1类型）这块空间赋值为x
}
template <class T>
inline void Destroy(T* p)
{
	//调用析构函数
	p->~T();
}

//析构多个对象，即对象数组,传迭代器
template <class ForwardIterator>
inline void Destory(ForwardIterator first, ForwardIterator last)
{
	__Destory(first, last, ValueType(first));
}

//ForwardIterator迭代器的类型，T迭代器所指对象的类型
template <class ForwardIterator, class T>
inline void __Destory(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __TypeTraits<T>::HasTrivialDestructor HasTrivialDestructor;
	__DestoryAux(first, last, HasTrivialDestructor());
}

template <class ForwardIterator>
inline void __DestoryAux(ForwardIterator first, ForwardIterator last, __TrueType)
{
	//内置类型，析构函数什么都不用做
}

template <class ForwardIterator>
inline void __DestoryAux(ForwardIterator first, ForwardIterator last, __FalseType)
{
	//自定义类型，为每个对象调用析构函数
	for (; first < last; ++first)
	{
		Destroy(&(*first));//因为first为迭代器
	}
}

inline void Destroy(char*, char*){}
inline void Destroy(wchar_t*, wchar_t*){}

#endif /*__CONSTRUCT_H__*/

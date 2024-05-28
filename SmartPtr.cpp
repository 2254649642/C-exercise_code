#include <iostream>
//#include <vector>
#include <atomic>

using namespace std;

template<typename T>
class RefCnt
{
public:
	atomic_int addRef()
	{
		return ++cnt;
	}
	atomic_int subRef()
	{
		return --cnt;
	}
	RefCnt(T* ptr = nullptr) : p(ptr)
	{
		if (p != nullptr)
			cnt = 0;
	}
private:
	atomic_int cnt;
	T* p;
};

template<typename T>
class SmartPtr
{
public:
	SmartPtr(T* p = nullptr) : m_ptr(p)
	{
		m_pRef = new RefCnt<T>(m_ptr);
	}
	~SmartPtr()
	{
		if (m_pRef->subRef() == 0)
		{
			delete m_ptr;
			delete m_pRef;
			m_pRef = nullptr;
			m_ptr = nullptr;
		}
	}
	T& operator*()
	{
		return *m_ptr;
	}
	SmartPtr(const SmartPtr<T>& rhs)
	{
		m_pRef = rhs.m_pRef;
		m_ptr = rhs.m_ptr;
		if (m_ptr != nullptr)
			m_pRef->addRef();
	}
	SmartPtr<T>& operator=(const SmartPtr<T>& rhs)
	{
		if (this == &rhs)
			return *this;
		if (m_pRef->subRef() == 0)
		{
			delete m_ptr;
			delete m_pRef;
			m_pRef = nullptr;
			m_ptr = nullptr;
		}
		m_pRef = rhs.m_pRef;
		m_ptr = rhs.m_ptr;
		m_pRef->addRef();
		return *this;
	}
private:
	RefCnt<T>* m_pRef;
	T* m_ptr;
};


int main()
{
	SmartPtr<int> sp1(new int(1));
	SmartPtr<int> sp2;
	sp2 = sp1;
	*sp2 = 2;
	cout << *sp1 << endl;
}

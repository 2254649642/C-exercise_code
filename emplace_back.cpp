#include <iostream>

//using namespace std;

template<typename T>
struct Allocator
{
	T* allocate(size_t size) // 负责内存开辟
	{
		return (T*)malloc(sizeof(T) * size);
	}
	void deallocate(void* p) // 负责内存释放
	{
		free(p);
	}
	template<typename... Type>
	void construct(T* p, Type&&... args) // 负责对象构造
	{
		new (p) T(std::forward<Type>(args)...); // 定位new
	}
	void destroy(T* p) // 负责对象析构
	{
		p->~T(); // ~T()代表了T类型的析构函数
	}
};

/*
容器底层内存开辟，内存释放，对象构造和析构，都通过allocator空间配置器来实现
*/
template<typename T, typename Alloc = Allocator<T>>
class vector
{
public:
	vector(int size = 10)
	{
		// 需要把内存开辟和对象构造分开处理
		_first = _allocator.allocate(size);
		_last = _first;
		_end = _first + size;
	}
	~vector()
	{
		// 析构容器有效的元素，然后释放_first指针指向的堆内存
		for (T* p = _first; p != _last; ++p)
		{
			_allocator.destroy(p); // 把_first指针指向的数组的有效元素进行析构操作
		}
		_allocator.deallocate(_first); // 释放堆上的数组内存
		_first = _last = _end = nullptr;
	}
	vector(const vector<T>& rhs)
	{
		int size = rhs._end - rhs._first;
		_first = _allocator.allocate(size);
		int len = rhs._last - rhs._first;
		for (int i = 0; i < len; ++i)
		{
			_allocator.construct(_first + i, rhs._first[i]);
		}
		_last = _first + len;
		_end = _first + size;
	}
	vector<T>& operator=(const vector<T>& rhs)
	{
		if (this == &rhs)
			return *this;

		for (T* p = _first; p != _last; ++p)
		{
			_allocator.destroy(p); // 把_first指针指向的数组的有效元素进行析构操作
		}
		_allocator.deallocate(_first);

		int size = rhs._end - rhs._first;
		_first = _allocator.allocate(size);
		int len = rhs._last - rhs._first;
		for (int i = 0; i < len; ++i)
		{
			_allocator.construct(_first + i, rhs._first[i]);
		}
		_last = _first + len;
		_end = _first + size;
		return *this;
	}

	//引用折叠 同时匹配左值引用和右值引用
	template<typename Type>
	void push_back(Type&& val) // 向容器末尾添加元素
	{
		if (full())
			expand();
		_allocator.construct(_last, std::forward<Type>(val));
		_last++;
		//std::cout << "push_back(Type&& val)" << std::endl;
	}

	//引用折叠 同时匹配左值引用和右值引用
	template<typename... Types>
	void emplace_back(Types&&... args) // 向容器末尾添加元素
	{
		if (full())
			expand();
		_allocator.construct(_last, std::forward<Types>(args)...);
		_last++;
		//std::cout << "void emplace_back(Types&&... args)" << std::endl;
	}

	void pop_back() // 从容器末尾删除元素
	{
		if (empty())
			return;
		--_last;
		_allocator.destroy(_last);
	}
	T back()const // 返回容器末尾的元素的值
	{
		return *(_last - 1);
	}
	bool full()const { return _last == _end; }
	bool empty()const { return _first == _last; }
	int size()const { return _last - _first; }
private:
	T* _first; // 指向数组起始的位置
	T* _last;  // 指向数组中有效元素的后继位置
	T* _end;   // 指向数组空间的后继位置
	Alloc _allocator; // 定义容器的空间配置器对象

	void expand() // 容器的二倍扩容
	{
		int size = _end - _first;
		T* ptmp = _allocator.allocate(2 * size);
		for (int i = 0; i < size; ++i)
		{
			_allocator.construct(ptmp + i, _first[i]);
		}
		for (T* p = _first; p != _last; ++p)
		{
			_allocator.destroy(p);
		}
		_allocator.deallocate(_first);
		_first = ptmp;
		_last = _first + size;
		_end = _first + 2 * size;
	}
};

class Test
{
public:
	Test(int val = 10) { std::cout << "Test()" << std::endl; }
	~Test() { std::cout << "~Test()" << std::endl; }
	Test(const Test&) { std::cout << "Test(const Test&)" << std::endl; }
	Test(Test&&) { std::cout << "Test(const Test&&)" << std::endl; }
};

int main()
{
	vector<Test> vec; 
	std::cout << "-------------" << std::endl;
	vec.push_back(Test());
	std::cout << "-------------" << std::endl;
	vec.emplace_back(10);
	std::cout << "-------------" << std::endl;
	return 0;
}

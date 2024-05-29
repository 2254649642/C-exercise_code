#include <iostream>
#include <typeinfo>
#include <functional>

using namespace std;

template<typename R, typename... A>
class myfunction
{};

template<typename R, typename... A>
class myfunction<R(*)(A...)> //函数指针部分特例化
{
public:
	using PFUNC = R(*)(A...);
	myfunction(PFUNC func) : pfunc(func) 
	{
		cout << typeid(PFUNC).name() << endl;;
	}
	R operator()(A... args)
	{
		return pfunc(args...);
	}
private:
	PFUNC pfunc;
};

template<typename R, typename... A>
class myfunction<R(A...)> //函数对象部分特例化
{
public:
	using FUNC = R(A...);
	myfunction(FUNC func) : pfunc(func) { 
		cout << typeid(FUNC).name() << endl; //函数对象类型
		cout << typeid(func).name() << endl; //函数指针类型
	}
	R operator()(A... args)
	{
		return pfunc(args...);
	}
private:
	FUNC* pfunc;
};

int sum(int a, int b)
{
	return a + b;
}

int main()
{
	myfunction<int(*)(int, int)> fun1(sum);
	myfunction<int(int, int)> fun2(sum);

	cout << fun1(1, 2) << endl;
	cout << fun2(2, 3) << endl;
	return 0;
}

#ifndef __HEAP_H__
#define __HEAP_H__

#include <vector>
#include <cassert>

/*比较器*/
template<class T>
class Less
{
public:
	bool operator() (const T& left, const T& right)
	{
		return left<right;
	}
};
template<class T>
class Greater
{
public:
	bool operator() (const T& left, const T& right)
	{
		return left>right;
	}
};

template<class T,class Compare = Less<T>>
class Heap
{
public:

	Heap():_hp()//
	{}

	Heap(const T* array,int size)
	{
		for(int i = 0; i<size; i++)
		{
			_hp.push_back(array[i]);
		}
		//通过向下调整算法建目标堆
		for(int i = (_hp.size()-2)/2; i >= 0; i--)
		{
			_AdJustDown(i);
		}
	}
public:
	void Push(const T& value)
	{
		_hp.push_back(value);	 //将新值插入到_hp最后面 	
		_AdJustUp(_hp.size()-1); //向上调整使其在合理位置
	}
	void Pop()
	{
		assert(!_hp.empty());
		swap(_hp[0], _hp[_hp.size()-1]); //将根与最后一个节点交换
		_hp.pop_back();	//将最后一个节点删除
		_AdJustDown(0);		//从0位置，再将堆调整为目标堆
	}
	void Print()
	{
		for(int i = 0; i < _hp.size(); i++)
		{
			cout<<_hp[i]<<" ";
		}
		cout<<endl;
	}
	int Size()
	{
		return _hp.size();
	}
	bool Empty()
	{
		return _hp.empty();
	}
	T& Top()
	{
		assert(_hp.size() > 0);
		return _hp[0];
	}
	/*清理对象*/
	void clear()
	{
		return _hp.clear();
	}
protected:
	void _AdJustUp(int child)
	{
		Compare Com;
		int parent = (child - 1)/2; //找到其父亲
		while(child > 0)
		{
			//孩子比父亲小时，进行交换并调整父亲和孩子的位置
			if(Com(_hp[child], _hp[parent]))
			{
				swap(_hp[child], _hp[parent]);
				child = parent;
				parent = (child-1)/2;
			}
			else
			{
				break;
			}
		}
	}
	void _AdJustDown(int parent)
	{
		Compare Com;
		int child = parent*2+1; //找到父亲的左孩子
		while(child < _hp.size())
		{
			//当右孩子存在且右孩子小于左孩子时，child指向右孩子
			if(child+1 < _hp.size() && Com(_hp[child+1], _hp[child]))
			{
				++child;
			}
			//当最小孩子小于父亲时，进行交换并且调整父亲和孩子的位置
			if(Com(_hp[child], _hp[parent]))
			{
				swap(_hp[child], _hp[parent]);
				parent = child;
				child = parent*2+1;
			}
			else
			{
				break;
			}
		}
	}
private:
	vector<T> _hp;
};



#endif
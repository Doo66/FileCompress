#ifndef __HEAP_H__
#define __HEAP_H__

#include <vector>
#include <cassert>

/*�Ƚ���*/
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
		//ͨ�����µ����㷨��Ŀ���
		for(int i = (_hp.size()-2)/2; i >= 0; i--)
		{
			_AdJustDown(i);
		}
	}
public:
	void Push(const T& value)
	{
		_hp.push_back(value);	 //����ֵ���뵽_hp����� 	
		_AdJustUp(_hp.size()-1); //���ϵ���ʹ���ں���λ��
	}
	void Pop()
	{
		assert(!_hp.empty());
		swap(_hp[0], _hp[_hp.size()-1]); //���������һ���ڵ㽻��
		_hp.pop_back();	//�����һ���ڵ�ɾ��
		_AdJustDown(0);		//��0λ�ã��ٽ��ѵ���ΪĿ���
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
	/*�������*/
	void clear()
	{
		return _hp.clear();
	}
protected:
	void _AdJustUp(int child)
	{
		Compare Com;
		int parent = (child - 1)/2; //�ҵ��丸��
		while(child > 0)
		{
			//���ӱȸ���Сʱ�����н������������׺ͺ��ӵ�λ��
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
		int child = parent*2+1; //�ҵ����׵�����
		while(child < _hp.size())
		{
			//���Һ��Ӵ������Һ���С������ʱ��childָ���Һ���
			if(child+1 < _hp.size() && Com(_hp[child+1], _hp[child]))
			{
				++child;
			}
			//����С����С�ڸ���ʱ�����н������ҵ������׺ͺ��ӵ�λ��
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
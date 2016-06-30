#ifndef __HAFFMANTREE_H__
#define __HAFFMANTREE_H__

#include "Heap.h"

template <class T>
struct HaffmanTreeNode
{
	typedef HaffmanTreeNode<T> Node;
	T _weight;	//权重
	Node* _left;
	Node* _right;

	HaffmanTreeNode(const T& weight)
		:_weight(weight)
		,_left(NULL)
		,_right(NULL)
	{}
};

template <class T>
class HaffmanTree
{
	typedef HaffmanTreeNode<T> Node;
public:
	HaffmanTree()
		:_root(NULL)
	{}
	HaffmanTree(const T* array, int size, const T& invalid)
	{
		_root = _Create(array, size, invalid);
	}
	~HaffmanTree()
	{
		if(_root != NULL)
		{
			_Destory(_root);
		}
	}
	Node* Getroot()
	{
		return _root;
	}
protected:
	void _Destory(Node* root)
	{
		if(root->_left == NULL && root->_right == NULL)
		{
			delete root;
			root = NULL;
		}
		else
		{
			_Destory(root->_left);
			_Destory(root->_right);
		}
	}
	Node* _Create(const T* array, int size, const T& invalid)
	{
		//仿函数
		struct compare
		{
			bool operator()(HaffmanTreeNode<T>* left, HaffmanTreeNode<T>* right)
			{
				return left->_weight < right->_weight;
			}
		};

		Heap<Node*, compare> hp;
		for(int i = 0; i<size; ++i)
		{
			if(array[i] != invalid)
			{
				Node* tmp = new Node(array[i]);
				hp.Push(tmp);
			}		
		}
		//建造哈夫曼树
		//Node* parent = hp.Top();

		while(hp.Size() > 1)
		{
			Node* left = hp.Top();
			hp.Pop();
			Node* right = hp.Top();
			hp.Pop();

		    Node* parent = new Node(left->_weight+right->_weight);
			parent->_left = left;
			parent->_right = right;
			
			hp.Push(parent);
		}
		//_root=parent;
		return hp.Top();
		//hp.Pop();
	}
private:
	Node* _root;
};


#endif
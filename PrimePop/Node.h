#pragma once
#include <memory>

class Node
{
private:
	int val;
public:
	std::shared_ptr<Node>node1, node2;//����̓J�v�Z�������Ȃ��Ă悳�����Ƃ����C�����ɂȂ���(�́H)
	Node(int prime)
		:val(prime)
	{}
	Node(const std::shared_ptr<Node>&n1, const std::shared_ptr<Node>&n2)
	{
		node1 = n1;
		node2 = n2;
	}
	bool prime()
	{
		return node1 == nullptr;
	}
	int value()
	{
		if (val)
		{
			//�����[
			return val;
		}
		else
		{
			//(�f���ł�)�Ȃ��ł��B
			return val = node1->value() * node2->value();
		}
	}
};
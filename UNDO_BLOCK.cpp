//
//   UNDO_BLOCK.cpp : implementation file
//
//	 Author : Jeonggu Kang
//            ggoo.kang@gmail.com
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
# include "UNDO_BLOCK.h"

// Node �� �����ڴ� �⺻�� �ʱ�ȭ�� ������
// candidates �� 0x3FE�� 1111111110 �� ��Ÿ����
// �ĺ����� ǥ���� �� ���ȴ�
Node::Node()
{
	x = 0; 
	y = 0; 
	itsNext =NULL; 
	itsPrev = NULL; 
	candidates = 0x3FE;
}
// Heap�� ������ �����͸� �ٷ� �ֱ� ���� ������
Node::Node(WORD candy, int x, int y)
{
	this->x = x;
	this->y = y;
	this->candidates = candy;
	itsNext = NULL;
	itsPrev = NULL;
}
// �ڱ� �ڽ��� ������ �ִ� �ĺ��� ���� ������ �ڽ��� �б���
// ���θ� Ȯ���ؼ� �������ش�
bool Node::IsJunction()
{
	if( (candidates ^ (1 << 0)) < candidates )
		return true;
	else
		return false;
}

ROOT::ROOT()
{
	undoList = NULL;
	head = NULL;
	tail = NULL;
}
// ���ο� node �� �����Ѵ�. ó�� ���ԵǴ� ��쿡��
// itsBefore �� ���� ��� ������ NULL�� ����� itsNext��
// ���ο� node �� �̾���� �� �ܿ� ��쿡�� ����
// itsBefore �� itsNext�� �̾���� tail�� �̵���Ų��
// ���Ե� undoList�� ������������ �Ҵ�� ROOT Ŭ��������
// node �� ����Ű�� �뵵�� ���ȴ�(��, �������� ����Ʈ ������ ����)
void ROOT::Insert(Node* newnode)
{
	// ó�� node ������ ���
	if( head == NULL )
	{
		// �� �����Ϳ� ������ �Ҵ�
		head = new Node();
		head = newnode;
		tail = new Node();
		tail = newnode;
		undoList = new Node();
		undoList = newnode;
		return;
	}
	else
	{
		tail->setNext(newnode);
		newnode->setPrev(tail);
		tail = newnode;
	}
}
// ����� ���� �Լ�
bool ROOT::Delete(int x, int y)
{
	Node* tempNode = new Node();

	while( (undoList->getNext() != NULL) || (undoList->getNext() == tail) )
	{
		if(	undoList->getX() == x && undoList->getY() == y )
		{
			if( undoList == head )
			{
				head = undoList->getNext();
				delete undoList;
				head->setPrev(NULL);
				undoList = head;
				return true;
			}
			else if( undoList == tail )
			{
				tail = undoList->getPrev();
				delete undoList;
				undoList->setNext(NULL);
				undoList = head;
				return true;
			}
			else
			{
				tempNode = undoList;
				undoList = undoList->getPrev();
				undoList->setNext(tempNode->getNext());
				undoList = undoList->getNext()->getNext();
				undoList->setPrev(tempNode->getPrev());
				delete tempNode;
				undoList = head;
				return true;
			} 
		} 

		if( undoList != tail ) 
			undoList = undoList->getNext();
		else
			return false;
	} 
	return false;
}
// ������ node�� �������ִ� �Լ�. ���� undoList �����Ͱ� tail�� ��ġ�� �ִ� ���
// undoList �����͸� ���� node�� ����(itsPrev)���� �̵��Ѵ�
bool ROOT::DeleteLastNode()
{
	Node* tempNode = new Node();

	if( tail != NULL && tail->getPrev() != NULL )
	{
		tempNode = tail;
		// ���� ����Ʈ �����Ͱ� tail�� ������ ���� �ڷ� �̵����� �ش�
		if( undoList == tail )	undoList = undoList->getPrev();
		tail = tail->getPrev();
		// ���ο� ������ ��尡 ����Ű�� ���� ��带 �����ش�
		tail->setNext(NULL);
		delete tempNode;
		return true;
	}
	return false;
}

bool ROOT::moveNext()
{
	// undoList �����͸� ���� node �� �ű�
	if( undoList->getNext() != NULL )
	{
		undoList = undoList->getNext();
		return true;
	}
	else
		return false;
}

bool ROOT::movePrev()
{
	if( undoList->getPrev() != NULL )
	{
		undoList = undoList->getPrev();
		return true;
	}
	else
		return false;
}
// �۾��ߴ� �������� ���� �����ش�
void ROOT::resetList()
{
	while( DeleteLastNode() ) {}
	moveFirst();
}
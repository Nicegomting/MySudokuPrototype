//
//   UNDO_BLOCK.cpp : implementation file
//
//	 Author : Jeonggu Kang
//            ggoo.kang@gmail.com
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
# include "UNDO_BLOCK.h"

// Node 의 생성자는 기본적 초기화를 수행함
// candidates 의 0x3FE는 1111111110 을 나타내며
// 후보군을 표시할 때 사용된다
Node::Node()
{
	x = 0; 
	y = 0; 
	itsNext =NULL; 
	itsPrev = NULL; 
	candidates = 0x3FE;
}
// Heap에 잡을때 데이터를 바로 주기 위한 생성자
Node::Node(WORD candy, int x, int y)
{
	this->x = x;
	this->y = y;
	this->candidates = candy;
	itsNext = NULL;
	itsPrev = NULL;
}
// 자기 자신이 가지고 있는 후보군 값을 참조해 자신의 분기점
// 여부를 확인해서 리턴해준다
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
// 새로운 node 를 삽입한다. 처음 삽입되는 경우에는
// itsBefore 의 이전 노드 내용을 NULL로 만들고 itsNext에
// 새로운 node 를 이어나가며 그 외에 경우에는 전부
// itsBefore 와 itsNext를 이어나가며 tail을 이동시킨다
// 포함된 undoList는 자유기억공간에 할당된 ROOT 클래스에서
// node 를 가리키는 용도로 사용된다(즉, 직접적인 리스트 포인터 역할)
void ROOT::Insert(Node* newnode)
{
	// 처음 node 상태인 경우
	if( head == NULL )
	{
		// 각 포인터에 기억공간 할당
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
// 예비용 삭제 함수
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
// 마지막 node를 삭제해주는 함수. 현재 undoList 포인터가 tail에 위치해 있는 경우
// undoList 포인터를 현재 node의 이전(itsPrev)노드로 이동한다
bool ROOT::DeleteLastNode()
{
	Node* tempNode = new Node();

	if( tail != NULL && tail->getPrev() != NULL )
	{
		tempNode = tail;
		// 현재 리스트 포인터가 tail에 있으면 같이 뒤로 이동시켜 준다
		if( undoList == tail )	undoList = undoList->getPrev();
		tail = tail->getPrev();
		// 새로운 마지막 노드가 가르키는 다음 노드를 없애준다
		tail->setNext(NULL);
		delete tempNode;
		return true;
	}
	return false;
}

bool ROOT::moveNext()
{
	// undoList 포인터를 다음 node 로 옮김
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
// 작업했던 내역들을 전부 지워준다
void ROOT::resetList()
{
	while( DeleteLastNode() ) {}
	moveFirst();
}
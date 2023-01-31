//
//   UNDO_BLOCK.h : header file
//
//   Author : Jeonggu Kang
//			  ggoo.kang@gmail.com
//
//	 스도쿠 퍼즐의 작업 내역을 저장하기 위해 개발된 양방향 연결리스트 시스템
//   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
# pragma once
# include <windows.h>

class Node
{
private:
	Node* itsNext;
	Node* itsPrev;
	WORD candidates;
	int x;
	int y;
public:
	Node();
	Node(WORD, int x, int y);
	~Node() { }
	WORD GetCandidates() { return candidates; }
	Node* getNext() const { return itsNext; }
	Node* getPrev() const { return itsPrev; }
	void setNext(Node* node) { itsNext = node; }
	void setPrev(Node* node) { itsPrev = node; }
	void setCandy(WORD candy) { candidates = candy; }
	void setX(int ix) { x = ix; }
	void setY(int iy) { y = iy; }
	bool IsJunction();
	int getX() const { return x; }
	int getY() const { return y; }
};

class ROOT
{
private:
	Node* undoList;
	Node* head;
	Node* tail;
public:
	ROOT();
	~ROOT() { if(!head) delete [] head;}
	void Insert(Node* newnode);
	bool Delete(int x, int y);
	bool DeleteLastNode();
	Node* getHead() const { return head; }
	Node* getTail() const { return tail; }
	Node* getPointer() const { return undoList; }
	bool moveNext();
	bool movePrev();
	void moveFirst() { undoList = head; }
	void moveLast() { undoList = tail; }
	void resetList();
};
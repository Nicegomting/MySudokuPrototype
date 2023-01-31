//
//   MySudokuPrototype.h
//
//	 Author : Jeonggu Kang
//			  ggoo.kang@gmail.com
//
//	  �� ���α׷��� �������� �뵵���� �����ο� ���� �� ������ �����մϴ� 
//    This software is released into the public domain. You are free to use it
//	  in any way you like, except that you may not sell this source code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
# pragma once
# include <windows.h>
# include "UNDO_BLOCK.h"
// [20120628] ���α׷� ���� ���� �߰�
# define GOM_VERSION "1.6"
// x ��ŭ 1�� left shift �ϴ� macro function
# define hyperDrive(x) (1 << (x))
// �������� �������� ����
# define MIN_BOARD_X		 50
# define MIN_BOARD_Y		 50
# define MAX_BOARD_X		 500
# define MAX_BOARD_Y		 500
// ���� ���� �ĺ��� ��� ���� �� �ʱ���ġ
# define X_GAP					 10
# define Y_GAP					 14	
# define X_START				 10
# define Y_START				  3
// ������ ���ڰ� ����(�� �ʺ�, ����)
# define GAP	50
// Load ��� ����� ���Ϸ� ���� �о���� �ڷḦ ������ �ִ� ���۷�
# define MAX_BUF 260
typedef enum { MAINFONT, SUBFONT, TITLEFONT, SOLFONT } FONT_TYPE;
typedef enum { WRONG, GOOD, FINISHED } WORKSET;
// �Լ� ���� ���Ǻ�
void CreateSudokuFont(HDC hdc, FONT_TYPE ft);
void ReleaseSudokuFont(HDC hdc, FONT_TYPE ft);
void DrawBoard(HDC hdc);
void SetSudokuTextColor(HDC hdc, COLORREF color);
void CreateSudokuLine(HDC hdc, COLORREF color, POINT weight);
void ReleaseSudokuLine(HDC hdc);
void CreateSudokuBrush(HDC hdc, COLORREF color);
void ReleaseSudokuBrush(HDC hdc);
void PrintAllSudoku(HDC hdc);
void PrintCandidates(HDC hdc);
void PrintSolution(HDC hdc);
void ShowSolution(HWND hWnd);
// �ش� ã�� �ַ�� ���� �Լ�
bool FindSolution();					// �ش�ã�� �ٽ� �Լ�
bool MarkSoloCandy(bool bWork);			// Ȧ�� �ִ� �ĺ��� �������� set �Ѵ�(���ڷ� ����Ʈ ���翩�θ� �ش�)
bool MarkUniqueCandy(bool bWork);		// 3x3Grid, Horizontal, Vertical �� �����ϴ� �ĺ��� �����ϰ� ���� �ĺ��� ��ŷ
void MarkCandyToJunction(POINT pt);		// �μ� pt �� �ĺ� �ϳ��� ��ũ�ϰ� '�б���'���� ����
WORKSET FindWrongCandy();				// �ش� �������� ���� �ش��� �� ã�Ҵ��� Ȥ�� ������������ üũ����
bool RestoreRecentJunction();			// ���� ���¸����� �ֱ��� �б������� ������ ���� ���¸� ������
POINT TakeMostSafeCandy();				// ���� Ȯ���� ����( 1/2 ���� ) �ĺ��� �� ��ǥ�� ������
void FindAllCandy();					// ��ü ���� �ĺ����� ã�´�
void Initialize();						// �ĺ��� �� �� �������� �ʱ�ȭ ���ش�							
// ������ ���� ���� ����ü
HFONT mainFont, mainFontOld;	// ���� ��Ʈ(ū ����)
HFONT subFont, subFontOld;	    // ���� ��Ʈ(�ĺ��� ǥ�� ����)
HFONT titleFont, titleFontOld;  // ������ �׵θ� Ÿ��Ʋ
HFONT solFont, solFontOld;		// �ش� ǥ�� ��Ʈ
// ������ �������� �׸��� ���� ���� GDI Pen
HPEN hGridpen, hOldGridpen;
// ������ �������� �ܰ� Ÿ��Ʋ�� ĥ�ϱ����� GDI Brush
HBRUSH hBrush, hOldBrush;
// ���� ������
int sudokuBoard[9][9];
// �ش� ������
int solutionBoard[9][9];
// �� ĭ�� �ĺ����� �����ϴ� �迭
WORD sudokuCandy[9][9];
// ����Ǵ� �۾��� �����ϱ� ���� ���Ḯ��Ʈ
ROOT* pList;
// ������ ���� ȭ�� DC
HDC g_hdc;
// �ɼ� Flag
bool bShowSolution = false;
bool bLoaded = false;
//
//   MySudokuPrototype.h
//
//	 Author : Jeonggu Kang
//			  ggoo.kang@gmail.com
//
//	  이 프로그램은 비상업적인 용도로의 자유로운 배포 및 변형이 가능합니다 
//    This software is released into the public domain. You are free to use it
//	  in any way you like, except that you may not sell this source code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
# pragma once
# include <windows.h>
# include "UNDO_BLOCK.h"
// [20120628] 프로그램 버전 정의 추가
# define GOM_VERSION "1.6"
// x 만큼 1을 left shift 하는 macro function
# define hyperDrive(x) (1 << (x))
// 보드판의 시작점과 끝점
# define MIN_BOARD_X		 50
# define MIN_BOARD_Y		 50
# define MAX_BOARD_X		 500
# define MAX_BOARD_Y		 500
// 격자 내부 후보군 출력 간격 및 초기위치
# define X_GAP					 10
# define Y_GAP					 14	
# define X_START				 10
# define Y_START				  3
// 보드판 격자간 간격(셀 너비, 높이)
# define GAP	50
// Load 명령 실행시 파일로 부터 읽어들인 자료를 저장할 최대 버퍼량
# define MAX_BUF 260
typedef enum { MAINFONT, SUBFONT, TITLEFONT, SOLFONT } FONT_TYPE;
typedef enum { WRONG, GOOD, FINISHED } WORKSET;
// 함수 원형 정의부
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
// 해답 찾기 솔루션 관련 함수
bool FindSolution();					// 해답찾기 핵심 함수
bool MarkSoloCandy(bool bWork);			// 홀로 있는 후보를 셀값으로 set 한다(인자로 리스트 등재여부를 준다)
bool MarkUniqueCandy(bool bWork);		// 3x3Grid, Horizontal, Vertical 상에 존재하는 후보중 유일하게 들어가는 후보를 마킹
void MarkCandyToJunction(POINT pt);		// 인수 pt 에 후보 하나를 마크하고 '분기점'으로 설정
WORKSET FindWrongCandy();				// 해답 보드판을 살펴 해답을 다 찾았는지 혹은 오류상태인지 체크해줌
bool RestoreRecentJunction();			// 보드 상태를가장 최근의 분기점으로 돌리며 퍼즐 상태를 복구함
POINT TakeMostSafeCandy();				// 가장 확률이 높은( 1/2 부터 ) 후보의 셀 좌표를 구해줌
void FindAllCandy();					// 전체 셀의 후보군을 찾는다
void Initialize();						// 후보군 및 각 보드판을 초기화 해준다							
// 스도쿠 퍼즐 전역 글자체
HFONT mainFont, mainFontOld;	// 메인 폰트(큰 숫자)
HFONT subFont, subFontOld;	    // 서브 폰트(후보군 표시 숫자)
HFONT titleFont, titleFontOld;  // 보드판 테두리 타이틀
HFONT solFont, solFontOld;		// 해답 표시 폰트
// 스도쿠 보드판을 그리기 위한 전역 GDI Pen
HPEN hGridpen, hOldGridpen;
// 스도쿠 보드판의 외곽 타이틀을 칠하기위한 GDI Brush
HBRUSH hBrush, hOldBrush;
// 게임 보드판
int sudokuBoard[9][9];
// 해답 보드판
int solutionBoard[9][9];
// 각 칸의 후보군을 저장하는 배열
WORD sudokuCandy[9][9];
// 진행되는 작업을 저장하기 위한 연결리스트
ROOT* pList;
// 퍼즐의 전역 화면 DC
HDC g_hdc;
// 옵션 Flag
bool bShowSolution = false;
bool bLoaded = false;
//
//   MySudokuPrototype.cpp
//
//   Author : Jeonggu Kang
//            ggoo.kang@gmail.com
//
//   Description :
//        순수 API로 제작한 스도쿠 퍼즐을 자동으로 풀어주는 프로그램입니다
//        알고리즘은 손으로 퍼즐을 푸는 방법을 그대로 코드화한 것입니다
//    
//   History :
//        Version 1.0 - 2009년 3월 10일(2009 Mar 10)
//            - 프로그램명이 Sudoku Gomting 으로 명명되었음
//            - First Release
//
//        Version 1.1 - 2012년 2월 3일(2012 Feb 3)
//            - 유니코드 컴파일이 되도록 수정
//            - 일부 주석 내용 수정
//
//        Version 1.2 - 2012년 6월 27일(2012 June 27)
//            - 유니코드 지원 이후 저장 기능이 작동되지 않던 버그 수정
//            - 버전 문자열 생성 방법 변경
//            - 비주얼스튜디오 2010 마이그레이션
//            - 일부 주석 내용 수정
//
//        Version 1.3 - 2012년 12월 21일 (2012 December 21)
//            - About Box의 이메일 주소 변경
//
//        Version 1.5 - 2022년 12월 28일 (2022 December 28)
//            - 64비트 지원 추가 (Visual Studio 2022 컨버전 포함)
//            - WinMain SAL(source-code annotation language) 주석 추가
//            - 퍼즐 txt 파일 불러오기 및 쓰기 시 UTF-8 인코딩을 기본으로 하도록 변경 (최신 Windows 10 빌드 기본값 반영)
// 
//        Version 1.6 - 2023년 1월 18일 (2023 January 18)
//            - 퍼즐 저장이 안되던 문제 수정
//
//	  이 프로그램은 비상업적인 용도로의 자유로운 배포 및 변형이 가능합니다	   
//    This software is released into the public domain. You are free to use it
//	  in any way you like, except that you may not sell this source code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <tchar.h>
#include "resource1.h"
#include "MySudokuPrototype.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK PuzzleInputDlgProc(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
//[20120627] 버전 문자열을 조합하는 방식으로 변경
LPTSTR lpszClass = TEXT("Sudoku Gomting ")TEXT(GOM_VERSION);

///////////////////////////////////////// 윈도우 메인 루틴 ///////////////////////////////////////// 
int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpszCmdParam,
	_In_ int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass, WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX,
		  100,100,560,600,NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
	
	while(GetMessage(&Message,0,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return static_cast<int>(Message.wParam);
}
///////////////////////////////////////// 메인 윈도우 프로시져  ///////////////////////////////////////// 
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	int x = 0;
	int y = 0;
	PAINTSTRUCT ps;
	OPENFILENAME ofn;
	HANDLE hFile;
	DWORD dwRead = 0;
	DWORD dwWritten = 0;
	TCHAR ext[_MAX_EXT] = TEXT("");
	char buf[MAX_BUF] = "";
	TCHAR lpstrFile[MAX_PATH] = TEXT("");
	char ch[5] = "";
	char saveBuf[9][9];
	
	switch(iMessage) {
	case WM_CREATE:
		// 리스트 생성
		pList = new ROOT();
		// 후보군 및 각 보드판 초기화
		Initialize();
		return 0;
	case WM_COMMAND:
		// 메뉴 명령 분기
		switch(LOWORD(wParam))
		{
		case IDM_SAVE:
			memset(&ofn,0,sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = TEXT("텍스트 파일(.txt)\0*.txt\0");
			ofn.lpstrFile = lpstrFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrDefExt = TEXT("txt");
			if(GetSaveFileName(&ofn) != NULL )
			{
				// 파일명을 구함
				//_tsplitpath_s(lpstrFile, NULL, NULL, NULL, NULL, NULL, NULL, ext, 255);
				// 확장자를 붙임
				//_tcscat_s(ext,_MAX_EXT,TEXT(".txt"));
				// 파일을 만든다
				hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				// 저장 버퍼에 문자변환을 거쳐 퍼즐을 저장한다
				for(int i=0; i<9; i++)
					for(int j=0; j<9; j++)
					{
						/*_itot_s*/_itoa_s(sudokuBoard[i][j],ch,5,10);
						saveBuf[i][j] = ch[0];
					}
				// 퍼즐을 쓴다. 총 81개가 써진다.
				WriteFile(hFile,saveBuf,81,&dwWritten,NULL);
				CloseHandle(hFile);
			}
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;
		case IDM_LOAD:
			memset(&ofn,0,sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = TEXT("텍스트 파일(.txt)\0*.txt\0");
			ofn.lpstrFile = lpstrFile;
			ofn.nMaxFile = MAX_PATH;
			if( GetOpenFileName(&ofn) != NULL)
			{
				bShowSolution = false;	// 이전 퍼즐에서 해답 보기를 선택했을 경우에 대한 초기화
				_tsplitpath_s(lpstrFile,NULL,NULL,NULL,NULL,NULL,NULL,ext,_MAX_EXT);
				// 1차로 확장자의 이상여부를 검사한다
				if( _tcscmp(ext,TEXT(".txt")) != NULL )
					MessageBox(hWnd,TEXT("파일에 문제가 있습니다. 확인해주십시오"),TEXT("Sudoku Gomting"),MB_OK|MB_ICONWARNING);
				else
				{
					// 열어야 하는 파일명을 읽음
					//_tsplitpath_s(lpstrFile,NULL,NULL,NULL,NULL,ext,255,NULL,NULL);
					// 열어야 하는 파일명 확장자를 붙여줌
					//_tcscat_s(ext,_MAX_EXT,TEXT(".txt"));
					hFile = CreateFile(/*ext*/lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if( hFile != INVALID_HANDLE_VALUE)
					{
						// 파일을 읽는다
						bool bRet = ReadFile(hFile,buf,MAX_BUF,&dwRead,NULL);
						// 2차로 읽은 파일의 길이가 이 프로그램에서 저장하는 방식보다
						// 더 큰경우에 올바르지 않은 데이터로 간주한다
						if(!bRet || dwRead != 81)
						{
							MessageBox(hWnd,TEXT("올바른 Sudoku Gomting 용 퍼즐파일이 아닙니다."),TEXT("Sudoku Gomting"),MB_OK);
							CloseHandle(hFile);
							return 0;
						}
						// 3차로 문자가 포함되어 있는지 검사한다
						for(UINT i=0; i</*_tcslen*/strlen(buf); i++)
							if( (buf[i] < 48) || (buf[i] > 57) )
							{
								MessageBox(hWnd,TEXT("올바른 Sudoku Gomting 용 퍼즐파일이 아닙니다."),TEXT("Sudoku Gomting"),MB_OK);
								CloseHandle(hFile);
								return 0;
							}
						// 이전에 로드했던 퍼즐이 있었으면 보드판을 초기화하고 이전 작업 내역을 지운다
						if( bLoaded )
						{
							Initialize();
							pList->resetList();
						}
						// 이상이 없으면 Load 한 내용대로 퍼즐을 준비한다
						// 퍼즐 set
						for(int i=0; i<9; i++)
							for(int j=0; j<9; j++)
							{
								ch[0] = buf[(i*9)+j];
								sudokuBoard[i][j] = /*_ttoi*/atoi(ch);
								solutionBoard[i][j] = /*_ttoi*/atoi(ch);
							}
						CloseHandle(hFile);
					}
					else
					{
						MessageBox(hWnd,TEXT("파일을 읽는 중 에러가 발생했습니다 !"),TEXT("Sudoku Gomting"),MB_OK);
						CloseHandle(hFile);
						return 0;
					}		
				}
			}
			else
				return 0;
			// 최종적으로 로드된 퍼즐의 해답을 찾는다. 퍼즐에 이상이 있는경우 해답보기 메뉴와
			// 저장메뉴를 비활성화 하도록 처리한다
			if( !FindSolution() )
			{
				MessageBox(hWnd,TEXT("퍼즐에 문제가 있습니다. 확인해 주십시오."),TEXT("Sudoku Gomting"),MB_OK|MB_ICONWARNING);
				EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_GRAYED|MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_GRAYED|MF_BYCOMMAND);
				bLoaded = false;
			}
			else
			{
				// 메뉴 활성화
				EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_ENABLED|MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_ENABLED|MF_BYCOMMAND);
				bLoaded = true;
			}
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;
		case IDM_INPUT:
			if(DialogBox(g_hInst,MAKEINTRESOURCE(IDD_INPUT),hWnd,PuzzleInputDlgProc)==IDOK)
			{
				bShowSolution = false;	// 이전 퍼즐에서 해답 보기를 선택했을 경우에 대한 초기화
				// 새로 입력된 퍼즐의 이상유무 점검 및 해답 찾기
				if( !FindSolution() )
				{
					bLoaded = false;
					EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_GRAYED|MF_BYCOMMAND);
					MessageBox(hWnd,TEXT("퍼즐에 문제가 있습니다. 확인해 주십시오."),TEXT("Sudoku Gomting"),MB_OK|MB_ICONWARNING);
				}
				else
				{
					EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_ENABLED|MF_BYCOMMAND);	
					bLoaded = true;			// 퍼즐이 로딩되었음을 체크해준다
				}
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case IDM_SHOWSOLUTION:
			ShowSolution(hWnd);
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		case IDM_ABOUT:
			DialogBox(g_hInst,MAKEINTRESOURCE(IDD_ABOUT),HWND_DESKTOP,AboutDlgProc);
			break;
		case IDM_EXIT:
			SendMessage(hWnd,WM_DESTROY,0,0);
			break;
		}
		return 0;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_F3:
			if(bLoaded)
				ShowSolution(hWnd);
			break;
		}
		return 0;
	case WM_PAINT:
		g_hdc=BeginPaint(hWnd,&ps);
		DrawBoard(g_hdc);
		PrintAllSudoku(g_hdc);
		if(bShowSolution) PrintSolution(g_hdc);
		EndPaint(hWnd,&ps);
		return 0;
	case WM_DESTROY:
		delete pList;
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}
///////////////////////////////////////// 퍼즐 직접입력 대화상자 프로시져  ///////////////////////////////////////// 
INT_PTR CALLBACK PuzzleInputDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	char puzzle[MAX_BUF] = "";
	char ch[5] = "";
	static HWND edit = 0; // 직접입력 에디트의 핸들을 계속 기억하기 위해 static으로 한다
	unsigned long int size = 0;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		 edit = GetDlgItem(hDlg,IDE_PUZZLE); // 에디트의 핸들을 구해둠
		 SendMessage(edit,EM_LIMITTEXT,(WPARAM)81,0); // 퍼즐 직접입력 에디트의 최대 갯수 제한
		return true;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			size = GetDlgItemTextA(hDlg,IDE_PUZZLE,puzzle,_countof(puzzle));
			// 길이 및 문자의 에러 처리
			if( size > 81 || size < 81)
			{
				MessageBox(hDlg,TEXT("이것은 올바른 스도쿠가 아닙니다."),TEXT("Sudoku Gomting"),MB_OK);
				::SetFocus(edit);
				return true;
			}
			else
			{
				// 문자가 포함되어 있는지 검사
				for(UINT i=0; i</*_tcslen*/strlen(puzzle); i++)
					if( (puzzle[i] < 48) || (puzzle[i] > 57) )
					{
						MessageBox(hDlg,TEXT("이것은 올바른 스도쿠가 아닙니다."),TEXT("Sudoku Gomting"),MB_OK);
						::SetFocus(edit);
						return true;
					}
			}
			// 이전에 퍼즐이 로딩된 상태라면 보드판을 초기화하고 이전 작업내용을 지운다
			if( bLoaded )
			{
				pList->resetList();
				Initialize();
			}
			// 그 밖에는 문자형을 숫자로 변환해 버퍼 복사
			for(int i=0; i<9; i++)
				for(int j=0; j<9; j++)
				{
					ch[0] = puzzle[(i*9)+j];
					sudokuBoard[i][j] = /*_ttoi*/atoi(ch);
					solutionBoard[i][j] = /*_ttoi*/atoi(ch);
				}
			EndDialog(hDlg,IDOK);
			return true;
		case IDCANCEL:
			EndDialog(hDlg,0);
			return true;
		}
		return false;
	}
	return false;
}
///////////////////////////////////////// About 프로시져  ///////////////////////////////////////// 
INT_PTR CALLBACK AboutDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		return true;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg,IDOK);
			return true;
		}
		return true;
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////  
//												함수 구현												//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// CreateSudokuFont(HDC hdc, FONT_TYPE ft)
//
// 동작 : 퍼즐내에서 사용할 글자체를 만듬
//
// 인수 : 글자를 만들어야 하는 화면의 DC, 그리고
//		   어떤 형태의 글자를 만들지 지정하는 다음과 같은 인수,
//		   MAINFONT : 퍼즐내에 표시되는 가장 큰 숫자
//		   SUBFONT : 퍼즐내에 후보군을 표시하는 작은 숫자
//		   TITLEFONT : 퍼즐 외곽의 라인번호와 알파벳
//		   SOLFONT : 해답을 표시하는 숫자
//
// 리턴값 : 없음
//
void CreateSudokuFont(HDC hdc, FONT_TYPE ft)
{
	mainFont = CreateFont(30,0,0,0,FW_NORMAL+400,0,0,0,HANGEUL_CHARSET,0,0,0,0,TEXT("바탕체"));
	subFont =  CreateFont(10,0,0,0,FW_NORMAL,0,0,0,HANGEUL_CHARSET,0,0,0,0,TEXT("명조체"));
	titleFont = CreateFont(15,0,0,0,FW_NORMAL+400,0,0,0,ANSI_CHARSET,0,0,0,0,TEXT("Arial"));
	solFont = CreateFont(30,0,0,0,FW_NORMAL+400,0,0,0,HANGEUL_CHARSET,0,0,0,0,TEXT("바탕체"));

	if( ft == MAINFONT )
	{
		mainFontOld = (HFONT)SelectObject(hdc,mainFont);
		SetSudokuTextColor(hdc,RGB(0,0,200));
	}
	else if( ft == SUBFONT )
	{
		subFontOld = (HFONT)SelectObject(hdc,subFontOld);
		SetSudokuTextColor(hdc,RGB(0,150,0));
	}
	else if( ft == TITLEFONT )
	{
		titleFontOld =  (HFONT)SelectObject(hdc,titleFontOld);
		SetSudokuTextColor(hdc,RGB(255,255,255));
	}
	else
	{
		solFontOld= (HFONT)SelectObject(hdc,solFont);
		SetSudokuTextColor(hdc,RGB(255,0,0));
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// ReleaseSudokuFont(HDC hdc, FONT_TYPE ft)
//
// 동작 : CreateSudokuFont로 만든 글자의 리소스를 해제한다
//
// 인수 : 화면 핸들, 해제할 글자체의 종류
//
// 리턴값 : 없음
//
void ReleaseSudokuFont(HDC hdc, FONT_TYPE ft)
{
	if( ft == MAINFONT )
	{
		SelectObject(hdc, mainFontOld);
		DeleteObject(mainFont);
	}
	else if( ft == SUBFONT )
	{
		SelectObject(hdc, subFontOld);
		DeleteObject(subFont);
	}
	else
	{
		SelectObject(hdc, titleFontOld);
		DeleteObject(titleFont);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// DrawBoard(HDC hdc)
//
// 동작 : 퍼즐의 바탕줄을 그린다. 그려지는 순서는,
//		   타이틀라인->타이틀 글자->Y축 선->X축 선 순서이다.
//
// 인수 : 대상의 화면 핸들
//
// 리턴값 : 없음
//
void DrawBoard(HDC hdc)
{
	char ch[5] = {0,};
	char al[9] = {'A','B','C','D','E','F','G','H','I'};
	POINT pt; pt.x = 1;
	RECT rightRT = {MIN_BOARD_X-30, MIN_BOARD_Y,MIN_BOARD_X,MAX_BOARD_Y };
	RECT topRT = {MIN_BOARD_X-30, MIN_BOARD_Y-25,MAX_BOARD_X,MIN_BOARD_Y };
	RECT leftRT = {MAX_BOARD_X, MIN_BOARD_Y-25, MAX_BOARD_X+25, MAX_BOARD_Y+25 };
	RECT bottomRT = { MIN_BOARD_X-30, MAX_BOARD_Y, MAX_BOARD_X, MAX_BOARD_Y+25 };
	// 외곽 타이틀을 칠하기 위한 펜과 브러시 생성
	CreateSudokuBrush(hdc,RGB(200,50,0));	
	CreateSudokuLine(hdc,RGB(200,50,0),pt);
	Rectangle(hdc,rightRT.left,rightRT.top,rightRT.right,rightRT.bottom);
	Rectangle(hdc,topRT.left,topRT.top,topRT.right,topRT.bottom);
	Rectangle(hdc,leftRT.left,leftRT.top,leftRT.right,leftRT.bottom);
	Rectangle(hdc,bottomRT.left,bottomRT.top,bottomRT.right,bottomRT.bottom);
	// 외곽 타이틀 사각형 선을 그린 후 펜과 브러시 삭제
	ReleaseSudokuLine(hdc);
	ReleaseSudokuBrush(hdc);
	// 외곽 타이틀 안에 글씨 출력
	CreateSudokuFont(hdc,TITLEFONT);

	for(int i=1; i<=9; i++)
	{
		/*_itot_s*/_itoa_s(i, ch, _countof(ch), 10);
		TextOutA(hdc,MIN_BOARD_X-20,(i*GAP)+17,ch,static_cast<int>(/*_tcslen*/strlen(ch))); // Side Number
		TextOutA(hdc,MIN_BOARD_X+((i-1)*GAP)+20,MIN_BOARD_Y-20,&al[i-1],1); // Top Alphabet
	}
	ReleaseSudokuFont(hdc,TITLEFONT);
	// 숫자 Grid 외곽 테두리 속성 Set
	CreateSudokuLine(hdc,RGB(200,50,0),pt);
	// 숫자 Grid 외곽 테두리
	Rectangle(hdc,MIN_BOARD_X,MIN_BOARD_Y,MAX_BOARD_X,MAX_BOARD_Y);
	// Y축 선 그음
	for(int i=GAP; i<=(MAX_BOARD_X-GAP); i+=GAP)
	{
		MoveToEx(hdc,i,MIN_BOARD_X,NULL);
		// 3 x 3 Grid 경계선 분기
		if( i == (GAP * 4) || i == (GAP * 7) )
		{
			ReleaseSudokuLine(hdc); pt.x = 2;
			CreateSudokuLine(hdc,RGB(200,50,0),pt);
			LineTo(hdc,i,MAX_BOARD_Y);
		}
		else
		{
			// 3 x 3 Grid 경계선을 그린 직후에만 이전 상태로 선을 복구함
			if( i == (GAP*5) || i == (GAP*8) )
			{
				ReleaseSudokuLine(hdc); pt.x = 1;
				CreateSudokuLine(hdc,RGB(200,50,0),pt);
				LineTo(hdc,i,MAX_BOARD_Y);
			}
			else
				LineTo(hdc,i,MAX_BOARD_Y);
		}
	}
	// X축 선 긋기 전 선 상태 복구
	ReleaseSudokuLine(hdc); pt.x = 1;
	CreateSudokuLine(hdc,RGB(200,50,0),pt);
	// X축 선 그음
	for(int j=GAP; j<=(MAX_BOARD_Y-GAP); j+=GAP)
	{
		MoveToEx(hdc,MIN_BOARD_X,j,NULL);
		// 3 x 3 Grid 경계선 분기
		if( j == (GAP * 4) || j == (GAP * 7) )
		{
			ReleaseSudokuLine(hdc); pt.x = 2;
			CreateSudokuLine(hdc,RGB(200,50,0),pt);
			LineTo(hdc,MAX_BOARD_X,j);
		}
		else
		{
			// 3 x 3 Grid 경계선을 그린 직후에만 이전 상태로 선을 복구함
			if( j == (GAP*5) || j == (GAP*8) )
			{
				ReleaseSudokuLine(hdc); pt.x = 1;
				CreateSudokuLine(hdc,RGB(200,50,0),pt);
				LineTo(hdc,MAX_BOARD_X,j);
			}
			else
				LineTo(hdc,MAX_BOARD_X,j);
		}
	}
	// 테두리 그리기에 사용한 오브젝트 복구 및 해제
	ReleaseSudokuLine(hdc);
	ReleaseSudokuFont(hdc,TITLEFONT);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PrintAllSudoku(HDC hdc)
//
// 동작 : 퍼즐 보드의 내용을 출력한다.
//
// 인수 : 대상의 화면 핸들
//
// 리턴값 : 없음
//
void PrintAllSudoku(HDC hdc)
{
	char ch[5] = {0,};

	CreateSudokuFont(hdc,MAINFONT);
	// y 축 개행 루프
	for(int i=0; i<9; i++)
	{
		// x 축 출력 루프
		for(int j=0; j<9; j++)
		{
			/*_itot_s*/_itoa_s(sudokuBoard[i][j],ch,_countof(ch),10);
			// 숫자가 0 즉, 입력이 안된 경우에는 출력하지 않는다
			if( sudokuBoard[i][j] )
				TextOutA(hdc,MIN_BOARD_X+(GAP*j)+17,MIN_BOARD_Y+(GAP*i)+10,&ch[0],static_cast<int>(/*_tcslen*/strlen(ch)));
		}
	}
	ReleaseSudokuFont(hdc,MAINFONT);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PrintSolution(HDC hdc)
//
// 동작 : 해답 보드의 내용을 출력한다.
//
// 인수 : 대상의 화면 핸들
//
// 리턴값 : 없음
//
void PrintSolution(HDC hdc)
{
	char ch[5] = {0,};

	CreateSudokuFont(hdc,SOLFONT);
	// y 축 개행 루프
	for(int i=0; i<9; i++)
	{
		// x 축 출력 루프
		for(int j=0; j<9; j++)
		{
			/*_itot_s*/_itoa_s(solutionBoard[i][j], ch, _countof(ch), 10);
			// 숫자가 0 즉, 입력이 안된 경우에는 출력하지 않는다
			if( solutionBoard[i][j] && !( solutionBoard[i][j] == sudokuBoard[i][j] ) )
				TextOutA(hdc,MIN_BOARD_X+(GAP*j)+17,MIN_BOARD_Y+(GAP*i)+10,&ch[0],static_cast<int>(/*_tcslen*/strlen(ch)));
		}
	}
	ReleaseSudokuFont(hdc,SOLFONT);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PrintCandidates(HDC hdc)
//
// 동작 : 각 셀의 후보를 출력해준다.
//
// 인수 : 대상의 화면 핸들
//
// 리턴값 : 없음
//
void PrintCandidates(HDC hdc)
{
	char ch[5] = {0,};
	int sudokuNumber = 1;
	int incX, incY; incX = X_START; incY = Y_START;
	
	CreateSudokuFont(hdc, SUBFONT);
	for(int i=0; i<9; i++)
	{
		for(int j=0; j<9; j++)
		{
			for(int k=0; k<9; k++)
			{
				// 현재 출력하려는 칸에 숫자가 있으면 후보군을 출력하지 않는다
				if( solutionBoard[i][j] )
					break;

				/*_itot_s*/_itoa_s(sudokuNumber, ch, _countof(ch), 10);
				if( (sudokuCandy[i][j] ^ hyperDrive(sudokuNumber)) < sudokuCandy[i][j] )
					TextOutA(hdc,MIN_BOARD_X+(GAP*j)+incX,MIN_BOARD_Y+(GAP*i)+incY,ch,static_cast<int>(/*_tcslen*/strlen(ch)));
				// 안쪽 후보군이 3개 출력되었으면 개행
				if( ((k+1) > 2) && ((k+1) % 3 == 0) )
				{
					incY += Y_GAP;
					incX = X_START;
				}
				else
					incX += X_GAP;
				// sudokuNumber 증가
				sudokuNumber++;
			}
			// 다음 출력을 위해 출력번호와 좌표 초기화
			sudokuNumber = 1;
			incX = X_START;
			incY = Y_START;
		}
	}
	ReleaseSudokuFont(hdc,SUBFONT);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// SetSudokuTextColor(HDC hdc, COLORREF color)
//
// 동작 : 출력될 글자의 색상 배경을 TRANSPARENT 모드로 하는 글자색 설정
//
// 인수 : 대상의 화면 핸들 및 글자 색상
//
// 리턴값 : 없음
//
void SetSudokuTextColor(HDC hdc, COLORREF color)
{
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc,color);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// CreateSudokuLine(HDC hdc, COLORREF color, POINT weight)
//
// 동작 : hdc에 color색상의 실선을 weight 굵기 만큼 생성한다
//
// 인수 : 대상의 화면 핸들 및 색상과 굵기
//
// 리턴값 : 없음
//
void CreateSudokuLine(HDC hdc, COLORREF color, POINT weight)
{
	LOGPEN logpen;

	logpen.lopnColor = color;
	logpen.lopnStyle = PS_SOLID;
	logpen.lopnWidth = weight;

	hGridpen = CreatePenIndirect(&logpen);
	hOldGridpen = (HPEN)SelectObject(hdc,hGridpen);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// ReleaseSudouLine(HDC hdc)
//
// 동작 : CreateSudokuLine으로 만든 선의 복구 및 삭제
//
// 인수 : 대상의 화면 핸들
//
// 리턴값 : 없음
//
void ReleaseSudokuLine(HDC hdc)
{
	SelectObject(hdc,hOldGridpen);
	DeleteObject(hGridpen);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// CreateSudokuBrush(HDC hdc, COLORREF color)
//
// 동작 : 타이틀영역의 색상을 칠할때 사용할 브러시를 만든다
//
// 인수 : 대상의 화면 핸들 및 원하는 색상
//
// 리턴값 : 없음
//
void CreateSudokuBrush(HDC hdc, COLORREF color)
{
	LOGBRUSH logbrush;

	logbrush.lbColor = color;
	logbrush.lbHatch = 0;
	logbrush.lbStyle = BS_SOLID;

	hBrush = CreateBrushIndirect(&logbrush);
	hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// ReleaseSudokuBrush(HDC hdc)
//
// 동작 : CreateSudokuBrush로 생성한 브러시를 복구 및 삭제
//
// 인수 : 대상의 화면 핸들
//
// 리턴값 : 없음
//
void ReleaseSudokuBrush(HDC hdc)
{
	SelectObject(hdc,hOldBrush);
	DeleteObject(hBrush);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// FindSolution()
//
// 동작 : 주어진 퍼즐의 해답을 찾는다
//
// 인수 : 없음
//
// 리턴값 : 퍼즐의 해답을 찾았으면 true를 리턴한다
//
bool FindSolution()
{
	bool loop = true;
	bool InList = false;

	// 최초로 모든 후보군 찾음
	FindAllCandy();
	// 최초로 후보가 하나 있는 셀의 후보 기입.. 도중에 오류를 점검해서
	// 완료시에는 끝내고 말도 안되는 퍼즐 상태인경우 해답없음(false)을 리턴한다
	while( MarkSoloCandy(InList) )
	{
		switch( FindWrongCandy() )
		{
			case FINISHED:
				return true;
				break;
			case WRONG:
				return false;
				break;
		}
	}
	// 확정기입 숫자들을 모조리 찾는다. 도중에 퍼즐을 다 찾으면 아주 좋다...!
	// 복잡한거 없이 끝낼 수 있으니까 말이지....역시 오류검출을 겸하고 있다
	while( MarkUniqueCandy(InList) )
		while( MarkSoloCandy(InList) )
		{
			switch( FindWrongCandy() )
			{
			case FINISHED:
				return true;
				break;
			case WRONG:
				return false;
				break;
			}
		}
	// 이제 부터는 리스트에 등재되는 작업이다
	InList = true;
	// 루프를 타고 결론이 날때까지 퍼즐을 푼다... 푸는 과정은 다음 순서에 따른다
	// 확률높은 셀을 찍음 -> Solo 후보 모두 찾음 -> Unique 후보 모두 찾음 ->
	// 과정중 에러를 검출해서 퍼즐이 끝났으면 true리턴, 에러 상황이면 이전 분기점까지 회귀한 이후
	// 계속 반복하는데 회귀할 기점이 없으면 에러인 false 리턴 -> 그 밖엔 처음부터 반복....
	while( loop )
	{
		// 분기점 생성
		MarkCandyToJunction(TakeMostSafeCandy());
		// 모든 셀에 하나만 후보가 단 하나만 존재하는 셀에 값을 찾음(Solo)
		while(MarkSoloCandy(InList))
		{
			// 중간 오류 검출
			switch( FindWrongCandy() )
			{
			case FINISHED:
				return true;
				break;
			case WRONG:
				if( !RestoreRecentJunction() )
					return false;
				break;
			}
		}
		// 전 영역상의 후보들에 홀로 존재하는 후보를 찾아 값을 채움(Unique)
		while(MarkUniqueCandy(InList))
		{
			// 중간 오류 검출
			switch( FindWrongCandy() )
			{
			case FINISHED:
				return true;
				break;
			case WRONG:
				if( !RestoreRecentJunction() )
					return false;
				break;
			}
		}
		// 최종적으로 위의 작업결과에 이상이 없는지 파악
		switch( FindWrongCandy() )
		{
		case FINISHED:
			return true;
			break;
		case WRONG:
			if( !RestoreRecentJunction() )
				return false;
			break;
		}
	}
	// Pastime ?
	return false;
}
//////////////////////////////////////////////////////////////////////////////////////
//	
//  FindAllCandy()
//
//	동작 : solutionBoard 배열을 참고하여 모든 셀들의 가능 후보군을 찾아준다
//		   찾는 순서는 3x3Grid, 수평(Horizontal), 수직(Vertical) 순이고, 각 과정에서
//		   찾은 숫자는 다른 과정에서 찾은 숫자와 중복되지 않는다.
//
//  인수 : 없음
//
//  리턴값 : 없음
//
void FindAllCandy()
{
	int target = 0;
	WORD newCandy = 0x3FE;
	// 3 x 3 Grid 후보군을 찾음
	for(int i=0; i<9; i+=3)
	{
		for(int j=0; j<9; j+=3)
		{
			for(int k=i; k<i+3; k++)
			{
				for(int m=j; m<j+3; m++)
				{
					// 3x3 Grid 내에서 숫자를 찾았으면 제외한다
					if( solutionBoard[k][m] != NULL )
						newCandy ^= hyperDrive(solutionBoard[k][m]);
				}
			}
			// 3x3Grid 내의 후보군을 갱신한다. 단, 갱신 대상 셀이 분기점이면
			// 최하위 비트를 1로 만들어준다.
			for(int n=i; n<i+3; n++)
				for(int p=j; p<j+3; p++)
				{
					if( (sudokuCandy[n][p] ^ hyperDrive(0)) < sudokuCandy[n][p] )
					{
						newCandy |= 1;
						sudokuCandy[n][p] = newCandy;
						newCandy ^= 1;
					}
					else
						sudokuCandy[n][p] = newCandy;
				}
			// 후보군 찾기용 변수 초기화
			newCandy = 0x3FE;
		}
	}
	// 이상 3x3 Grid 후보군 찾기 끝
	// 후보군 찾기용 변수 초기화
	newCandy = 0x3FE;
	// 수평 후보군 찾기 시작
	for(int i=0; i<9; i++)
	{
		for(int j=0; j<9; j++)
		{
			if( solutionBoard[i][j] != NULL )
				newCandy ^= hyperDrive(solutionBoard[i][j]);
		}
		// 수평 후보군을 갱신한다
		for(int k=0; k<9; k++)
		{
			// 갱신대상 셀이 분기점이면 갱신되는 후보군의 최하위 비트에 1을 추가해준다
			if( (sudokuCandy[i][k] ^ hyperDrive(0)) < sudokuCandy[i][k] )
			{
				newCandy |= 1;
				sudokuCandy[i][k] &= newCandy;
				newCandy ^= 1;
			}
			else
				sudokuCandy[i][k] &= newCandy;
		}
		// 다음 수평 후보군을 찾기 전 초기화
		newCandy = 0x3FE;
	}
	// 이상 수평 후보군 찾기 끝
	// 수직 후보군을 찾기 전 초기화
	newCandy = 0x3FE;
	// 수직 후보군 찾기 시작
	for(int i=0; i<9; i++)
	{
		for(int j=0; j<9; j++)
		{
			// 각 라인을 수직으로 검사하며 숫자를 찾음
			if( solutionBoard[j][i] != NULL )
				newCandy ^= hyperDrive(solutionBoard[j][i]);
		}
		for(int k=0; k<9; k++)
		{
			// 갱신대상 셀이 분기점이면 갱신되는 후보군의 최하위 비트에 1을 추가해준다
			if( (sudokuCandy[k][i] ^ hyperDrive(0)) < sudokuCandy[k][i] )
			{
				newCandy |= 1;
				sudokuCandy[k][i] &= newCandy;
				newCandy ^= 1;
			}
			else
				sudokuCandy[k][i] &= newCandy;
		}
		// 다음 수직 후보군을 찾기 전 초기화
		newCandy = 0x3FE;
	}
	// 이상 수직 후보군 찾기 끝
}
///////////////////////////////////////////////////////////////////////////////
//
// MarkSoloCandy(bool bWork)
//
// 동작 : solutionBoard배열에 있는 셀들중에 후보를 단 하나만 가지고있는
//		  셀의 후보를 해당 셀의 값으로 채운다
//
// 인수 :  작업리스트 pList 에 등록할지의 여부를 지정한다. 단, 후보가
//		   하나만있는 셀이기 때문에 분기가 될 수 없으므로 무조건 복기의 대상이다
//
// 리턴값 :  동작에 해당하는 셀이 없는경우 false, 그외에는 true
//
bool MarkSoloCandy(bool bWork)
{
	int candyCount = 0;
	int saveCandy = 0;
	bool result = false;

	for(int i=0; i<9; i++)
	{
		for(int j=0; j<9; j++)
		{
			// 셀이 비어있지 않은 경우엔 후보 갯수를 세지 않음
			if( solutionBoard[i][j] ) 
				continue;
			// 셀의 후보 갯수를 센다
			for(int k=1; k<=9; k++)
			{
				if( (sudokuCandy[i][j] ^ hyperDrive(k)) < sudokuCandy[i][j] )
				{
					candyCount++;
					if(!saveCandy) saveCandy = k;
				}
			}
			// 셀의 후보 갯수가 1개인 경우 셀의 값으로 등록한다
			// 단, 인수가 true 면 리스트에 작업내역을 추가한다
			if( candyCount == 1 )
			{
				if( bWork )
					pList->Insert(new Node(sudokuCandy[i][j],j,i));
				// 해답 배열판에 등록
				solutionBoard[i][j] = saveCandy;
				result = true;
			}
			// 각 카운트 초기화
			candyCount = 0;
			saveCandy = 0;
		}
	}
	// 작업한 내역이 있다면 전체 후보군을 재조정해준다
	if( result ) FindAllCandy();
	return result;
}
///////////////////////////////////////////////////////////////////////////////
//
// MarkCandyToJunction(POINT pt)
//
// 동작 : 인수 pt의 위치에 있는 셀에다 후보 하나를 선택하고
//		   해당 셀을 '분기점'으로 지정한다. (비트 최하위가 1)
//		   또한, 이 작업이 시작된 이후의 모든 작업은 리스트에 추가된다
//
// 인수 : 분기점으로 지정할 셀의 x,y주소
//
// 리턴값 : 없음
//
void MarkCandyToJunction(POINT pt)
{
	int x = pt.x;
	int y = pt.y;
	int target = 0;
	
	for(int k=1; k<=9; k++)
	{
		if( (sudokuCandy[y][x] ^ hyperDrive(k)) < sudokuCandy[y][x] )
		{
			// pt 의 위치를 분기점으로 설정
			sudokuCandy[y][x] ^= hyperDrive(0);
			// 리스트에 등록
			pList->Insert(new Node(sudokuCandy[y][x],x,y));
			// 값 입력
			solutionBoard[y][x] = k;
			break;
		}
	}
	// 최종적으로 전체 후보군을 갱신
	FindAllCandy();
}
///////////////////////////////////////////////////////////////////////////////
//
// MarkUniqueCandy()
//
// 동작 : 1~9까지 돌아가면서 3x3Grid, HOR, VER 영역의 후보들 중 현재 검사중인 숫자가
//		   혼자 들어가는 셀을 찾아 기입한다
//
// 인수 :  현재 작업이 리스트에 등록될 지의 여부(true - 등록, false - 미등록)		
//
// 리턴값 :  더 이상 마킹할 Unique 숫자가 모든 영역들 상에서 발견되지 않는 경우 false를 리턴
//
bool MarkUniqueCandy(bool bWork)
{
	bool being = false;
	int savedSeq = 0;
	int candyCount = 0;
	// 3x3Grid의 Unique 후보 찾기 시작
	for(int p=0; p<9; p+=3)
		for(int q=0; q<9; q+=3)
		{
			for(int i=p; i<p+3; i++)
				for(int j=q; j<q+3; j++)
				{
					// 빈칸만 대상으로 하므로 숫자가 있는 칸은 무시한다
					if( solutionBoard[i][j] ) 
						continue;
					// 1~9까지의 후보를 점검해서 셀에 해당 숫자가 후보로 존재하면
					// 전체 셀을 대상으로 해당 숫자를 후보로 가지는 셀이 있는지 찾고
					// 어떤 셀에도 해당 숫자가 없는 경우에만 현재 셀에 숫자를 등록
					for(int k=1; k<=9; k++)
					{
						// 후보를 발견한 경우 전체 셀을 대상으로 점검
						if( (sudokuCandy[i][j] ^ hyperDrive(k)) < sudokuCandy[i][j] )
						{
							for(int n=p; n<p+3; n++)
								for(int m=q; m<q+3; m++)
								{
									// 점검 대상셀이 비어있지 않은 경우와 대상이 점검 기준과 같은
									// 셀인 경우에는 무시한다
									if( solutionBoard[n][m] || ((n==i)&&(m==j)) )
										continue;
									// 숫자가 있는 경우..
									if( (sudokuCandy[n][m] ^ hyperDrive(k)) < sudokuCandy[n][m] )
										candyCount++;
								}
							// 해당 숫자가 자신외에 더 없다면 셀에 숫자를 등록한다
							// 또한 리스트 등재 여부에 따라 리스트에 등록해준다
							if( !candyCount ) 
							{
								// 넣으려는 칸에 숫자가 있으면 무시한다
								if( !solutionBoard[i][j] ) 
								{
									solutionBoard[i][j] = k;
									if( bWork )
										pList->Insert(new Node(sudokuCandy[i][j],j,i));
									being = true;
								}
							}
							// 갯수세기 초기화
							candyCount = 0;
						}
					}
				}
			}
	// 이상 3x3 Grid Unique 후보 찾기 끝
	// 전체 후보군 재조정 및 후보숫자세기 초기화
	FindAllCandy();
	candyCount = 0;
	// Horizontal Line Unique 후보 찾기 시작
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
		{
			// 셀에 숫자가 있는 경우 검사를 건너뜀
			if( solutionBoard[i][j] )
				continue;
			// 현재 셀의 후보군을 점검해서 후보군이 발견되면 수평선상에 존재하는
			// 빈 셀들의 후보를 점검해서 같은 후보가 있는지 확인한다
			for(int k=1; k<=9; k++)
			{
				if( (sudokuCandy[i][j] ^ hyperDrive(k)) < sudokuCandy[i][j] )
				{
					for(int n=0; n<9; n++)
					{
						if( solutionBoard[i][n] || (n==j)) 
							continue;
						if( (sudokuCandy[i][n] ^ hyperDrive(k)) < sudokuCandy[i][n] )
							candyCount++;
					}
					if( !candyCount )
					{
						// 넣으려는 칸에 숫자가 있으면 무시한다
						if( !solutionBoard[i][j] ) 
						{
							solutionBoard[i][j] = k;
							if( bWork )
								pList->Insert(new Node(sudokuCandy[i][j],j,i));
							being = true;
						}
					}
					candyCount = 0;
				}
			}
		}
	// 이상 Horizontal Unique 후보 찾기 끝
	// 전체 후보군 재조정 및 후보숫자세기 초기화
	FindAllCandy();
	candyCount = 0;
	// Vertical Line 후보 찾기 시작
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
		{
			// 셀에 숫자가 있는 경우 검사를 건너뜀
			if( solutionBoard[j][i] )
				continue;
			// 현재 셀의 후보군을 점검해서 후보군이 발견되면 수직선상에 존재하는
			// 빈 셀들의 후보를 점검해서 같은 후보가 있는지 확인한다
			for(int k=1; k<=9; k++)
			{
				if( (sudokuCandy[j][i] ^ hyperDrive(k)) < sudokuCandy[j][i] )
				{
					for(int n=0; n<9; n++)
					{
						if( solutionBoard[n][i] || (n==j)) 
							continue;
						if( (sudokuCandy[n][i] ^ hyperDrive(k)) < sudokuCandy[n][i] )
							candyCount++;
					}
					if( !candyCount )
					{
						// 넣으려는 칸에 숫자가 있으면 무시한다
						if( !solutionBoard[j][i] )
						{
							solutionBoard[j][i] = k;
							if( bWork )
								pList->Insert(new Node(sudokuCandy[j][i],i,j));
							being = true;
						}
					}
					candyCount = 0;
				}
			}
		}
	// Vertical Line 후보 찾기 끝
	// 작업한 내역이 하나라도 있으면 true를 리턴해주며 마직막으로 전체적인
	// 후보 재조정을 해주도록 한다
	if( being ) FindAllCandy();
	return being;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TakeMostSafeCandy()
//
// 동작 : 전체 퍼즐의 빈 셀들 중에 가장 적은 가능 후보를 가진 셀의 좌표를 찾아준다
//		   단, 동일한 최소확률을 가진셀이 2개 이상인 경우 먼저 찾아진 것을 기준으로한다
//
// 인수 : 없음
//
// 리턴값 : 가장 적은 가능 후보를 가진 셀의 x,y 좌표
//
POINT TakeMostSafeCandy()
{
	POINT pt;
	int candyCount[2] = {0,0};
	// 좌표값 초기화
	pt.x = 0;
	pt.y = 0;

	// solutionBoard 를 뒤져서 확률이 가장 낮은 것을 찾는다
	for( int i=0; i<9; i++)
	{
		for( int j=0; j<9; j++)
		{
			// 해당 셀에 숫자가 들어가 있으면 안돼요!
			if( ! solutionBoard[i][j] )
			{
				for(int k=1; k<=9; k++)
				{
					// 후보의 갯수를 센다
					if( (sudokuCandy[i][j] ^ hyperDrive(k)) < sudokuCandy[i][j] )
						candyCount[0] = candyCount[0] + 1;
				}
				// 후보가 얼마나 있는지 확인한다. [0]번째는 현재 후보 숫자를,
				// [1]번째는 최소후보 갯수를 저장한다
				if( !candyCount[1] || (candyCount[0] < candyCount[1] ) )
				{
					candyCount[1] = candyCount[0];
					pt.x = j;
					pt.y = i;
				}
				// 후보 갯수를 초기화해서 다음 후보를 세기 위한 준비를 한다
				// 후보 갯수를 직접적으로 파악하는 [1]은 초기화하지 않는다
				candyCount[0] = 0;
			}
		}
	}
	return pt;
}
//////////////////////////////////////////////////////////////////////////////////////////
//
// FindWrongCandy()
//
// 동작 : 퍼즐의 남아있는 현재 후보들의 상태를 파악해서 이상유무를 점검해주고
//		   퍼즐의 종료 여부를 확인해준다
//
// 인수 : 없음
//
// 리턴값 : 퍼즐에 이상이 있는 경우 WRONG, 없는 경우 GOOD. 단, 퍼즐에 빈칸이 없으면
//			  FINISHED를 리턴해준다.(즉, 퍼즐이 종료된 경우)
//
WORKSET FindWrongCandy()
{
	WORD target = NULL;
	int IsFull = 0;
	int candyCount = 0;
	// 가장 먼저 전체 셀이 꽉 찬 상태인지 파악한다
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
		{
			if( solutionBoard[i][j] )
				IsFull++;
		}
	if( IsFull == 81 ) 
		return FINISHED;
	// 3x3Grid 이상 유무 점검
	for(int i=0; i<9; i+=3)
		for(int j=0; j<9; j+=3)
		{
			for(int inY=i; inY<i+3; inY++)
				for(int inX=j; inX<j+3; inX++)
				{
					if( solutionBoard[inY][inX] ) continue;
					for(int k=1; k<=9; k++)
					{
						if( (sudokuCandy[inY][inX]^hyperDrive(k)) < sudokuCandy[inY][inX] )
						{
							candyCount++;
							if(!target) target = sudokuCandy[inY][inX];
						}
					}
					// 후보가 하나도 없는 셀이라면 이상이 발생한 것으로 간주한다
					if( !candyCount ) return WRONG;
					// 후보가 하나만 존재하는 셀인 경우 현재 3x3Grid 내의 비어 있는 셀들 중
					// 후보를 하나만 가지는 셀이 같은 후보를 가지고 있는지 파악한다
					if( candyCount == 1 )
					{
						for(int n=i; n<i+3; n++)
							for(int p=j; p<j+3; p++)
							{
								if( solutionBoard[n][p] || ((n==inY)&&(p==inX)) ) continue;
								if( sudokuCandy[n][p] == target )
									return WRONG;
							}
					}
					// 다음 검사를 위한 초기화
					candyCount = 0;
					target = NULL;
				}
		}
	// 이상 3x3Grid 이상 유무 점검 끝
	// Horizontal Line 이상 유무 점검 시작
	for(int y=0; y<9; y++)
		for(int x=0; x<9; x++)
		{
			if( !solutionBoard[y][x] )
			{
				for(int k=1; k<=9; k++)
				{
					if( (sudokuCandy[y][x]^hyperDrive(k)) < sudokuCandy[y][x])
					{
						candyCount++;
						if(!target) target = sudokuCandy[y][x];
					}
				}
			}
			if( candyCount == 1 )
			{
				for(int inX=0; inX<9; inX++)
				{
					if( inX != x && !solutionBoard[y][inX])
						if( sudokuCandy[y][inX] == target )
							return WRONG;
				}
			}
			// 다음 검사를 위한 초기화
			candyCount = 0;
			target = NULL;
		}
	// 이상 Horizontal Line 이상 유무 점검 끝
	// Vertical Line 이상 유무 점검 시작
	for(int x=0; x<9; x++)
		for(int y=0; y<9; y++)
		{
			if( !solutionBoard[y][x] )
			{
				for(int k=1; k<=9; k++)
				{
					if( (sudokuCandy[y][x]^hyperDrive(k)) < sudokuCandy[y][x])
					{
						candyCount++;
						if(!target) target = sudokuCandy[y][x];
					}
				}
			}
			if( candyCount == 1 )
			{
				for(int inY=0; inY<9; inY++)
				{
					if( inY != y && !solutionBoard[inY][x])
						if( sudokuCandy[inY][x] == target )
							return WRONG;
				}
			}
			// 다음 검사를 위한 초기화
			candyCount = 0;
			target = NULL;
		}
	// 이상 Vertical Line 이상 유무 점검 끝
	return GOOD;
}
//////////////////////////////////////////////////////////////////////////////////////
//
// RestoreRecentJunction()
//
// 동작 : 가장 최근의 분기점 셀까지 돌아가면서 작업해왔던 셀들의 내용을 지우고
//		   가장 최근의 분기점까지 돌아가서 다음 후보를 선택하는데, 남은 후보가 없으면
//		   더 이전의 분기점까지 돌아가면서 위의 작업을 반복한다. 만약 더 이상 돌아갈 수
//		   있는 셀이 없으면 에러로 판정한다.
//
// 인수 : 없음
//
// 리턴값 : 더 이상 돌아갈 수 있는 셀이 없는경우 false를 리턴
//
bool RestoreRecentJunction()
{
	int rx = 0;
	int ry = 0;
	bool loop = true;
	WORD candy = 0x3FE;
	// 리스트 포인터의 위치를 제일 마지막으로 둔다
	pList->moveLast();

	while( loop )
	{
		// 가장 최근의 분기점을 만날때까지 작업을 취소해준다
		while(!(pList->getPointer()->IsJunction()))
		{
			rx = pList->getPointer()->getX();
			ry = pList->getPointer()->getY();
			candy = pList->getPointer()->GetCandidates();
			solutionBoard[ry][rx] = 0;
			sudokuCandy[ry][rx] = candy;
			pList->DeleteLastNode();
		}
		// 최근 분기점의 좌표와 원래 후보군 구함
		rx = pList->getPointer()->getX();
		ry = pList->getPointer()->getY();
		candy = pList->getPointer()->GetCandidates();
		// 최근의 분기점을 만났으면 후보를 바꾸려고 시도한다
		for(int k=1; k<=9; k++)
		{
			// 후보를 만났으면 선택되었었던 후보인지 파악하고 선택한다
			if( (candy ^ hyperDrive(k)) < candy )
			{
				if( k > solutionBoard[ry][rx] )
				{
					solutionBoard[ry][rx] = k;
					FindAllCandy();
					return true;
				}
			}
		}
		// 이곳에 도달하면 후보가 없었다는 것이므로 더 이전의 분기점으로 돌아가야 한다
		// 그런데 더 이상 돌아갈 분기점이 없다면 에러이다...
		if( pList->getTail() == pList->getHead() )
			return false;
		else
		{
			candy ^= 1;
			pList->getPointer()->setCandy(candy);
		}
	} // outer while
	return false;
}
//////////////////////////////////////////////////////////////////////////////////////
//
// Initialize()
//
// 동작 : 각 보드판 및 후보군을 초기화한다.
//
// 인수 : 없음
//
// 리턴값 : 없음
//
void Initialize()
{
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
		{
			sudokuCandy[i][j] = 0x3FE;
			solutionBoard[i][j] = 0;
			sudokuBoard[i][j] = 0;
		}
}
//////////////////////////////////////////////////////////////////////////////////////
//
// ShowSolution(HWND hWnd)
//
// 동작 : 해답을 보여주는 flag 을 true로 하고 hWnd 를 다시 그린다
//
// 인수 : 다시 그려지는 윈도우의 핸들
//
// 리턴값 : 없음
//
void ShowSolution(HWND hWnd)
{
	if( bShowSolution ) 
		bShowSolution = false; 
	else
		bShowSolution = true;
	InvalidateRect(hWnd,NULL,TRUE);
}

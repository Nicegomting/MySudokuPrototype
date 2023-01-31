//
//   MySudokuPrototype.cpp
//
//   Author : Jeonggu Kang
//            ggoo.kang@gmail.com
//
//   Description :
//        ���� API�� ������ ������ ������ �ڵ����� Ǯ���ִ� ���α׷��Դϴ�
//        �˰����� ������ ������ Ǫ�� ����� �״�� �ڵ�ȭ�� ���Դϴ�
//    
//   History :
//        Version 1.0 - 2009�� 3�� 10��(2009 Mar 10)
//            - ���α׷����� Sudoku Gomting ���� ���Ǿ���
//            - First Release
//
//        Version 1.1 - 2012�� 2�� 3��(2012 Feb 3)
//            - �����ڵ� �������� �ǵ��� ����
//            - �Ϻ� �ּ� ���� ����
//
//        Version 1.2 - 2012�� 6�� 27��(2012 June 27)
//            - �����ڵ� ���� ���� ���� ����� �۵����� �ʴ� ���� ����
//            - ���� ���ڿ� ���� ��� ����
//            - ���־�Ʃ��� 2010 ���̱׷��̼�
//            - �Ϻ� �ּ� ���� ����
//
//        Version 1.3 - 2012�� 12�� 21�� (2012 December 21)
//            - About Box�� �̸��� �ּ� ����
//
//        Version 1.5 - 2022�� 12�� 28�� (2022 December 28)
//            - 64��Ʈ ���� �߰� (Visual Studio 2022 ������ ����)
//            - WinMain SAL(source-code annotation language) �ּ� �߰�
//            - ���� txt ���� �ҷ����� �� ���� �� UTF-8 ���ڵ��� �⺻���� �ϵ��� ���� (�ֽ� Windows 10 ���� �⺻�� �ݿ�)
// 
//        Version 1.6 - 2023�� 1�� 18�� (2023 January 18)
//            - ���� ������ �ȵǴ� ���� ����
//
//	  �� ���α׷��� �������� �뵵���� �����ο� ���� �� ������ �����մϴ�	   
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
//[20120627] ���� ���ڿ��� �����ϴ� ������� ����
LPTSTR lpszClass = TEXT("Sudoku Gomting ")TEXT(GOM_VERSION);

///////////////////////////////////////// ������ ���� ��ƾ ///////////////////////////////////////// 
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
///////////////////////////////////////// ���� ������ ���ν���  ///////////////////////////////////////// 
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
		// ����Ʈ ����
		pList = new ROOT();
		// �ĺ��� �� �� ������ �ʱ�ȭ
		Initialize();
		return 0;
	case WM_COMMAND:
		// �޴� ��� �б�
		switch(LOWORD(wParam))
		{
		case IDM_SAVE:
			memset(&ofn,0,sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = TEXT("�ؽ�Ʈ ����(.txt)\0*.txt\0");
			ofn.lpstrFile = lpstrFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrDefExt = TEXT("txt");
			if(GetSaveFileName(&ofn) != NULL )
			{
				// ���ϸ��� ����
				//_tsplitpath_s(lpstrFile, NULL, NULL, NULL, NULL, NULL, NULL, ext, 255);
				// Ȯ���ڸ� ����
				//_tcscat_s(ext,_MAX_EXT,TEXT(".txt"));
				// ������ �����
				hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				// ���� ���ۿ� ���ں�ȯ�� ���� ������ �����Ѵ�
				for(int i=0; i<9; i++)
					for(int j=0; j<9; j++)
					{
						/*_itot_s*/_itoa_s(sudokuBoard[i][j],ch,5,10);
						saveBuf[i][j] = ch[0];
					}
				// ������ ����. �� 81���� ������.
				WriteFile(hFile,saveBuf,81,&dwWritten,NULL);
				CloseHandle(hFile);
			}
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;
		case IDM_LOAD:
			memset(&ofn,0,sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = TEXT("�ؽ�Ʈ ����(.txt)\0*.txt\0");
			ofn.lpstrFile = lpstrFile;
			ofn.nMaxFile = MAX_PATH;
			if( GetOpenFileName(&ofn) != NULL)
			{
				bShowSolution = false;	// ���� ���񿡼� �ش� ���⸦ �������� ��쿡 ���� �ʱ�ȭ
				_tsplitpath_s(lpstrFile,NULL,NULL,NULL,NULL,NULL,NULL,ext,_MAX_EXT);
				// 1���� Ȯ������ �̻󿩺θ� �˻��Ѵ�
				if( _tcscmp(ext,TEXT(".txt")) != NULL )
					MessageBox(hWnd,TEXT("���Ͽ� ������ �ֽ��ϴ�. Ȯ�����ֽʽÿ�"),TEXT("Sudoku Gomting"),MB_OK|MB_ICONWARNING);
				else
				{
					// ����� �ϴ� ���ϸ��� ����
					//_tsplitpath_s(lpstrFile,NULL,NULL,NULL,NULL,ext,255,NULL,NULL);
					// ����� �ϴ� ���ϸ� Ȯ���ڸ� �ٿ���
					//_tcscat_s(ext,_MAX_EXT,TEXT(".txt"));
					hFile = CreateFile(/*ext*/lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if( hFile != INVALID_HANDLE_VALUE)
					{
						// ������ �д´�
						bool bRet = ReadFile(hFile,buf,MAX_BUF,&dwRead,NULL);
						// 2���� ���� ������ ���̰� �� ���α׷����� �����ϴ� ��ĺ���
						// �� ū��쿡 �ùٸ��� ���� �����ͷ� �����Ѵ�
						if(!bRet || dwRead != 81)
						{
							MessageBox(hWnd,TEXT("�ùٸ� Sudoku Gomting �� ���������� �ƴմϴ�."),TEXT("Sudoku Gomting"),MB_OK);
							CloseHandle(hFile);
							return 0;
						}
						// 3���� ���ڰ� ���ԵǾ� �ִ��� �˻��Ѵ�
						for(UINT i=0; i</*_tcslen*/strlen(buf); i++)
							if( (buf[i] < 48) || (buf[i] > 57) )
							{
								MessageBox(hWnd,TEXT("�ùٸ� Sudoku Gomting �� ���������� �ƴմϴ�."),TEXT("Sudoku Gomting"),MB_OK);
								CloseHandle(hFile);
								return 0;
							}
						// ������ �ε��ߴ� ������ �־����� �������� �ʱ�ȭ�ϰ� ���� �۾� ������ �����
						if( bLoaded )
						{
							Initialize();
							pList->resetList();
						}
						// �̻��� ������ Load �� ������ ������ �غ��Ѵ�
						// ���� set
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
						MessageBox(hWnd,TEXT("������ �д� �� ������ �߻��߽��ϴ� !"),TEXT("Sudoku Gomting"),MB_OK);
						CloseHandle(hFile);
						return 0;
					}		
				}
			}
			else
				return 0;
			// ���������� �ε�� ������ �ش��� ã�´�. ���� �̻��� �ִ°�� �ش亸�� �޴���
			// ����޴��� ��Ȱ��ȭ �ϵ��� ó���Ѵ�
			if( !FindSolution() )
			{
				MessageBox(hWnd,TEXT("���� ������ �ֽ��ϴ�. Ȯ���� �ֽʽÿ�."),TEXT("Sudoku Gomting"),MB_OK|MB_ICONWARNING);
				EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_GRAYED|MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_GRAYED|MF_BYCOMMAND);
				bLoaded = false;
			}
			else
			{
				// �޴� Ȱ��ȭ
				EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_ENABLED|MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_ENABLED|MF_BYCOMMAND);
				bLoaded = true;
			}
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;
		case IDM_INPUT:
			if(DialogBox(g_hInst,MAKEINTRESOURCE(IDD_INPUT),hWnd,PuzzleInputDlgProc)==IDOK)
			{
				bShowSolution = false;	// ���� ���񿡼� �ش� ���⸦ �������� ��쿡 ���� �ʱ�ȭ
				// ���� �Էµ� ������ �̻����� ���� �� �ش� ã��
				if( !FindSolution() )
				{
					bLoaded = false;
					EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_GRAYED|MF_BYCOMMAND);
					MessageBox(hWnd,TEXT("���� ������ �ֽ��ϴ�. Ȯ���� �ֽʽÿ�."),TEXT("Sudoku Gomting"),MB_OK|MB_ICONWARNING);
				}
				else
				{
					EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem(GetMenu(hWnd),IDM_SHOWSOLUTION,MF_ENABLED|MF_BYCOMMAND);	
					bLoaded = true;			// ������ �ε��Ǿ����� üũ���ش�
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
///////////////////////////////////////// ���� �����Է� ��ȭ���� ���ν���  ///////////////////////////////////////// 
INT_PTR CALLBACK PuzzleInputDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	char puzzle[MAX_BUF] = "";
	char ch[5] = "";
	static HWND edit = 0; // �����Է� ����Ʈ�� �ڵ��� ��� ����ϱ� ���� static���� �Ѵ�
	unsigned long int size = 0;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		 edit = GetDlgItem(hDlg,IDE_PUZZLE); // ����Ʈ�� �ڵ��� ���ص�
		 SendMessage(edit,EM_LIMITTEXT,(WPARAM)81,0); // ���� �����Է� ����Ʈ�� �ִ� ���� ����
		return true;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			size = GetDlgItemTextA(hDlg,IDE_PUZZLE,puzzle,_countof(puzzle));
			// ���� �� ������ ���� ó��
			if( size > 81 || size < 81)
			{
				MessageBox(hDlg,TEXT("�̰��� �ùٸ� ������ �ƴմϴ�."),TEXT("Sudoku Gomting"),MB_OK);
				::SetFocus(edit);
				return true;
			}
			else
			{
				// ���ڰ� ���ԵǾ� �ִ��� �˻�
				for(UINT i=0; i</*_tcslen*/strlen(puzzle); i++)
					if( (puzzle[i] < 48) || (puzzle[i] > 57) )
					{
						MessageBox(hDlg,TEXT("�̰��� �ùٸ� ������ �ƴմϴ�."),TEXT("Sudoku Gomting"),MB_OK);
						::SetFocus(edit);
						return true;
					}
			}
			// ������ ������ �ε��� ���¶�� �������� �ʱ�ȭ�ϰ� ���� �۾������� �����
			if( bLoaded )
			{
				pList->resetList();
				Initialize();
			}
			// �� �ۿ��� �������� ���ڷ� ��ȯ�� ���� ����
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
///////////////////////////////////////// About ���ν���  ///////////////////////////////////////// 
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
//												�Լ� ����												//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// CreateSudokuFont(HDC hdc, FONT_TYPE ft)
//
// ���� : ���񳻿��� ����� ����ü�� ����
//
// �μ� : ���ڸ� ������ �ϴ� ȭ���� DC, �׸���
//		   � ������ ���ڸ� ������ �����ϴ� ������ ���� �μ�,
//		   MAINFONT : ���񳻿� ǥ�õǴ� ���� ū ����
//		   SUBFONT : ���񳻿� �ĺ����� ǥ���ϴ� ���� ����
//		   TITLEFONT : ���� �ܰ��� ���ι�ȣ�� ���ĺ�
//		   SOLFONT : �ش��� ǥ���ϴ� ����
//
// ���ϰ� : ����
//
void CreateSudokuFont(HDC hdc, FONT_TYPE ft)
{
	mainFont = CreateFont(30,0,0,0,FW_NORMAL+400,0,0,0,HANGEUL_CHARSET,0,0,0,0,TEXT("����ü"));
	subFont =  CreateFont(10,0,0,0,FW_NORMAL,0,0,0,HANGEUL_CHARSET,0,0,0,0,TEXT("����ü"));
	titleFont = CreateFont(15,0,0,0,FW_NORMAL+400,0,0,0,ANSI_CHARSET,0,0,0,0,TEXT("Arial"));
	solFont = CreateFont(30,0,0,0,FW_NORMAL+400,0,0,0,HANGEUL_CHARSET,0,0,0,0,TEXT("����ü"));

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
// ���� : CreateSudokuFont�� ���� ������ ���ҽ��� �����Ѵ�
//
// �μ� : ȭ�� �ڵ�, ������ ����ü�� ����
//
// ���ϰ� : ����
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
// ���� : ������ �������� �׸���. �׷����� ������,
//		   Ÿ��Ʋ����->Ÿ��Ʋ ����->Y�� ��->X�� �� �����̴�.
//
// �μ� : ����� ȭ�� �ڵ�
//
// ���ϰ� : ����
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
	// �ܰ� Ÿ��Ʋ�� ĥ�ϱ� ���� ��� �귯�� ����
	CreateSudokuBrush(hdc,RGB(200,50,0));	
	CreateSudokuLine(hdc,RGB(200,50,0),pt);
	Rectangle(hdc,rightRT.left,rightRT.top,rightRT.right,rightRT.bottom);
	Rectangle(hdc,topRT.left,topRT.top,topRT.right,topRT.bottom);
	Rectangle(hdc,leftRT.left,leftRT.top,leftRT.right,leftRT.bottom);
	Rectangle(hdc,bottomRT.left,bottomRT.top,bottomRT.right,bottomRT.bottom);
	// �ܰ� Ÿ��Ʋ �簢�� ���� �׸� �� ��� �귯�� ����
	ReleaseSudokuLine(hdc);
	ReleaseSudokuBrush(hdc);
	// �ܰ� Ÿ��Ʋ �ȿ� �۾� ���
	CreateSudokuFont(hdc,TITLEFONT);

	for(int i=1; i<=9; i++)
	{
		/*_itot_s*/_itoa_s(i, ch, _countof(ch), 10);
		TextOutA(hdc,MIN_BOARD_X-20,(i*GAP)+17,ch,static_cast<int>(/*_tcslen*/strlen(ch))); // Side Number
		TextOutA(hdc,MIN_BOARD_X+((i-1)*GAP)+20,MIN_BOARD_Y-20,&al[i-1],1); // Top Alphabet
	}
	ReleaseSudokuFont(hdc,TITLEFONT);
	// ���� Grid �ܰ� �׵θ� �Ӽ� Set
	CreateSudokuLine(hdc,RGB(200,50,0),pt);
	// ���� Grid �ܰ� �׵θ�
	Rectangle(hdc,MIN_BOARD_X,MIN_BOARD_Y,MAX_BOARD_X,MAX_BOARD_Y);
	// Y�� �� ����
	for(int i=GAP; i<=(MAX_BOARD_X-GAP); i+=GAP)
	{
		MoveToEx(hdc,i,MIN_BOARD_X,NULL);
		// 3 x 3 Grid ��輱 �б�
		if( i == (GAP * 4) || i == (GAP * 7) )
		{
			ReleaseSudokuLine(hdc); pt.x = 2;
			CreateSudokuLine(hdc,RGB(200,50,0),pt);
			LineTo(hdc,i,MAX_BOARD_Y);
		}
		else
		{
			// 3 x 3 Grid ��輱�� �׸� ���Ŀ��� ���� ���·� ���� ������
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
	// X�� �� �߱� �� �� ���� ����
	ReleaseSudokuLine(hdc); pt.x = 1;
	CreateSudokuLine(hdc,RGB(200,50,0),pt);
	// X�� �� ����
	for(int j=GAP; j<=(MAX_BOARD_Y-GAP); j+=GAP)
	{
		MoveToEx(hdc,MIN_BOARD_X,j,NULL);
		// 3 x 3 Grid ��輱 �б�
		if( j == (GAP * 4) || j == (GAP * 7) )
		{
			ReleaseSudokuLine(hdc); pt.x = 2;
			CreateSudokuLine(hdc,RGB(200,50,0),pt);
			LineTo(hdc,MAX_BOARD_X,j);
		}
		else
		{
			// 3 x 3 Grid ��輱�� �׸� ���Ŀ��� ���� ���·� ���� ������
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
	// �׵θ� �׸��⿡ ����� ������Ʈ ���� �� ����
	ReleaseSudokuLine(hdc);
	ReleaseSudokuFont(hdc,TITLEFONT);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PrintAllSudoku(HDC hdc)
//
// ���� : ���� ������ ������ ����Ѵ�.
//
// �μ� : ����� ȭ�� �ڵ�
//
// ���ϰ� : ����
//
void PrintAllSudoku(HDC hdc)
{
	char ch[5] = {0,};

	CreateSudokuFont(hdc,MAINFONT);
	// y �� ���� ����
	for(int i=0; i<9; i++)
	{
		// x �� ��� ����
		for(int j=0; j<9; j++)
		{
			/*_itot_s*/_itoa_s(sudokuBoard[i][j],ch,_countof(ch),10);
			// ���ڰ� 0 ��, �Է��� �ȵ� ��쿡�� ������� �ʴ´�
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
// ���� : �ش� ������ ������ ����Ѵ�.
//
// �μ� : ����� ȭ�� �ڵ�
//
// ���ϰ� : ����
//
void PrintSolution(HDC hdc)
{
	char ch[5] = {0,};

	CreateSudokuFont(hdc,SOLFONT);
	// y �� ���� ����
	for(int i=0; i<9; i++)
	{
		// x �� ��� ����
		for(int j=0; j<9; j++)
		{
			/*_itot_s*/_itoa_s(solutionBoard[i][j], ch, _countof(ch), 10);
			// ���ڰ� 0 ��, �Է��� �ȵ� ��쿡�� ������� �ʴ´�
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
// ���� : �� ���� �ĺ��� ������ش�.
//
// �μ� : ����� ȭ�� �ڵ�
//
// ���ϰ� : ����
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
				// ���� ����Ϸ��� ĭ�� ���ڰ� ������ �ĺ����� ������� �ʴ´�
				if( solutionBoard[i][j] )
					break;

				/*_itot_s*/_itoa_s(sudokuNumber, ch, _countof(ch), 10);
				if( (sudokuCandy[i][j] ^ hyperDrive(sudokuNumber)) < sudokuCandy[i][j] )
					TextOutA(hdc,MIN_BOARD_X+(GAP*j)+incX,MIN_BOARD_Y+(GAP*i)+incY,ch,static_cast<int>(/*_tcslen*/strlen(ch)));
				// ���� �ĺ����� 3�� ��µǾ����� ����
				if( ((k+1) > 2) && ((k+1) % 3 == 0) )
				{
					incY += Y_GAP;
					incX = X_START;
				}
				else
					incX += X_GAP;
				// sudokuNumber ����
				sudokuNumber++;
			}
			// ���� ����� ���� ��¹�ȣ�� ��ǥ �ʱ�ȭ
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
// ���� : ��µ� ������ ���� ����� TRANSPARENT ���� �ϴ� ���ڻ� ����
//
// �μ� : ����� ȭ�� �ڵ� �� ���� ����
//
// ���ϰ� : ����
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
// ���� : hdc�� color������ �Ǽ��� weight ���� ��ŭ �����Ѵ�
//
// �μ� : ����� ȭ�� �ڵ� �� ����� ����
//
// ���ϰ� : ����
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
// ���� : CreateSudokuLine���� ���� ���� ���� �� ����
//
// �μ� : ����� ȭ�� �ڵ�
//
// ���ϰ� : ����
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
// ���� : Ÿ��Ʋ������ ������ ĥ�Ҷ� ����� �귯�ø� �����
//
// �μ� : ����� ȭ�� �ڵ� �� ���ϴ� ����
//
// ���ϰ� : ����
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
// ���� : CreateSudokuBrush�� ������ �귯�ø� ���� �� ����
//
// �μ� : ����� ȭ�� �ڵ�
//
// ���ϰ� : ����
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
// ���� : �־��� ������ �ش��� ã�´�
//
// �μ� : ����
//
// ���ϰ� : ������ �ش��� ã������ true�� �����Ѵ�
//
bool FindSolution()
{
	bool loop = true;
	bool InList = false;

	// ���ʷ� ��� �ĺ��� ã��
	FindAllCandy();
	// ���ʷ� �ĺ��� �ϳ� �ִ� ���� �ĺ� ����.. ���߿� ������ �����ؼ�
	// �Ϸ�ÿ��� ������ ���� �ȵǴ� ���� �����ΰ�� �ش����(false)�� �����Ѵ�
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
	// Ȯ������ ���ڵ��� ������ ã�´�. ���߿� ������ �� ã���� ���� ����...!
	// �����Ѱ� ���� ���� �� �����ϱ� ������....���� ���������� ���ϰ� �ִ�
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
	// ���� ���ʹ� ����Ʈ�� ����Ǵ� �۾��̴�
	InList = true;
	// ������ Ÿ�� ����� �������� ������ Ǭ��... Ǫ�� ������ ���� ������ ������
	// Ȯ������ ���� ���� -> Solo �ĺ� ��� ã�� -> Unique �ĺ� ��� ã�� ->
	// ������ ������ �����ؼ� ������ �������� true����, ���� ��Ȳ�̸� ���� �б������� ȸ���� ����
	// ��� �ݺ��ϴµ� ȸ���� ������ ������ ������ false ���� -> �� �ۿ� ó������ �ݺ�....
	while( loop )
	{
		// �б��� ����
		MarkCandyToJunction(TakeMostSafeCandy());
		// ��� ���� �ϳ��� �ĺ��� �� �ϳ��� �����ϴ� ���� ���� ã��(Solo)
		while(MarkSoloCandy(InList))
		{
			// �߰� ���� ����
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
		// �� �������� �ĺ��鿡 Ȧ�� �����ϴ� �ĺ��� ã�� ���� ä��(Unique)
		while(MarkUniqueCandy(InList))
		{
			// �߰� ���� ����
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
		// ���������� ���� �۾������ �̻��� ������ �ľ�
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
//	���� : solutionBoard �迭�� �����Ͽ� ��� ������ ���� �ĺ����� ã���ش�
//		   ã�� ������ 3x3Grid, ����(Horizontal), ����(Vertical) ���̰�, �� ��������
//		   ã�� ���ڴ� �ٸ� �������� ã�� ���ڿ� �ߺ����� �ʴ´�.
//
//  �μ� : ����
//
//  ���ϰ� : ����
//
void FindAllCandy()
{
	int target = 0;
	WORD newCandy = 0x3FE;
	// 3 x 3 Grid �ĺ����� ã��
	for(int i=0; i<9; i+=3)
	{
		for(int j=0; j<9; j+=3)
		{
			for(int k=i; k<i+3; k++)
			{
				for(int m=j; m<j+3; m++)
				{
					// 3x3 Grid ������ ���ڸ� ã������ �����Ѵ�
					if( solutionBoard[k][m] != NULL )
						newCandy ^= hyperDrive(solutionBoard[k][m]);
				}
			}
			// 3x3Grid ���� �ĺ����� �����Ѵ�. ��, ���� ��� ���� �б����̸�
			// ������ ��Ʈ�� 1�� ������ش�.
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
			// �ĺ��� ã��� ���� �ʱ�ȭ
			newCandy = 0x3FE;
		}
	}
	// �̻� 3x3 Grid �ĺ��� ã�� ��
	// �ĺ��� ã��� ���� �ʱ�ȭ
	newCandy = 0x3FE;
	// ���� �ĺ��� ã�� ����
	for(int i=0; i<9; i++)
	{
		for(int j=0; j<9; j++)
		{
			if( solutionBoard[i][j] != NULL )
				newCandy ^= hyperDrive(solutionBoard[i][j]);
		}
		// ���� �ĺ����� �����Ѵ�
		for(int k=0; k<9; k++)
		{
			// ���Ŵ�� ���� �б����̸� ���ŵǴ� �ĺ����� ������ ��Ʈ�� 1�� �߰����ش�
			if( (sudokuCandy[i][k] ^ hyperDrive(0)) < sudokuCandy[i][k] )
			{
				newCandy |= 1;
				sudokuCandy[i][k] &= newCandy;
				newCandy ^= 1;
			}
			else
				sudokuCandy[i][k] &= newCandy;
		}
		// ���� ���� �ĺ����� ã�� �� �ʱ�ȭ
		newCandy = 0x3FE;
	}
	// �̻� ���� �ĺ��� ã�� ��
	// ���� �ĺ����� ã�� �� �ʱ�ȭ
	newCandy = 0x3FE;
	// ���� �ĺ��� ã�� ����
	for(int i=0; i<9; i++)
	{
		for(int j=0; j<9; j++)
		{
			// �� ������ �������� �˻��ϸ� ���ڸ� ã��
			if( solutionBoard[j][i] != NULL )
				newCandy ^= hyperDrive(solutionBoard[j][i]);
		}
		for(int k=0; k<9; k++)
		{
			// ���Ŵ�� ���� �б����̸� ���ŵǴ� �ĺ����� ������ ��Ʈ�� 1�� �߰����ش�
			if( (sudokuCandy[k][i] ^ hyperDrive(0)) < sudokuCandy[k][i] )
			{
				newCandy |= 1;
				sudokuCandy[k][i] &= newCandy;
				newCandy ^= 1;
			}
			else
				sudokuCandy[k][i] &= newCandy;
		}
		// ���� ���� �ĺ����� ã�� �� �ʱ�ȭ
		newCandy = 0x3FE;
	}
	// �̻� ���� �ĺ��� ã�� ��
}
///////////////////////////////////////////////////////////////////////////////
//
// MarkSoloCandy(bool bWork)
//
// ���� : solutionBoard�迭�� �ִ� �����߿� �ĺ��� �� �ϳ��� �������ִ�
//		  ���� �ĺ��� �ش� ���� ������ ä���
//
// �μ� :  �۾�����Ʈ pList �� ��������� ���θ� �����Ѵ�. ��, �ĺ���
//		   �ϳ����ִ� ���̱� ������ �бⰡ �� �� �����Ƿ� ������ ������ ����̴�
//
// ���ϰ� :  ���ۿ� �ش��ϴ� ���� ���°�� false, �׿ܿ��� true
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
			// ���� ������� ���� ��쿣 �ĺ� ������ ���� ����
			if( solutionBoard[i][j] ) 
				continue;
			// ���� �ĺ� ������ ����
			for(int k=1; k<=9; k++)
			{
				if( (sudokuCandy[i][j] ^ hyperDrive(k)) < sudokuCandy[i][j] )
				{
					candyCount++;
					if(!saveCandy) saveCandy = k;
				}
			}
			// ���� �ĺ� ������ 1���� ��� ���� ������ ����Ѵ�
			// ��, �μ��� true �� ����Ʈ�� �۾������� �߰��Ѵ�
			if( candyCount == 1 )
			{
				if( bWork )
					pList->Insert(new Node(sudokuCandy[i][j],j,i));
				// �ش� �迭�ǿ� ���
				solutionBoard[i][j] = saveCandy;
				result = true;
			}
			// �� ī��Ʈ �ʱ�ȭ
			candyCount = 0;
			saveCandy = 0;
		}
	}
	// �۾��� ������ �ִٸ� ��ü �ĺ����� ���������ش�
	if( result ) FindAllCandy();
	return result;
}
///////////////////////////////////////////////////////////////////////////////
//
// MarkCandyToJunction(POINT pt)
//
// ���� : �μ� pt�� ��ġ�� �ִ� ������ �ĺ� �ϳ��� �����ϰ�
//		   �ش� ���� '�б���'���� �����Ѵ�. (��Ʈ �������� 1)
//		   ����, �� �۾��� ���۵� ������ ��� �۾��� ����Ʈ�� �߰��ȴ�
//
// �μ� : �б������� ������ ���� x,y�ּ�
//
// ���ϰ� : ����
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
			// pt �� ��ġ�� �б������� ����
			sudokuCandy[y][x] ^= hyperDrive(0);
			// ����Ʈ�� ���
			pList->Insert(new Node(sudokuCandy[y][x],x,y));
			// �� �Է�
			solutionBoard[y][x] = k;
			break;
		}
	}
	// ���������� ��ü �ĺ����� ����
	FindAllCandy();
}
///////////////////////////////////////////////////////////////////////////////
//
// MarkUniqueCandy()
//
// ���� : 1~9���� ���ư��鼭 3x3Grid, HOR, VER ������ �ĺ��� �� ���� �˻����� ���ڰ�
//		   ȥ�� ���� ���� ã�� �����Ѵ�
//
// �μ� :  ���� �۾��� ����Ʈ�� ��ϵ� ���� ����(true - ���, false - �̵��)		
//
// ���ϰ� :  �� �̻� ��ŷ�� Unique ���ڰ� ��� ������ �󿡼� �߰ߵ��� �ʴ� ��� false�� ����
//
bool MarkUniqueCandy(bool bWork)
{
	bool being = false;
	int savedSeq = 0;
	int candyCount = 0;
	// 3x3Grid�� Unique �ĺ� ã�� ����
	for(int p=0; p<9; p+=3)
		for(int q=0; q<9; q+=3)
		{
			for(int i=p; i<p+3; i++)
				for(int j=q; j<q+3; j++)
				{
					// ��ĭ�� ������� �ϹǷ� ���ڰ� �ִ� ĭ�� �����Ѵ�
					if( solutionBoard[i][j] ) 
						continue;
					// 1~9������ �ĺ��� �����ؼ� ���� �ش� ���ڰ� �ĺ��� �����ϸ�
					// ��ü ���� ������� �ش� ���ڸ� �ĺ��� ������ ���� �ִ��� ã��
					// � ������ �ش� ���ڰ� ���� ��쿡�� ���� ���� ���ڸ� ���
					for(int k=1; k<=9; k++)
					{
						// �ĺ��� �߰��� ��� ��ü ���� ������� ����
						if( (sudokuCandy[i][j] ^ hyperDrive(k)) < sudokuCandy[i][j] )
						{
							for(int n=p; n<p+3; n++)
								for(int m=q; m<q+3; m++)
								{
									// ���� ����� ������� ���� ���� ����� ���� ���ذ� ����
									// ���� ��쿡�� �����Ѵ�
									if( solutionBoard[n][m] || ((n==i)&&(m==j)) )
										continue;
									// ���ڰ� �ִ� ���..
									if( (sudokuCandy[n][m] ^ hyperDrive(k)) < sudokuCandy[n][m] )
										candyCount++;
								}
							// �ش� ���ڰ� �ڽſܿ� �� ���ٸ� ���� ���ڸ� ����Ѵ�
							// ���� ����Ʈ ���� ���ο� ���� ����Ʈ�� ������ش�
							if( !candyCount ) 
							{
								// �������� ĭ�� ���ڰ� ������ �����Ѵ�
								if( !solutionBoard[i][j] ) 
								{
									solutionBoard[i][j] = k;
									if( bWork )
										pList->Insert(new Node(sudokuCandy[i][j],j,i));
									being = true;
								}
							}
							// �������� �ʱ�ȭ
							candyCount = 0;
						}
					}
				}
			}
	// �̻� 3x3 Grid Unique �ĺ� ã�� ��
	// ��ü �ĺ��� ������ �� �ĺ����ڼ��� �ʱ�ȭ
	FindAllCandy();
	candyCount = 0;
	// Horizontal Line Unique �ĺ� ã�� ����
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
		{
			// ���� ���ڰ� �ִ� ��� �˻縦 �ǳʶ�
			if( solutionBoard[i][j] )
				continue;
			// ���� ���� �ĺ����� �����ؼ� �ĺ����� �߰ߵǸ� ���򼱻� �����ϴ�
			// �� ������ �ĺ��� �����ؼ� ���� �ĺ��� �ִ��� Ȯ���Ѵ�
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
						// �������� ĭ�� ���ڰ� ������ �����Ѵ�
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
	// �̻� Horizontal Unique �ĺ� ã�� ��
	// ��ü �ĺ��� ������ �� �ĺ����ڼ��� �ʱ�ȭ
	FindAllCandy();
	candyCount = 0;
	// Vertical Line �ĺ� ã�� ����
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
		{
			// ���� ���ڰ� �ִ� ��� �˻縦 �ǳʶ�
			if( solutionBoard[j][i] )
				continue;
			// ���� ���� �ĺ����� �����ؼ� �ĺ����� �߰ߵǸ� �������� �����ϴ�
			// �� ������ �ĺ��� �����ؼ� ���� �ĺ��� �ִ��� Ȯ���Ѵ�
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
						// �������� ĭ�� ���ڰ� ������ �����Ѵ�
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
	// Vertical Line �ĺ� ã�� ��
	// �۾��� ������ �ϳ��� ������ true�� �������ָ� ���������� ��ü����
	// �ĺ� �������� ���ֵ��� �Ѵ�
	if( being ) FindAllCandy();
	return being;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TakeMostSafeCandy()
//
// ���� : ��ü ������ �� ���� �߿� ���� ���� ���� �ĺ��� ���� ���� ��ǥ�� ã���ش�
//		   ��, ������ �ּ�Ȯ���� �������� 2�� �̻��� ��� ���� ã���� ���� ���������Ѵ�
//
// �μ� : ����
//
// ���ϰ� : ���� ���� ���� �ĺ��� ���� ���� x,y ��ǥ
//
POINT TakeMostSafeCandy()
{
	POINT pt;
	int candyCount[2] = {0,0};
	// ��ǥ�� �ʱ�ȭ
	pt.x = 0;
	pt.y = 0;

	// solutionBoard �� ������ Ȯ���� ���� ���� ���� ã�´�
	for( int i=0; i<9; i++)
	{
		for( int j=0; j<9; j++)
		{
			// �ش� ���� ���ڰ� �� ������ �ȵſ�!
			if( ! solutionBoard[i][j] )
			{
				for(int k=1; k<=9; k++)
				{
					// �ĺ��� ������ ����
					if( (sudokuCandy[i][j] ^ hyperDrive(k)) < sudokuCandy[i][j] )
						candyCount[0] = candyCount[0] + 1;
				}
				// �ĺ��� �󸶳� �ִ��� Ȯ���Ѵ�. [0]��°�� ���� �ĺ� ���ڸ�,
				// [1]��°�� �ּ��ĺ� ������ �����Ѵ�
				if( !candyCount[1] || (candyCount[0] < candyCount[1] ) )
				{
					candyCount[1] = candyCount[0];
					pt.x = j;
					pt.y = i;
				}
				// �ĺ� ������ �ʱ�ȭ�ؼ� ���� �ĺ��� ���� ���� �غ� �Ѵ�
				// �ĺ� ������ ���������� �ľ��ϴ� [1]�� �ʱ�ȭ���� �ʴ´�
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
// ���� : ������ �����ִ� ���� �ĺ����� ���¸� �ľ��ؼ� �̻������� �������ְ�
//		   ������ ���� ���θ� Ȯ�����ش�
//
// �μ� : ����
//
// ���ϰ� : ���� �̻��� �ִ� ��� WRONG, ���� ��� GOOD. ��, ���� ��ĭ�� ������
//			  FINISHED�� �������ش�.(��, ������ ����� ���)
//
WORKSET FindWrongCandy()
{
	WORD target = NULL;
	int IsFull = 0;
	int candyCount = 0;
	// ���� ���� ��ü ���� �� �� �������� �ľ��Ѵ�
	for(int i=0; i<9; i++)
		for(int j=0; j<9; j++)
		{
			if( solutionBoard[i][j] )
				IsFull++;
		}
	if( IsFull == 81 ) 
		return FINISHED;
	// 3x3Grid �̻� ���� ����
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
					// �ĺ��� �ϳ��� ���� ���̶�� �̻��� �߻��� ������ �����Ѵ�
					if( !candyCount ) return WRONG;
					// �ĺ��� �ϳ��� �����ϴ� ���� ��� ���� 3x3Grid ���� ��� �ִ� ���� ��
					// �ĺ��� �ϳ��� ������ ���� ���� �ĺ��� ������ �ִ��� �ľ��Ѵ�
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
					// ���� �˻縦 ���� �ʱ�ȭ
					candyCount = 0;
					target = NULL;
				}
		}
	// �̻� 3x3Grid �̻� ���� ���� ��
	// Horizontal Line �̻� ���� ���� ����
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
			// ���� �˻縦 ���� �ʱ�ȭ
			candyCount = 0;
			target = NULL;
		}
	// �̻� Horizontal Line �̻� ���� ���� ��
	// Vertical Line �̻� ���� ���� ����
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
			// ���� �˻縦 ���� �ʱ�ȭ
			candyCount = 0;
			target = NULL;
		}
	// �̻� Vertical Line �̻� ���� ���� ��
	return GOOD;
}
//////////////////////////////////////////////////////////////////////////////////////
//
// RestoreRecentJunction()
//
// ���� : ���� �ֱ��� �б��� ������ ���ư��鼭 �۾��ؿԴ� ������ ������ �����
//		   ���� �ֱ��� �б������� ���ư��� ���� �ĺ��� �����ϴµ�, ���� �ĺ��� ������
//		   �� ������ �б������� ���ư��鼭 ���� �۾��� �ݺ��Ѵ�. ���� �� �̻� ���ư� ��
//		   �ִ� ���� ������ ������ �����Ѵ�.
//
// �μ� : ����
//
// ���ϰ� : �� �̻� ���ư� �� �ִ� ���� ���°�� false�� ����
//
bool RestoreRecentJunction()
{
	int rx = 0;
	int ry = 0;
	bool loop = true;
	WORD candy = 0x3FE;
	// ����Ʈ �������� ��ġ�� ���� ���������� �д�
	pList->moveLast();

	while( loop )
	{
		// ���� �ֱ��� �б����� ���������� �۾��� ������ش�
		while(!(pList->getPointer()->IsJunction()))
		{
			rx = pList->getPointer()->getX();
			ry = pList->getPointer()->getY();
			candy = pList->getPointer()->GetCandidates();
			solutionBoard[ry][rx] = 0;
			sudokuCandy[ry][rx] = candy;
			pList->DeleteLastNode();
		}
		// �ֱ� �б����� ��ǥ�� ���� �ĺ��� ����
		rx = pList->getPointer()->getX();
		ry = pList->getPointer()->getY();
		candy = pList->getPointer()->GetCandidates();
		// �ֱ��� �б����� �������� �ĺ��� �ٲٷ��� �õ��Ѵ�
		for(int k=1; k<=9; k++)
		{
			// �ĺ��� �������� ���õǾ����� �ĺ����� �ľ��ϰ� �����Ѵ�
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
		// �̰��� �����ϸ� �ĺ��� �����ٴ� ���̹Ƿ� �� ������ �б������� ���ư��� �Ѵ�
		// �׷��� �� �̻� ���ư� �б����� ���ٸ� �����̴�...
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
// ���� : �� ������ �� �ĺ����� �ʱ�ȭ�Ѵ�.
//
// �μ� : ����
//
// ���ϰ� : ����
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
// ���� : �ش��� �����ִ� flag �� true�� �ϰ� hWnd �� �ٽ� �׸���
//
// �μ� : �ٽ� �׷����� �������� �ڵ�
//
// ���ϰ� : ����
//
void ShowSolution(HWND hWnd)
{
	if( bShowSolution ) 
		bShowSolution = false; 
	else
		bShowSolution = true;
	InvalidateRect(hWnd,NULL,TRUE);
}

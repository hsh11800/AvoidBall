//--------------------------------------------------------------------------------------
// File: ShootingGame.cpp
//
// This application demonstrates animation using matrix transformations
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <ctime>
#include <cstdio>
#include "resource.h"
#include "Graphics.h"
#include "GameObject.h"
#include "Stick.h"
#include "CircleObject.h"
#include "GameSimulator.h"
#include "Obstacle.h"
#include "UserCharactor.h"
#include "eGameLevel.h"
//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
bool					bOverMsged = false;
static bool s_gameStart = false;
//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;
	srand(static_cast<unsigned int>(time(NULL)));

	// Main message loop
	MSG msg = { 0 };

	GameSimulator* gameSimulator = GameSimulator::GetInstance();
	Graphics* graphics = Graphics::GetInstance(g_hInst, g_hWnd);
	
	gameSimulator->InitGame();
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (s_gameStart)
			{
				gameSimulator->Update();
			}

			graphics->Render();
			if (gameSimulator->IsGameOver()&& bOverMsged == false)
			{
				char msg[16];
				wchar_t wMsg[16]; 
				sprintf(msg, "%d balls", gameSimulator->GetObstacleCount());
				mbstowcs(wMsg, msg, 16);
				MessageBox(g_hWnd, wMsg, TEXT("End"), MB_OK);
				bOverMsged = true;
			}
		}
	}

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	GameSimulator* gameSimulator = GameSimulator::GetInstance();

	//gameSimulator->WndProc(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_CREATE:
	{
		int x, y, width, height;
		RECT rtDesk, rtWindow;
		GetWindowRect(GetDesktopWindow(), &rtDesk);
		GetWindowRect(hWnd, &rtWindow);
		width = rtWindow.right - rtWindow.left;
		height = rtWindow.bottom - rtWindow.top;
		x = (rtDesk.right - width) / 2;
		y = (rtDesk.bottom - height) / 2;

		MoveWindow(hWnd, x, y, width, height, TRUE);

	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{ // wParam의 low word에 resource id값이 날라온다.
		case ID_GAME_START:
			//MessageBox(hWnd, TEXT("Srart"), TEXT(""), MB_OK);
			s_gameStart = true;
			break;
		case ID_GAME_RESTART:
			gameSimulator->resetGame();
			bOverMsged = false;
			break;
		case ID_GAME_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_LEVEL_EASY:
			gameSimulator->ChangeGameLevel(eGameLevel::Easy);
			MessageBox(hWnd, TEXT("Easy"), TEXT("Level"), MB_OK);
			gameSimulator->resetGame();
			bOverMsged = false;
			break;
		case ID_LEVEL_NORMAL:
			gameSimulator->ChangeGameLevel(eGameLevel::Normal);
			MessageBox(hWnd, TEXT("Normal"), TEXT("Level"), MB_OK);
			gameSimulator->resetGame();
			bOverMsged = false;
			break;
		case ID_LEVEL_HARD:
			gameSimulator->ChangeGameLevel(eGameLevel::Hard);
			MessageBox(hWnd, TEXT("Hard"), TEXT("Level"), MB_OK);
			gameSimulator->resetGame();
			bOverMsged = false;
			break;
		case IDD_HELP_HOWTOPLAY:
			MessageBox(hWnd, TEXT("←→↑↓ : move ball (Blue)\nThe enemy (Red) moves to random direction in black block\nAs time goes by, the enemy increase\nAvoid enemy!"), TEXT("How To Play"), MB_OK);
			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		//키보드 입력 여기서 처리
		if (s_gameStart == false)
		{
			break;
		}
		if (gameSimulator->IsGameOver())
		{
#ifdef _DEBUG						
			switch (wParam)
			{
			case 'R':
				gameSimulator->mbGameOver = false;
			}
#endif
			break;
		}
		
		switch (wParam)
		{
#define MOVE_VALUE (0.04f)
		case VK_LEFT:
			gameSimulator->mUserCharactor->Move(-MOVE_VALUE, 0.0f, 0.0f);
			break;
		case VK_RIGHT:
			gameSimulator->mUserCharactor->Move(MOVE_VALUE, 0.0f, 0.0f);
			break;
		case VK_UP:
			gameSimulator->mUserCharactor->Move(0.0f, MOVE_VALUE, 0.0f);
			break;
		case VK_DOWN:
			gameSimulator->mUserCharactor->Move(0.0f, -MOVE_VALUE, 0.0f);
			break;
		default:
			break;
		}
		break;

	case WM_SIZE:
		//창이 변경됨
		switch (wParam)
		{
		case SIZE_MAXHIDE:
		
			break;
		case SIZE_RESTORED:
			//intentional fallthrough
		case SIZE_MAXIMIZED:
		{
			GameObject** const gameObjects = gameSimulator->mGameObjects;
			const UINT gameObjectCount = gameSimulator->mGameObjectCount;
			for (GameObject** p = gameObjects; p < gameObjects + gameObjectCount; ++p)
			{
				ASSERT(p != nullptr, "");
				ASSERT(*p != nullptr, "");
				(*p)->ResetProjection();
			}
		}
			break;
		case SIZE_MAXSHOW:
			break;
		case SIZE_MINIMIZED:
			break;
		
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); //menu resorce add
	wcex.lpszClassName = L"WindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"WindowClass", L"Avoid The Ball", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	Graphics* graphics = Graphics::GetInstance(g_hInst, g_hWnd);
	return S_OK;
}





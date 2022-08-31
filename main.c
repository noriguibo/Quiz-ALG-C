#include <windows.h>
#include <wingdi.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "resource.h"

const char g_szClassName[] = "Quiz";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/* ===========
*  Functions
* ============
*/
void randomQuestion();
void questHandler(int questid, HWND hwnd);
void selectQuestion(HWND hwnd);
void checkAnswer(int questid, HWND hwnd);
void finishQuiz(HWND hwnd);
void controlDisplay(int type, HWND hwnd);
void CenterWindow(HWND);'

/* ==============================
*  GUI / Static Controls
* ===============================
*/
HWND hwndgroup, hwndresp[3], hwndselec[4], hwndquest, hwndstatus, hwndstart, hwndconfirm, hwndlogo, hwndcorrect, hwndincorrect, hwndnext, hwndlast, hwndexit;

/* =================
*  Global Variables
* ==================
*/
int s = -1, status = -1, total, total2 = 5;
int ca[5] = { 3, 0, 2, 3, 0 }, questid[5];

/* ========================
*  Initialize Main Window
* =========================
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow) {

    MSG  msg;
    WNDCLASSEX wc;
    HWND hwnd;

    //Main Window Properties
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW+2);
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    //Window registration failed?
    if(!RegisterClassEx(&wc)){
        MessageBox(NULL, "Fail! (Register Class)", "Error", MB_OK);
        return 0;
    }

    //Trying to create the main window...
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, g_szClassName, "Quiz - ALP", WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 780, 500, NULL, NULL, hInstance, NULL);

    //It fails?
    if (hwnd == NULL){
        MessageBox(NULL, "Fail! hwnd is NULL!", "Error", MB_OK);
        return 0;
    }

    //Everything OK!
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

/* =============================
*  Event Handler / Draw Window
* ==============================
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam) {

    switch(msg) {
        case WM_CREATE:
            CenterWindow(hwnd);

            HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);

            //Logo
            hwndlogo = CreateWindowW(L"Static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, 125, 120, 500, 60, hwnd, (HMENU)IDB_LOGO, hInstance, NULL);
            SendMessageW(hwndlogo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\logo.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            //Last Message
            hwndlast = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 125, 120, 500, 60, hwnd, (HMENU)IDB_LAST, hInstance, NULL);

            //Quiz Start Button
            hwndstart = CreateWindowW(L"Static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, 295, 285, 185, 75, hwnd, (HMENU)IDB_START, hInstance, NULL);
            SendMessageW(hwndstart, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\buttons\\startquiz.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            //Exit Button
            hwndexit = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP | SS_NOTIFY, 295, 285, 185, 75, hwnd, (HMENU)IDB_EXIT, hInstance, NULL);

            //Group Box
            hwndgroup = CreateWindowW(L"Button", NULL, WS_CHILD | BS_GROUPBOX, 10, 10, 545, 430, hwnd, (HMENU)IDB_GROUP, hInstance, NULL);

            //Question
            hwndquest = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 33, 40, 500, 60, hwnd, (HMENU)IDB_QUEST, hInstance, NULL);

            //Progress
            hwndstatus = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 605, 19, 100, 60, hwnd, (HMENU)IDB_STATUS, hInstance, NULL);

            //Correct Answer
            hwndcorrect = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 582, 149, 100, 60, hwnd, (HMENU)IDB_CORRECT, hInstance, NULL);
            SendMessageW(hwndcorrect, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\correct.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            //Incorrect Answer
            hwndincorrect = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 582, 149, 100, 60, hwnd, (HMENU)IDB_INCORRECT, hInstance, NULL);
            SendMessageW(hwndincorrect, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\incorrect.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            //Next Question
            hwndnext = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP | SS_NOTIFY, 565, 300, 100, 60, hwnd, (HMENU)IDB_NEXTQUEST, hInstance, NULL);
            SendMessageW(hwndnext, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\buttons\\nextquestion.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            //Non Selected Answer
            hwndresp[0] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP | SS_NOTIFY, 33, 120, 500, 60, hwnd, (HMENU)IDB_ANSWER1, hInstance, NULL);
            hwndresp[1] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP | SS_NOTIFY, 33, 200, 500, 60, hwnd, (HMENU)IDB_ANSWER2, hInstance, NULL);
            hwndresp[2] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP | SS_NOTIFY, 33, 280, 500, 60, hwnd, (HMENU)IDB_ANSWER3, hInstance, NULL);
            hwndresp[3] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP | SS_NOTIFY, 33, 360, 500, 60, hwnd, (HMENU)IDB_ANSWER4, hInstance, NULL);

            //Selected Answer
            hwndselec[1] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 33, 120, 500, 60, hwnd, (HMENU)IDB_SELECT1, hInstance, NULL);
            hwndselec[2] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 33, 200, 500, 60, hwnd, (HMENU)IDB_SELECT2, hInstance, NULL);
            hwndselec[3] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 33, 280, 500, 60, hwnd, (HMENU)IDB_SELECT3, hInstance, NULL);
            hwndselec[4] = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP, 33, 360, 500, 60, hwnd, (HMENU)IDB_SELECT4, hInstance, NULL);

            //Confirm Button
            hwndconfirm = CreateWindowW(L"Static", NULL, WS_CHILD | SS_BITMAP | SS_NOTIFY, 565, 380, 70, 40, hwnd, (HMENU)IDB_CONFIRM, hInstance, NULL);
            SendMessageW(hwndconfirm, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\buttons\\confirm.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            randomQuestion();
            break;
        case WM_COMMAND:
            //WM_COMMAND received from the Static Controls (SS_NOTIFY)
            switch(LOWORD(wParam)){
                case IDB_EXIT:
                    DestroyWindow(hwnd);
                    break;
                case IDB_NEXTQUEST:
                    //Have the user finished the quiz?
                    if (status >= (total2-1)) {
                        controlDisplay(99, hwnd);
                        break;
                    }
                    status++;
                    controlDisplay(2, hwnd);
                    questHandler(questid[status], hwnd);
                    break;
                case IDB_CONFIRM:
                    if(s == -1)
                        break;
                    checkAnswer(questid[status], hwnd);
                    break;
                case IDB_START:
                    //Increase Progress
                    status++;

                    controlDisplay(0, hwnd);
                    //Pick a question (Todo: Make it random)
                    questHandler(questid[status], hwnd);
                    break;
                case IDB_ANSWER1:
                    s = 0;
                    selectQuestion(hwnd);
                    break;
                case IDB_ANSWER2:
                    s = 1;
                    selectQuestion(hwnd);
                    break;
                case IDB_ANSWER3:
                    s = 2;
                    selectQuestion(hwnd);
                    break;
                case IDB_ANSWER4:
                	s = 3;
                    selectQuestion(hwnd);
                    break;
                return 0;
            }
            break;
        //Exiting...
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/* ==========================
*  Randomizes the questions
* ===========================
*/
void randomQuestion(){
    int i, j, t;

    srand(time(NULL));

    for (i = 0; i < 5; i++)
        questid[i] = i;

    for (i = 0; i < 5; i++){
        j = rand() % 5;
        t = questid[i];
        questid[i] = questid[j];
        questid[j] = t;
    }
}

/* ==============================================
*  Hide and Show selected / non-selected answers
*  based on the integer given.
* ===============================================
*/
void selectQuestion(HWND hwnd){
    for (int c = 0; c < 4; c++){
        if (c == s){
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER1+c), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT1+c), SW_SHOW);
        } else {
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER1+c), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT1+c), SW_HIDE);
        }
    }
}

/* =====================================================
*  Sends the correct image to the static controls based
*  on the integer (question) given.
* ======================================================
*/
void questHandler(int questid, HWND hwnd) {
    s = -1;
    switch (status){
        case 0:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\1-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 1:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\2-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 2:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\3-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 3:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\4-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 4:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\5-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 5:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\6-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 6:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\7-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 7:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\8-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 8:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\9-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 9:
            SendMessageW(GetDlgItem(hwnd, IDB_STATUS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\status\\10-10.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
    }
	switch (questid){
		case 0: {
		    SendMessageW(GetDlgItem(hwnd, IDB_QUEST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\question.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\r1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\r2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\r3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\r4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            SendMessageW(GetDlgItem(hwnd, IDB_SELECT1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\s1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\s2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\s3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\1\\s4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
        }
		break;
		case 1: {
		    SendMessageW(GetDlgItem(hwnd, IDB_QUEST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\question.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\r1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\r2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\r3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\r4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            SendMessageW(GetDlgItem(hwnd, IDB_SELECT1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\s1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\s2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\s3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\2\\s4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
        }
		break;
		case 2: {
		    SendMessageW(GetDlgItem(hwnd, IDB_QUEST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\question.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\r1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\r2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\r3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\r4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            SendMessageW(GetDlgItem(hwnd, IDB_SELECT1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\s1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\s2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\s3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\3\\s4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
        }
		break;
		case 3: {
		    SendMessageW(GetDlgItem(hwnd, IDB_QUEST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\question.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\r1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\r2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\r3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\r4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            SendMessageW(GetDlgItem(hwnd, IDB_SELECT1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\s1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\s2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\s3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\4\\s4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
        }
		break;
		case 4: {
		    SendMessageW(GetDlgItem(hwnd, IDB_QUEST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\question.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\r1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\r2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\r3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_ANSWER4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\r4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

            SendMessageW(GetDlgItem(hwnd, IDB_SELECT1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\s1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\s2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\s3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
			SendMessageW(GetDlgItem(hwnd, IDB_SELECT4), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\questions\\5\\s4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
        }
		break;
	}
	controlDisplay(1, hwnd);
}

/* =======================================
*  Check if the answer is correct or not
*  and act accordingly
* ========================================
*/
void checkAnswer(int questid, HWND hwnd){
    if (s == ca[questid]){
        total++;
        ShowWindow(GetDlgItem(hwnd, IDB_CORRECT), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDB_INCORRECT), SW_HIDE);
    } else {
        ShowWindow(GetDlgItem(hwnd, IDB_INCORRECT), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDB_CORRECT), SW_HIDE);
    }
    ShowWindow(GetDlgItem(hwnd, IDB_CONFIRM), SW_HIDE);
    ShowWindow(GetDlgItem(hwnd, IDB_NEXTQUEST), SW_SHOW);
}

/* ==========================================
*  Hide / Show controls based on the request
* ===========================================
*/
void controlDisplay(int type, HWND hwnd)
{
    switch(type){
        case 0:
            ShowWindow(GetDlgItem(hwnd, IDB_LOGO), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_START), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_CONFIRM), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_QUEST), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_GROUP), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_STATUS), SW_SHOW);
            break;
        case 1:
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER1), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER2), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER3), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER4), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT1), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT2), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT3), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT4), SW_HIDE);
            break;
        case 2:
            ShowWindow(GetDlgItem(hwnd, IDB_CONFIRM), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDB_NEXTQUEST), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_CORRECT), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_INCORRECT), SW_HIDE);
            break;
        case 99:
            ShowWindow(GetDlgItem(hwnd, IDB_CONFIRM), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_QUEST), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_GROUP), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_STATUS), SW_HIDE);

            ShowWindow(GetDlgItem(hwnd, IDB_CORRECT), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_INCORRECT), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_NEXTQUEST), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER1), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER2), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER3), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_ANSWER4), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT1), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT2), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT3), SW_HIDE);
            ShowWindow(GetDlgItem(hwnd, IDB_SELECT4), SW_HIDE);

            finishQuiz(hwnd);
        break;
    }
}

/* =============================================
*  Check / display how many questions the user
*  got right and hide the Quiz GUI
* ==============================================
*/
void finishQuiz(HWND hwnd){
    if (total == total2)
        total = 99;

    switch(total){
        case 0:
            SendMessageW(GetDlgItem(hwnd, IDB_LAST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\points\\0.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 1:
            SendMessageW(GetDlgItem(hwnd, IDB_LAST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\points\\1.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 2:
            SendMessageW(GetDlgItem(hwnd, IDB_LAST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\points\\2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 3:
            SendMessageW(GetDlgItem(hwnd, IDB_LAST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\points\\3.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 4:
            SendMessageW(GetDlgItem(hwnd, IDB_LAST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\points\\4.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 5:
            SendMessageW(GetDlgItem(hwnd, IDB_LAST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\points\\5.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
        case 99:
            SendMessageW(GetDlgItem(hwnd, IDB_LAST), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\points\\all.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));
            break;
    }

    SendMessageW(GetDlgItem(hwnd, IDB_EXIT), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(LoadImageW(NULL, L"resources\\buttons\\exit.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE)));

    ShowWindow(GetDlgItem(hwnd, IDB_LAST), SW_SHOW);
    ShowWindow(GetDlgItem(hwnd, IDB_EXIT), SW_SHOW);
}

/* =======================================
*  Set Window position at screen center
* ========================================
*/
void CenterWindow(HWND hwnd) {

    RECT rc = {0};

    GetWindowRect(hwnd, &rc);
    int win_w = rc.right - rc.left;
    int win_h = rc.bottom - rc.top;

    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);

    SetWindowPos(hwnd, HWND_TOP, (screen_w - win_w)/2,
        (screen_h - win_h)/2, 0, 0, SWP_NOSIZE);
}

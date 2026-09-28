//Library Includes
#include <windows.h>

#define ID_NEXT_BUTTON 101

//Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SecondWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateMainWindow(HINSTANCE hInstance);
HWND CreateSecondWindow(HINSTANCE hInstance);
void CreateText(HWND parent, HINSTANCE hInstance);
void CreateNextButton(HWND parent, HINSTANCE hInstance);

static HBRUSH gBackgroundBrush = CreateSolidBrush(RGB(255, 192, 203));

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR LpCmdLine, int nCmdShow){
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("OrderingSystem");
    wc.hbrBackground = gBackgroundBrush;
    RegisterClass(&wc);

    WNDCLASS secondClass = {};
    secondClass.lpfnWndProc = SecondWindowProc;
    secondClass.hInstance = hInstance;
    secondClass.lpszClassName = TEXT("Login&RegistrationPage");
    secondClass.hbrBackground = gBackgroundBrush;
    RegisterClass(&secondClass);

    HWND hwnd = CreateMainWindow(hInstance);
    CreateText(hwnd, hInstance);
    CreateNextButton(hwnd, hInstance);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while(GetMessage(&msg, nullptr, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(gBackgroundBrush);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, gBackgroundBrush);
            return TRUE;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(80, 30, 50));
            return (LRESULT)gBackgroundBrush;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_NEXT_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                ShowWindow(hwnd, SW_HIDE);
                HWND nextWindow = CreateSecondWindow(GetModuleHandle(nullptr));
                ShowWindow(nextWindow, SW_SHOW);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SecondWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND CreateMainWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("OrderingSystem"),
        TEXT("FLOWER SHOP ORDERING SYSTEM"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

HWND CreateSecondWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("Login&RegistrationPage"),
        TEXT("Login and Registration"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        700,
        500,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateText(HWND parent, HINSTANCE hInstance){
    HWND label = CreateWindowEx(
        0,
        TEXT("STATIC"),
        TEXT("FLOWER SHOP ORDERING SYSTEM"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        200, 100, 370, 40,
        parent,
        nullptr,
        hInstance,
        nullptr
    );

    HFONT font = CreateFont(
        30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI"));

    SendMessage(label, WM_SETFONT, (WPARAM)font, TRUE);
}

void CreateNextButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("PROCEED"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        330, 200, 120, 40,
        parent,
        (HMENU)ID_NEXT_BUTTON,
        hInstance,
        nullptr
    );
}

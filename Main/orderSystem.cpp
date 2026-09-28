//Library Includes
#include <windows.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <gdiplus.h>

using namespace Gdiplus;


#define ID_NEXT_BUTTON 101
#define ID_LOGIN_BUTTON 102
#define ID_REGISTER_BUTTON 103
#define ID_LOGIN_BACK_BUTTON 104
#define ID_REGISTER_BACK_BUTTON 105
#define ID_LOGIN_SUBMIT_BUTTON 106
#define ID_REGISTER_SUBMIT_BUTTON 107
#define ID_MAIN_MENU_EXIT_BUTTON 108
#define ID_MAIN_MENU_LOG_OUT_BUTTON 109
#define ID_PRODUCT_BUTTON_BASE 2000


//Function Declarations
static std::string GetProductImagePath(const std::string& productId);
static HBITMAP CreateDefaultProductBitmap(const std::string& productName, int width, int height);
static HBITMAP LoadProductBitmap(const std::string& productId, const std::string& productName);
LRESULT CALLBACK LandingWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LoginRegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LogInWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainMenuWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateLandingWindow(HINSTANCE hInstance);
HWND CreateLoginRegisterWindow(HINSTANCE hInstance);
HWND CreateLogInWindow(HINSTANCE hInstance);
HWND CreateRegisterWindow(HINSTANCE hInstance);
HWND CreateMainMenuWindow(HINSTANCE hInstance);
void CreateTitleText(HWND parent, HINSTANCE hInstance);
void CreateNextButton(HWND parent, HINSTANCE hInstance);
void CreateLogInButton(HWND parent, HINSTANCE hInstance);
void CreateRegisterButton(HWND parent, HINSTANCE hInstance);
void CreateLogInBackButton(HWND parent, HINSTANCE hInstance);
void CreateRegisterBackButton(HWND parent, HINSTANCE hInstance);
void CreateLogInUsernameLabel(HWND parent, HINSTANCE hInstance);
void CreateLogInPasswordLabel(HWND parent, HINSTANCE hInstance);
void CreateLogInUsernameInput(HWND parent, HINSTANCE hInstance);
void CreateLogInPasswordInput(HWND parent, HINSTANCE hInstance);
void CreateLogInSubmitButton(HWND parent, HINSTANCE hInstance);
void CreateRegisterUsernameLabel(HWND parent, HINSTANCE hInstance);
void CreateRegisterPasswordLabel(HWND parent, HINSTANCE hInstance);
void CreateRegisterUsernameInput(HWND parent, HINSTANCE hInstance);
void CreateRegisterPasswordInput(HWND parent, HINSTANCE hInstance);
void CreateRegisterSubmitButton(HWND parent, HINSTANCE hInstance);
void CreateLogInForm(HWND parent, HINSTANCE hInstance);
void CreateRegisterForm(HWND parent, HINSTANCE hInstance);
void CreateProductGrid(HWND parent, HINSTANCE hInstance);
void CreateMainMenuExitButton(HWND parent, HINSTANCE hInstance);
void CreateMainMenuLogOutButton(HWND parent, HINSTANCE hInstance);
void CreateMainMenuForm(HWND parent, HINSTANCE hInstance);
void MainMenuExit(HWND hwnd);
void LoadUsers();
void LoadProducts();
std::string ToString(const TCHAR* str);

//Static Variables
static HBRUSH gBackgroundBrush = CreateSolidBrush(RGB(255, 192, 203));
static int gLandingWindowX = 0;
static int gLandingWindowY = 0;
static int gLoginRegisterWindowX = 0;
static int gLoginRegisterWindowY = 0;
static HWND gLoginUsernameEdit = nullptr;
static HWND gLoginPasswordEdit = nullptr;
static HWND gRegisterUsernameEdit = nullptr;
static HWND gRegisterPasswordEdit = nullptr;
static int gProductScrollPos = 0;
static ULONG_PTR gGdiplusToken = 0;

//Global Variables
std::map<std::string, std::string> userMap;
std::vector<std::string> gProductNames;
std::vector<std::string> gProductIds;
std::string currentUser;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR LpCmdLine, int nCmdShow){
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gGdiplusToken, &gdiplusStartupInput, nullptr);

    LoadUsers();
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    gLandingWindowX = (screenW - 800) / 2;
    gLandingWindowY = (screenH - 600) / 2;
    gLoginRegisterWindowX = (screenW - 800) / 2;
    gLoginRegisterWindowY = (screenH - 600) / 2;

    WNDCLASS lwc = {};
    lwc.lpfnWndProc = LandingWindowProc;
    lwc.hInstance = hInstance;
    lwc.lpszClassName = TEXT("LandingPage");
    lwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&lwc);

    WNDCLASS lrwc = {};
    lrwc.lpfnWndProc = LoginRegisterWindowProc;
    lrwc.hInstance = hInstance;
    lrwc.lpszClassName = TEXT("Login&RegistrationPage");
    lrwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&lrwc);

    WNDCLASS lwc2 ={};
    lwc2.lpfnWndProc = LogInWindowProc;
    lwc2.hInstance = hInstance;
    lwc2.lpszClassName = TEXT("LoginPage");
    lwc2.hbrBackground = gBackgroundBrush;
    RegisterClass(&lwc2);

    WNDCLASS rwc = {};
    rwc.lpfnWndProc = RegisterWindowProc;
    rwc.hInstance = hInstance;
    rwc.lpszClassName = TEXT("RegisterPage");
    rwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&rwc);

    WNDCLASS mmwc = {};
    mmwc.lpfnWndProc = MainMenuWindowProc;
    mmwc.hInstance = hInstance;
    mmwc.lpszClassName = TEXT("MainMenuPage");
    mmwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&mmwc);

    HWND hwnd = CreateLandingWindow(hInstance);
    CreateTitleText(hwnd, hInstance);
    CreateNextButton(hwnd, hInstance);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while(GetMessage(&msg, nullptr, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(gBackgroundBrush);
    GdiplusShutdown(gGdiplusToken);
    return 0;
}


static std::string GetProductImagePath(const std::string& productId) {
    std::string cleanedId = productId;
    if (cleanedId.empty()) {
        cleanedId = "001";
    }
    cleanedId.erase(std::remove_if(cleanedId.begin(), cleanedId.end(), [](unsigned char ch) { return !std::isdigit(ch); }), cleanedId.end());
    if (cleanedId.empty()) {
        cleanedId = "001";
    }

    std::string path = "Product Informations/" + cleanedId + ".png";
    std::ifstream file(path.c_str());
    if (!file) {
        path = "Product Informations/default.png";
    }
    return path;
}

static HBITMAP CreateDefaultProductBitmap(const std::string& productName, int width, int height) {
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

    HBRUSH bgBrush = CreateSolidBrush(RGB(255, 224, 230));
    HBRUSH accentBrush = CreateSolidBrush(RGB(255, 192, 203));
    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(168, 85, 110));
    HGDIOBJ oldBrush = SelectObject(memDC, bgBrush);
    HGDIOBJ oldPen = SelectObject(memDC, borderPen);

    Rectangle(memDC, 0, 0, width, height);
    SelectObject(memDC, accentBrush);
    Rectangle(memDC, 5, 5, width - 5, height - 5);
    SelectObject(memDC, oldBrush);
    SelectObject(memDC, oldPen);

    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(100, 40, 60));
    HFONT font = CreateFontA(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(memDC, font);
    TextOutA(memDC, 20, 32, productName.c_str(), (int)productName.length());
    SelectObject(memDC, oldFont);
    DeleteObject(font);

    DeleteObject(bgBrush);
    DeleteObject(accentBrush);
    DeleteObject(borderPen);
    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);

    return bitmap;
}

static HBITMAP LoadProductBitmap(const std::string& productId, const std::string& productName) {
    std::string path = GetProductImagePath(productId);
    std::wstring widePath(path.begin(), path.end());

    Gdiplus::Bitmap image(widePath.c_str());
    if (image.GetLastStatus() == Gdiplus::Ok) {
        HBITMAP bitmap = nullptr;
        image.GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &bitmap);
        if (bitmap) {
            return bitmap;
        }
    }

    return CreateDefaultProductBitmap(productName, 140, 140);
}


LRESULT CALLBACK LandingWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
                HWND nextWindow = CreateLoginRegisterWindow(GetModuleHandle(nullptr));
                CreateLogInButton(nextWindow, GetModuleHandle(nullptr));
                CreateRegisterButton(nextWindow, GetModuleHandle(nullptr));
                ShowWindow(nextWindow, SW_SHOW);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LoginRegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_LOGIN_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                ShowWindow(hwnd, SW_HIDE);
                HWND loginWindow = CreateLogInWindow(GetModuleHandle(nullptr));
                CreateLogInForm(loginWindow, GetModuleHandle(nullptr));
                CreateLogInBackButton(loginWindow, GetModuleHandle(nullptr));
                ShowWindow(loginWindow, SW_SHOW);
            } else if (LOWORD(wParam) == ID_REGISTER_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                ShowWindow(hwnd, SW_HIDE);
                HWND registerWindow = CreateRegisterWindow(GetModuleHandle(nullptr));
                CreateRegisterForm(registerWindow, GetModuleHandle(nullptr));
                CreateRegisterBackButton(registerWindow, GetModuleHandle(nullptr));
                ShowWindow(registerWindow, SW_SHOW);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LogInWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
            if (LOWORD(wParam) == ID_LOGIN_BACK_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                ShowWindow(hwnd, SW_HIDE);
                HWND loginRegisterWindow = CreateLoginRegisterWindow(GetModuleHandle(nullptr));
                CreateLogInButton(loginRegisterWindow, GetModuleHandle(nullptr));
                CreateRegisterButton(loginRegisterWindow, GetModuleHandle(nullptr));
                ShowWindow(loginRegisterWindow, SW_SHOW);
            } else if (LOWORD(wParam) == ID_LOGIN_SUBMIT_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                TCHAR username[128] = {};
                TCHAR password[128] = {};

                GetWindowText(gLoginUsernameEdit, username, 128);
                GetWindowText(gLoginPasswordEdit, password, 128);

                if (username[0] == '\0' || password[0] == '\0') {
                    MessageBox(hwnd, TEXT("Please enter username and password."), TEXT("Login"), MB_OK);
                }
                else {
                    std::string usernameStr = ToString(username);
                    std::string passwordStr = ToString(password);

                    auto it = userMap.find(usernameStr);
                    if (it != userMap.end() && it->second == passwordStr) {
                        ShowWindow(hwnd, SW_HIDE);
                        HWND mainMenuWindow = CreateMainMenuWindow(GetModuleHandle(nullptr));
                        CreateMainMenuForm(mainMenuWindow, GetModuleHandle(nullptr));
                        ShowWindow(mainMenuWindow, SW_SHOW);
                        currentUser = usernameStr;
                    } else {
                        MessageBox(hwnd, TEXT("Invalid username or password."), TEXT("Login"), MB_OK);
                    }
                }
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK RegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
            if (LOWORD(wParam) == ID_REGISTER_BACK_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                ShowWindow(hwnd, SW_HIDE);
                HWND loginRegisterWindow = CreateLoginRegisterWindow(GetModuleHandle(nullptr));
                CreateLogInButton(loginRegisterWindow, GetModuleHandle(nullptr));
                CreateRegisterButton(loginRegisterWindow, GetModuleHandle(nullptr));
                ShowWindow(loginRegisterWindow, SW_SHOW);
            } else if (LOWORD(wParam) == ID_REGISTER_SUBMIT_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                TCHAR username[128] = {};
                TCHAR password[128] = {};

                GetWindowText(gRegisterUsernameEdit, username, 128);
                GetWindowText(gRegisterPasswordEdit, password, 128);

                if (username[0] == '\0' || password[0] == '\0') {
                    MessageBox(hwnd, TEXT("Please enter username and password."), TEXT("Register"), MB_OK);
                } else {
                    std::string usernameStr = ToString(username);
                    std::string passwordStr = ToString(password);

                    if (userMap.find(usernameStr) != userMap.end()) {
                        MessageBox(hwnd, TEXT("Username already exists."), TEXT("Register"), MB_OK);
                    } else {
                        userMap[usernameStr] = passwordStr;

                        std::ofstream file("users&passwords.txt", std::ios::app);
                        if (file) {
                            file << usernameStr << "|" << passwordStr << "\n";
                            file.close();
                            ShowWindow(hwnd, SW_HIDE);
                            HWND mainMenuWindow = CreateMainMenuWindow(GetModuleHandle(nullptr)); 
                            CreateMainMenuForm(mainMenuWindow, GetModuleHandle(nullptr));
                            ShowWindow(mainMenuWindow, SW_SHOW);
                            currentUser = usernameStr;
                        } else {
                            MessageBox(hwnd, TEXT("Error saving user data."), TEXT("Register"), MB_OK);
                        }
                    }
                }
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainMenuWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_MAIN_MENU_EXIT_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                MainMenuExit(hwnd);
            } else if (LOWORD(wParam) == ID_MAIN_MENU_LOG_OUT_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                ShowWindow(hwnd, SW_HIDE);
                HWND loginRegisterWindow = CreateLoginRegisterWindow(GetModuleHandle(nullptr));
                CreateLogInButton(loginRegisterWindow, GetModuleHandle(nullptr));
                CreateRegisterButton(loginRegisterWindow, GetModuleHandle(nullptr));
                ShowWindow(loginRegisterWindow, SW_SHOW);
            } else if (HIWORD(wParam) == BN_CLICKED) {
                int controlId = LOWORD(wParam);
                if (controlId >= ID_PRODUCT_BUTTON_BASE && controlId < ID_PRODUCT_BUTTON_BASE + (int)gProductNames.size()) {
                    int productIndex = controlId - ID_PRODUCT_BUTTON_BASE;
                    std::string productName = gProductNames[productIndex];
                    std::string details = "Selected flower: " + productName + "\n\nThis is a sample product detail window.";
                    MessageBoxA(hwnd, details.c_str(), "Flower Details", MB_OK | MB_ICONINFORMATION);
                }
            }
            return 0;

        case WM_VSCROLL: {
            int currentPos = gProductScrollPos;
            int newPos = currentPos;
            int minPos = 0;
            int maxPos = 0;
            GetScrollRange(hwnd, SB_VERT, &minPos, &maxPos);

            switch (LOWORD(wParam)) {
                case SB_LINEUP:
                    newPos -= 20;
                    break;
                case SB_LINEDOWN:
                    newPos += 20;
                    break;
                case SB_PAGEUP:
                    newPos -= 80;
                    break;
                case SB_PAGEDOWN:
                    newPos += 80;
                    break;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    newPos = HIWORD(wParam);
                    break;
                case SB_TOP:
                    newPos = minPos;
                    break;
                case SB_BOTTOM:
                    newPos = maxPos;
                    break;
            }

            if (newPos < minPos) newPos = minPos;
            if (newPos > maxPos) newPos = maxPos;

            if (newPos != currentPos) {
                gProductScrollPos = newPos;
                SetScrollPos(hwnd, SB_VERT, newPos, TRUE);
                
                int boxWidth = 140;
                int boxHeight = 140;
                int xStart = 115;
                int yStart = 70;
                int xGap = 20;
                int yGap = 20;
                int cols = 4;
                for (size_t i = 0; i < gProductNames.size(); ++i) {
                    int col = static_cast<int>(i % cols);
                    int row = static_cast<int>(i / cols);
                    int x = xStart + col * (boxWidth + xGap);
                    int y = yStart + row * (boxHeight + yGap) - gProductScrollPos;
                    HWND button = GetDlgItem(hwnd, ID_PRODUCT_BUTTON_BASE + (int)i);
                    if (button) {
                        SetWindowPos(button, nullptr, x, y, boxWidth, boxHeight, SWP_NOZORDER | SWP_NOACTIVATE);
                    }
                }
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND CreateLandingWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("LandingPage"),
        TEXT("FLOWER SHOP ORDERING SYSTEM"),
        WS_OVERLAPPEDWINDOW,
        gLandingWindowX,
        gLandingWindowY,
        800,
        600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

HWND CreateLoginRegisterWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("Login&RegistrationPage"),
        TEXT("Login and Registration"),
        WS_OVERLAPPEDWINDOW,
        gLoginRegisterWindowX,
        gLoginRegisterWindowY,
        800,
        600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

HWND CreateLogInWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("LoginPage"),
        TEXT("Log In"),
        WS_OVERLAPPEDWINDOW,
        gLoginRegisterWindowX,
        gLoginRegisterWindowY,
        800,
        600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

HWND CreateRegisterWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("RegisterPage"),
        TEXT("Register"),
        WS_OVERLAPPEDWINDOW,
        gLoginRegisterWindowX,
        gLoginRegisterWindowY,
        800,
        600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

HWND CreateMainMenuWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("MainMenuPage"),
        TEXT("Main Menu"),
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        gLoginRegisterWindowX,
        gLoginRegisterWindowY,
        800,
        600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateTitleText(HWND parent, HINSTANCE hInstance){
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

void CreateLogInButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Log In"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        330, 250, 120, 40,
        parent,
        (HMENU)ID_LOGIN_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateRegisterButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0, 
        TEXT("BUTTON"),
        TEXT("Register"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        330, 300, 120, 40, 
        parent,
        (HMENU)ID_REGISTER_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateLogInBackButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Back"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 10, 80, 30,
        parent,
        (HMENU)ID_LOGIN_BACK_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateRegisterBackButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Back"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 10, 80, 30,
        parent,
        (HMENU)ID_REGISTER_BACK_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateLogInUsernameLabel(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("STATIC"),
        TEXT("Username:"),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        200, 150, 100, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateLogInPasswordLabel(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("STATIC"),
        TEXT("Password:"),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        200, 200, 100, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateLogInUsernameInput(HWND parent, HINSTANCE hInstance){
    gLoginUsernameEdit = CreateWindowEx(
        0,
        TEXT("EDIT"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
        300, 150, 200, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateLogInPasswordInput(HWND parent, HINSTANCE hInstance){
    gLoginPasswordEdit = CreateWindowEx(
        0,
        TEXT("EDIT"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_PASSWORD,
        300, 200, 200, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateLogInSubmitButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Submit"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        330, 250, 120, 40,
        parent,
        (HMENU)ID_LOGIN_SUBMIT_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateRegisterUsernameLabel(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("STATIC"),
        TEXT("Username:"),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        200, 150, 100, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateRegisterPasswordLabel(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("STATIC"),
        TEXT("Password:"),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        200, 200, 100, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateRegisterUsernameInput(HWND parent, HINSTANCE hInstance){
    gRegisterUsernameEdit = CreateWindowEx(
        0,
        TEXT("EDIT"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
        300, 150, 200, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateRegisterPasswordInput(HWND parent, HINSTANCE hInstance){
    gRegisterPasswordEdit = CreateWindowEx(
        0,
        TEXT("EDIT"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_PASSWORD,
        300, 200, 200, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateRegisterSubmitButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Submit"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        330, 250, 120, 40,
        parent,
        (HMENU)ID_REGISTER_SUBMIT_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateLogInForm(HWND parent, HINSTANCE hInstance){
    CreateLogInUsernameLabel(parent, hInstance);
    CreateLogInPasswordLabel(parent, hInstance);
    CreateLogInUsernameInput(parent, hInstance);
    CreateLogInPasswordInput(parent, hInstance);
    CreateLogInSubmitButton(parent, hInstance);
}

void CreateRegisterForm(HWND parent, HINSTANCE hInstance){
    CreateRegisterUsernameLabel(parent, hInstance);
    CreateRegisterPasswordLabel(parent, hInstance);
    CreateRegisterUsernameInput(parent, hInstance);
    CreateRegisterPasswordInput(parent, hInstance);
    CreateRegisterSubmitButton(parent, hInstance);
}

void CreateProductGrid(HWND parent, HINSTANCE hInstance){
    LoadProducts();

    int boxWidth = 140;
    int boxHeight = 140;
    int xStart = 115;
    int yStart = 70;
    int xGap = 20;
    int yGap = 20;
    int cols = 4;
    int visibleHeight = 250;

    int rows = (gProductNames.empty() ? 0 : (static_cast<int>(gProductNames.size()) + cols - 1) / cols);
    int totalHeight = rows * (boxHeight + yGap) + 20;
    gProductScrollPos = 0;
    SetScrollRange(parent, SB_VERT, 0, std::max(0, totalHeight - visibleHeight), TRUE);
    SetScrollPos(parent, SB_VERT, 0, TRUE);

    for (size_t i = 0; i < gProductNames.size(); ++i) {
        int col = static_cast<int>(i % cols);
        int row = static_cast<int>(i / cols);

        int x = xStart + col * (boxWidth + xGap);
        int y = yStart + row * (boxHeight + yGap) - gProductScrollPos;

        HWND button = CreateWindowExA(
            0,
            "BUTTON",
            "",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_BITMAP | BS_NOTIFY,
            x, y, boxWidth, boxHeight,
            parent,
            (HMENU)(ID_PRODUCT_BUTTON_BASE + i),
            hInstance,
            nullptr
        );

        if (button) {
            HBITMAP productBitmap = LoadProductBitmap(gProductIds[i], gProductNames[i]);
            SendMessage(button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)productBitmap);
        }
    }
}

void CreateMainMenuExitButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Exit"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 10, 90, 30,
        parent,
        (HMENU)ID_MAIN_MENU_EXIT_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateMainMenuLogOutButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Log Out"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 50, 90, 30,
        parent,
        (HMENU)ID_MAIN_MENU_LOG_OUT_BUTTON,
        hInstance,
        nullptr
    );
}

void MainMenuExit(HWND hwnd){
    PostQuitMessage(0);
}

void CreateMainMenuForm(HWND parent, HINSTANCE hInstance){
    CreateMainMenuExitButton(parent, hInstance);
    CreateMainMenuLogOutButton(parent, hInstance);
    CreateProductGrid(parent, hInstance);
}

void LoadUsers(){
    userMap.clear();
    std::ifstream file("users&passwords.txt");
    if(!file){
        std::ofstream createFile("users&passwords.txt");
        createFile.close();
    }
    while(file){
        std::string line;
        std::getline(file, line);
        if(line.empty()) continue;

        size_t delimiterPos = line.find('|');
        if(delimiterPos != std::string::npos){
            std::string username = line.substr(0, delimiterPos);
            std::string password = line.substr(delimiterPos + 1);
            userMap[username] = password;
        }
    }
}

void LoadProducts(){
    gProductNames.clear();
    gProductIds.clear();
    std::ifstream file("products.txt");
    if (!file) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        size_t delimiterPos = line.find('|');
        std::string productName = line;
        std::string productId = "001";

        if (delimiterPos != std::string::npos) {
            productName = line.substr(0, delimiterPos);
            productId = line.substr(delimiterPos + 1);
        }

        if (!productName.empty()) {
            gProductNames.push_back(productName);
            gProductIds.push_back(productId);
        }
    }
}

std::string ToString(const TCHAR* str){
    std::string result;
    if(!str) return result;
    int length = 0;
    while(str[length] != '\0') ++length;
    result.assign(length, '\0');
    for(int i = 0; i < length; ++i){
        result[i] = static_cast<char>(str[i]);
    }
    return result;
}
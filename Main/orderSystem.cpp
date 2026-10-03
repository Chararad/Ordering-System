//Library Includes
#include <windows.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <gdiplus.h>
#include <ctime>

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
#define ID_MAIN_MENU_CART_BUTTON 110
#define ID_MAIN_MENU_MORE_BUTTON 111
#define ID_MORE_ABOUT_US_BUTTON 112
#define ID_MORE_PROFILE_BUTTON 113
#define ID_MORE_TRANSACTION_HISTORY_BUTTON 114
#define ID_MORE_BACK_BUTTON 115
#define ID_PROFILE_IMAGE 116
#define ID_PROFILE_BACK_BUTTON 117
#define ID_ABOUT_US_BACK_BUTTON 118
#define ID_TRANSACTION_HISTORY_BACK_BUTTON 119
#define ID_PRODUCT_BUTTON_BASE 2000
#define WM_PRODUCT_BITMAP_READY (WM_APP + 1)


//Function Declarations
static std::string GetProductImagePath(const std::string& productId);
static HBITMAP CreateDefaultProductBitmap(const std::string& productName, int width, int height);
static HBITMAP LoadProductBitmap(const std::string& productId, const std::string& productName);
static DWORD WINAPI PreloadProductBitmaps(LPVOID parameter);
LRESULT CALLBACK LandingWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LoginRegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LogInWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainMenuWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CartWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MoreWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ProfileWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutUsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TransactionHistoryWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ProductDetailsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ProductViewportWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateLandingWindow(HINSTANCE hInstance);
HWND CreateLoginRegisterWindow(HINSTANCE hInstance);
HWND CreateLogInWindow(HINSTANCE hInstance);
HWND CreateRegisterWindow(HINSTANCE hInstance);
HWND CreateMainMenuWindow(HINSTANCE hInstance);
HWND CreateCartWindow(HINSTANCE hInstance);
HWND CreateMoreWindow(HINSTANCE hInstance);
HWND CreateProfileWindow(HINSTANCE hInstance, HWND moreWindow);
HWND CreateAboutUsWindow(HINSTANCE hInstance, HWND moreWindow);
HWND CreateTransactionHistoryWindow(HINSTANCE hInstance, HWND moreWindow);
HWND CreateProductDetailsWindow(HINSTANCE hInstance);
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
void CreateMainMenuCartButton(HWND parent, HINSTANCE hInstance);
void CreateMainMenuMoreButton(HWND parent, HINSTANCE hInstance);
void CreateMainMenuGreetingsText(HWND parent, HINSTANCE hInstance);
void CreateMainMenuTitleText(HWND parent, HINSTANCE hInstance);
void CreateMainMenuBalanceText(HWND parent, HINSTANCE hInstance);
void CreateMainMenuForm(HWND parent, HINSTANCE hInstance);
void MainMenuExit(HWND hwnd);
void CreateMoreAboutUsButton(HWND parent, HINSTANCE hInstance);
void CreateMoreProfileButton(HWND parent, HINSTANCE hInstance);
void CreateMoreTransactionHistoryButton(HWND parent, HINSTANCE hInstance);
void CreateMoreBackButton(HWND parent, HINSTANCE hInstance);
void CreateMoreForm(HWND parent, HINSTANCE hInstance);
void CreateProfileForm(HWND parent, HINSTANCE hInstance);
void CreateProfileDefaultUserImage(HWND parent, HINSTANCE hInstance);
void CreateProfileUserNameText(HWND parent, HINSTANCE hInstance);
void CreateProfileBalanceText(HWND parent, HINSTANCE hInstance);
void CreateProfileTransactionsText(HWND parent, HINSTANCE hInstance);
void CreateProfileBackButton(HWND parent, HINSTANCE hInstance);
void CreateAboutUsText(HWND parent, HINSTANCE hInstance);
void CreateAboutUsBackButton(HWND parent, HINSTANCE hInstance);
void CreateAboutUsForm(HWND parent, HINSTANCE hInstance);
void CreateTransactionHistoryText(HWND parent, HINSTANCE hInstance);
void CreateTransactionHistoryBackButton(HWND parent, HINSTANCE hInstance);
void CreateTransactionHistoryForm(HWND parent, HINSTANCE hInstance);
void CreateProfileForm(HWND parent, HINSTANCE hInstance);
void LoadUsers();
void LoadProducts();
void LoadCurrentUserDetails();
void RegisterUser();
void LoginUser();
void LoginUserLogs();
void CreateUserInformationTextFile();
void CreateUserLogsTextFile();
void CreateUserTransactionHistoryTextFile();
void LoadUserTransactionHistory();
std::string ToString(const TCHAR* str);
std::string CreateTimeStamp();

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
static HWND gProductViewport = nullptr;
static int gProductScrollPos = 0;
static ULONG_PTR gGdiplusToken = 0;
static HANDLE gProductLoaderThread = nullptr;
static std::vector<HBITMAP> gProductBitmaps;
static bool gNavigationDestroy = false;

static void DestroyWindowForNavigation(HWND hwnd) {
    gNavigationDestroy = true;
    if (gProductViewport && GetParent(gProductViewport) == hwnd) {
        gProductViewport = nullptr;
    }
    DestroyWindow(hwnd);
    gNavigationDestroy = false;
}

//Structures
struct userInformation{
    std::string curUserPassword;
    long long curUserBalance;
    std:: string curUserTransactions;
};

//Global Variables
std::map<std::string, std::string> userMap;
std::map<std::string, userInformation> userInformationMap;
std::vector<std::string> transactions;
std::vector<std::string> gProductNames;
std::vector<std::string> gProductIds;
std::string currentUser;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR LpCmdLine, int nCmdShow){
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gGdiplusToken, &gdiplusStartupInput, nullptr);

    LoadUsers();
    LoadProducts();
    gProductBitmaps.resize(gProductIds.size(), nullptr);
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

    WNDCLASS pvwc = {};
    pvwc.lpfnWndProc = ProductViewportWindowProc;
    pvwc.hInstance = hInstance;
    pvwc.lpszClassName = TEXT("ProductViewport");
    pvwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&pvwc);

    WNDCLASS cw = {};
    cw.lpfnWndProc = CartWindowProc;
    cw.hInstance = hInstance;
    cw.lpszClassName = TEXT("CartPage");
    cw.hbrBackground = gBackgroundBrush;
    RegisterClass(&cw);

    WNDCLASS mwc = {};
    mwc.lpfnWndProc = MoreWindowProc;
    mwc.hInstance = hInstance;
    mwc.lpszClassName = TEXT("MorePage");
    mwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&mwc);

    WNDCLASS pwc = {};
    pwc.lpfnWndProc = ProfileWindowProc;
    pwc.hInstance = hInstance;
    pwc.lpszClassName = TEXT("ProfilePage");
    pwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&pwc);

    WNDCLASS auwc = {};
    auwc.lpfnWndProc = AboutUsWindowProc;
    auwc.hInstance = hInstance;
    auwc.lpszClassName = TEXT("AboutUsPage");
    auwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&auwc);

    WNDCLASS thwc = {};
    thwc.lpfnWndProc = TransactionHistoryWindowProc;
    thwc.hInstance = hInstance;
    thwc.lpszClassName = TEXT("TransactionHistoryPage");
    thwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&thwc);

    WNDCLASS pdwc = {};
    pdwc.lpfnWndProc = ProductDetailsWindowProc;
    pdwc.hInstance = hInstance;
    pdwc.lpszClassName = TEXT("ProductDetailsPage");
    pdwc.hbrBackground = gBackgroundBrush;
    RegisterClass(&pdwc);

    HWND hwnd = CreateLandingWindow(hInstance);
    CreateTitleText(hwnd, hInstance);
    CreateNextButton(hwnd, hInstance);

    ShowWindow(hwnd, nCmdShow);
    gProductLoaderThread = CreateThread(nullptr, 0, PreloadProductBitmaps, hwnd, 0, nullptr);

    MSG msg = {};
    while(GetMessage(&msg, nullptr, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (gProductLoaderThread) {
        WaitForSingleObject(gProductLoaderThread, INFINITE);
        CloseHandle(gProductLoaderThread);
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
        const int bitmapWidth = 140;
        const int bitmapHeight = 140;
        const double scale = std::min(
            static_cast<double>(bitmapWidth) / image.GetWidth(),
            static_cast<double>(bitmapHeight) / image.GetHeight()
        );
        const int scaledWidth = static_cast<int>(image.GetWidth() * scale);
        const int scaledHeight = static_cast<int>(image.GetHeight() * scale);
        Gdiplus::Bitmap scaledImage(bitmapWidth, bitmapHeight, PixelFormat32bppARGB);
        Gdiplus::Graphics graphics(&scaledImage);
        graphics.Clear(Gdiplus::Color(255, 255, 255, 255));
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics.DrawImage(
            &image,
            (bitmapWidth - scaledWidth) / 2,
            (bitmapHeight - scaledHeight) / 2,
            scaledWidth,
            scaledHeight
        );

        HBITMAP bitmap = nullptr;
        scaledImage.GetHBITMAP(Gdiplus::Color(255, 255, 255), &bitmap);
        if (bitmap) {
            return bitmap;
        }
    }

    return CreateDefaultProductBitmap(productName, 140, 140);
}

static DWORD WINAPI PreloadProductBitmaps(LPVOID parameter) {
    HWND landingWindow = static_cast<HWND>(parameter);
    for (size_t i = 0; i < gProductIds.size(); ++i) {
        HBITMAP bitmap = LoadProductBitmap(gProductIds[i], gProductNames[i]);
        if (!PostMessage(landingWindow, WM_PRODUCT_BITMAP_READY, static_cast<WPARAM>(i), reinterpret_cast<LPARAM>(bitmap))) {
            if (bitmap) {
                DeleteObject(bitmap);
            }
        }
    }
    return 0;
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

        case WM_PRODUCT_BITMAP_READY: {
            size_t productIndex = static_cast<size_t>(wParam);
            HBITMAP bitmap = reinterpret_cast<HBITMAP>(lParam);
            if (productIndex < gProductBitmaps.size()) {
                HBITMAP previousBitmap = gProductBitmaps[productIndex];
                gProductBitmaps[productIndex] = bitmap;
                HWND button = gProductViewport
                    ? GetDlgItem(gProductViewport, ID_PRODUCT_BUTTON_BASE + static_cast<int>(productIndex))
                    : nullptr;
                if (button && bitmap) {
                    SendMessage(button, BM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(bitmap));
                }
                if (previousBitmap && previousBitmap != bitmap) {
                    DeleteObject(previousBitmap);
                }
            } else if (bitmap) {
                DeleteObject(bitmap);
            }
            return 0;
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
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LoginRegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_LOGIN_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                DestroyWindowForNavigation(hwnd);
                HWND loginWindow = CreateLogInWindow(GetModuleHandle(nullptr));
                CreateLogInForm(loginWindow, GetModuleHandle(nullptr));
                CreateLogInBackButton(loginWindow, GetModuleHandle(nullptr));
                ShowWindow(loginWindow, SW_SHOW);
            } else if (LOWORD(wParam) == ID_REGISTER_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                DestroyWindowForNavigation(hwnd);
                HWND registerWindow = CreateRegisterWindow(GetModuleHandle(nullptr));
                CreateRegisterForm(registerWindow, GetModuleHandle(nullptr));
                CreateRegisterBackButton(registerWindow, GetModuleHandle(nullptr));
                ShowWindow(registerWindow, SW_SHOW);
            }
            return 0;

        case WM_DESTROY:
            if (!gNavigationDestroy) PostQuitMessage(0);
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
                DestroyWindowForNavigation(hwnd);
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
                        currentUser = usernameStr;
                        LoadCurrentUserDetails();
                        LoginUser();
                        DestroyWindowForNavigation(hwnd);
                        HWND mainMenuWindow = CreateMainMenuWindow(GetModuleHandle(nullptr));
                        CreateMainMenuForm(mainMenuWindow, GetModuleHandle(nullptr));
                        ShowWindow(mainMenuWindow, SW_SHOW);
                    } else {
                        MessageBox(hwnd, TEXT("Invalid username or password."), TEXT("Login"), MB_OK);
                    }
                }
            }
            return 0;

        case WM_DESTROY:
            if (!gNavigationDestroy) PostQuitMessage(0);
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
                DestroyWindowForNavigation(hwnd);
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
                            currentUser = usernameStr;
                            LoadCurrentUserDetails();
                            RegisterUser();
                            DestroyWindowForNavigation(hwnd);
                            HWND mainMenuWindow = CreateMainMenuWindow(GetModuleHandle(nullptr)); 
                            CreateMainMenuForm(mainMenuWindow, GetModuleHandle(nullptr));
                            ShowWindow(mainMenuWindow, SW_SHOW);
                        } else {
                            MessageBox(hwnd, TEXT("Error saving user data."), TEXT("Register"), MB_OK);
                        }
                    }
                }
            }
            return 0;

        case WM_DESTROY:
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainMenuWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
            if (LOWORD(wParam) == ID_MAIN_MENU_EXIT_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                MainMenuExit(hwnd);
            } else if (LOWORD(wParam) == ID_MAIN_MENU_LOG_OUT_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                DestroyWindowForNavigation(hwnd);
                HWND loginRegisterWindow = CreateLoginRegisterWindow(GetModuleHandle(nullptr));
                CreateLogInButton(loginRegisterWindow, GetModuleHandle(nullptr));
                CreateRegisterButton(loginRegisterWindow, GetModuleHandle(nullptr));
                ShowWindow(loginRegisterWindow, SW_SHOW);
            }
            else if(LOWORD(wParam) == ID_MAIN_MENU_CART_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                DestroyWindowForNavigation(hwnd);
                HWND cartWindow = CreateCartWindow(GetModuleHandle(nullptr));
                ShowWindow(cartWindow, SW_SHOW);
            }
            else if (LOWORD(wParam) == ID_MAIN_MENU_MORE_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                DestroyWindowForNavigation(hwnd);
                HWND moreWindow = CreateMoreWindow(GetModuleHandle(nullptr));
                CreateMoreForm(moreWindow, GetModuleHandle(nullptr));
                ShowWindow(moreWindow, SW_SHOW);
            }
            else if (HIWORD(wParam) == BN_CLICKED) {
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
                    int x = col * (boxWidth + xGap);
                    int y = row * (boxHeight + yGap) - gProductScrollPos;
                    HWND button = GetDlgItem(gProductViewport, ID_PRODUCT_BUTTON_BASE + (int)i);
                    if (button) {
                        SetWindowPos(button, nullptr, x, y, boxWidth, boxHeight, SWP_NOZORDER | SWP_NOACTIVATE);
                    }
                }
            }
            return 0;
        }

        case WM_DESTROY:
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CartWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_DESTROY:
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MoreWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            if(LOWORD(wParam) == ID_MORE_ABOUT_US_BUTTON && HIWORD(wParam) == BN_CLICKED){
                ShowWindow(hwnd, SW_HIDE);
                HWND aboutUsWindow = CreateAboutUsWindow(GetModuleHandle(nullptr), hwnd);
                CreateAboutUsForm(aboutUsWindow, GetModuleHandle(nullptr));
                ShowWindow(aboutUsWindow, SW_SHOW);
            }
            else if(LOWORD(wParam) == ID_MORE_PROFILE_BUTTON && HIWORD(wParam) == BN_CLICKED){
                ShowWindow(hwnd, SW_HIDE);
                HWND profileWindow = CreateProfileWindow(GetModuleHandle(nullptr), hwnd);
                CreateProfileForm(profileWindow, GetModuleHandle(nullptr));
                ShowWindow(profileWindow, SW_SHOW);
            }
            else if(LOWORD(wParam) == ID_MORE_TRANSACTION_HISTORY_BUTTON && HIWORD(wParam) == BN_CLICKED){
                ShowWindow(hwnd, SW_HIDE);
                HWND historyWindow = CreateTransactionHistoryWindow(GetModuleHandle(nullptr), hwnd);
                CreateTransactionHistoryForm(historyWindow, GetModuleHandle(nullptr));
                ShowWindow(historyWindow, SW_SHOW);
            }
            else if(LOWORD(wParam) == ID_MORE_BACK_BUTTON && HIWORD(wParam) == BN_CLICKED){
                DestroyWindowForNavigation(hwnd);
                HWND mainMenuWindow = CreateMainMenuWindow(GetModuleHandle(nullptr));
                CreateMainMenuForm(mainMenuWindow, GetModuleHandle(nullptr));
                ShowWindow(mainMenuWindow, SW_SHOW);
            }
            return 0;
        case WM_DESTROY:
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ProfileWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_PROFILE_BACK_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                HWND moreWindow = reinterpret_cast<HWND>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                DestroyWindowForNavigation(hwnd);
                if (moreWindow) {
                    ShowWindow(moreWindow, SW_SHOW);
                }
                return 0;
            }
            return 0;

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

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY: {
            HWND imageControl = GetDlgItem(hwnd, ID_PROFILE_IMAGE);
            if (imageControl) {
                HBITMAP bitmap = (HBITMAP)SendMessage(imageControl, STM_GETIMAGE, IMAGE_BITMAP, 0);
                if (bitmap) {
                    DeleteObject(bitmap);
                }
            }
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK AboutUsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_ABOUT_US_BACK_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                HWND moreWindow = reinterpret_cast<HWND>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                DestroyWindowForNavigation(hwnd);
                if (moreWindow) {
                    ShowWindow(moreWindow, SW_SHOW);
                }
                return 0;
            }
            return 0;

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

        case WM_CLOSE: {
            HWND moreWindow = reinterpret_cast<HWND>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            DestroyWindowForNavigation(hwnd);
            if (moreWindow) {
                ShowWindow(moreWindow, SW_SHOW);
            }
            return 0;
        }
        case WM_DESTROY: {
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TransactionHistoryWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TRANSACTION_HISTORY_BACK_BUTTON && HIWORD(wParam) == BN_CLICKED) {
                HWND moreWindow = reinterpret_cast<HWND>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                DestroyWindowForNavigation(hwnd);
                if (moreWindow) {
                    ShowWindow(moreWindow, SW_SHOW);
                }
                return 0;
            }
            return 0;

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

        case WM_CLOSE: {
            HWND moreWindow = reinterpret_cast<HWND>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            DestroyWindowForNavigation(hwnd);
            if (moreWindow) {
                ShowWindow(moreWindow, SW_SHOW);
            }
            return 0;
        }

        case WM_DESTROY:
            if (!gNavigationDestroy) PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ProductDetailsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){

    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ProductViewportWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    if (uMsg == WM_COMMAND) {
        return SendMessage(GetParent(hwnd), uMsg, wParam, lParam);
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

HWND CreateCartWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("CartPage"),
        TEXT("Cart"),
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

HWND CreateMoreWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("MorePage"),
        TEXT("More"),
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

HWND CreateProfileWindow(HINSTANCE hInstance, HWND moreWindow){
    HWND profileWindow = CreateWindowEx(
        0,
        TEXT("ProfilePage"),
        TEXT("Profile"),
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
    if (profileWindow) {
        SetWindowLongPtr(profileWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(moreWindow));
    }
    return profileWindow;
}

HWND CreateAboutUsWindow(HINSTANCE hInstance, HWND moreWindow){
    HWND aboutUsWindow = CreateWindowEx(
        0,
        TEXT("AboutUsPage"),
        TEXT("AboutUs"),
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
    if (aboutUsWindow) {
        SetWindowLongPtr(aboutUsWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(moreWindow));
    }
    return aboutUsWindow;
}

HWND CreateTransactionHistoryWindow(HINSTANCE hInstance, HWND moreWindow){
    HWND historyWindow = CreateWindowEx(
        0,
        TEXT("TransactionHistoryPage"),
        TEXT("Transaction History"),
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
    if (historyWindow) {
        SetWindowLongPtr(historyWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(moreWindow));
    }
    return historyWindow;
}

HWND CreateProductDetailsWindow(HINSTANCE hInstance){
    return CreateWindowEx(
        0,
        TEXT("ProductDetailsPage"),
        TEXT("ProductDetails"),
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
    int boxWidth = 140;
    int boxHeight = 140;
    int xStart = 115;
    int yStart = 110;
    int xGap = 20;
    int yGap = 20;
    int cols = 4;

    RECT clientRect;
    GetClientRect(parent, &clientRect);
    int viewportWidth = std::max<LONG>(0, clientRect.right - xStart - 20);
    int visibleHeight = std::max<LONG>(0, clientRect.bottom - yStart - 20);
    gProductViewport = CreateWindowEx(
        0,
        TEXT("ProductViewport"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        xStart, yStart, viewportWidth, visibleHeight,
        parent,
        nullptr,
        hInstance,
        nullptr
    );

    int rows = (gProductNames.empty() ? 0 : (static_cast<int>(gProductNames.size()) + cols - 1) / cols);
    int totalHeight = rows * (boxHeight + yGap) + 20;
    gProductScrollPos = 0;
    SetScrollRange(parent, SB_VERT, 0, std::max(0, totalHeight - visibleHeight), TRUE);
    SetScrollPos(parent, SB_VERT, 0, TRUE);

    for (size_t i = 0; i < gProductNames.size(); ++i) {
        int col = static_cast<int>(i % cols);
        int row = static_cast<int>(i / cols);

        int x = col * (boxWidth + xGap);
        int y = row * (boxHeight + yGap) - gProductScrollPos;

        HWND button = CreateWindowExA(
            0,
            "BUTTON",
            "",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS | BS_PUSHBUTTON | BS_BITMAP | BS_NOTIFY,
            x, y, boxWidth, boxHeight,
            gProductViewport,
            (HMENU)(ID_PRODUCT_BUTTON_BASE + i),
            hInstance,
            nullptr
        );

        if (button) {
            HBITMAP productBitmap = gProductBitmaps[i];
            if (!productBitmap) {
                productBitmap = CreateDefaultProductBitmap(gProductNames[i], boxWidth, boxHeight);
                gProductBitmaps[i] = productBitmap;
            }
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

void CreateMainMenuCartButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Cart"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        630, 10, 90, 30,
        parent,
        (HMENU)ID_MAIN_MENU_CART_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateMainMenuMoreButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("More"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 521, 90, 30,
        parent,
        (HMENU)ID_MAIN_MENU_MORE_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateMainMenuGreetingsText(HWND parent, HINSTANCE hInstance){
    std::string greeting = "Welcome Back, " + currentUser + "!";
    HWND label = CreateWindowExA(
        0,
        "STATIC",
        greeting.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        200, 10, 400, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );

    HFONT font = CreateFont(
        20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI"));

    SendMessage(label, WM_SETFONT, (WPARAM)font, TRUE);
}

void CreateMainMenuTitleText(HWND parent, HINSTANCE hInstance){
    HWND label = CreateWindowEx(
        0,
        TEXT("STATIC"),
        TEXT("FLOWER SHOP CATALOG"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        180, 40, 440, 40,
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

void CreateMainMenuBalanceText(HWND parent, HINSTANCE hInstance){
    std::string balance = "BALANCE: P" + std::to_string(userInformationMap[currentUser].curUserBalance);
    HWND label = CreateWindowExA(
        0,
        "STATIC",
        balance.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        630, 50, 250, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );

    HFONT font = CreateFont(
        19, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI"));

    SendMessage(label, WM_SETFONT, (WPARAM)font, TRUE);
}

void MainMenuExit(HWND hwnd){
    PostQuitMessage(0);
}

void CreateMainMenuForm(HWND parent, HINSTANCE hInstance){
    CreateMainMenuExitButton(parent, hInstance);
    CreateMainMenuLogOutButton(parent, hInstance);
    CreateProductGrid(parent, hInstance);
    CreateMainMenuCartButton(parent, hInstance);
    CreateMainMenuMoreButton(parent, hInstance);
    CreateMainMenuGreetingsText(parent, hInstance);
    CreateMainMenuBalanceText(parent, hInstance);
    CreateMainMenuTitleText(parent, hInstance);
}

void CreateMoreAboutUsButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("About Us"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        330, 100, 90, 30,
        parent,
        (HMENU)ID_MORE_ABOUT_US_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateMoreProfileButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Profile"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        330, 150, 90, 30,
        parent,
        (HMENU)ID_MORE_PROFILE_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateMoreTransactionHistoryButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Transaction History"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        270, 200, 220, 40,
        parent,
        (HMENU)ID_MORE_TRANSACTION_HISTORY_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateMoreBackButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0, 
        TEXT("BUTTON"),
        TEXT("Back"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 10, 90, 30,
        parent,
        (HMENU)ID_MORE_BACK_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateMoreForm(HWND parent, HINSTANCE hInstance){
    CreateMoreAboutUsButton(parent, hInstance);
    CreateMoreProfileButton(parent, hInstance);
    CreateMoreTransactionHistoryButton(parent, hInstance);
    CreateMoreBackButton(parent, hInstance);
}

void CreateAboutUsText(HWND parent, HINSTANCE hInstance){
    std::ifstream file("AboutUs.txt");
    std::string aboutText;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        aboutText += line + "\r\n";
    }

    CreateWindowExA(
        0,
        "STATIC",
        aboutText.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        40, 60, 700, 450,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateAboutUsBackButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Back"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 10, 90, 30,
        parent,
        (HMENU)ID_ABOUT_US_BACK_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateAboutUsForm(HWND parent, HINSTANCE hInstance){
    CreateAboutUsText(parent, hInstance);
    CreateAboutUsBackButton(parent, hInstance);
}

void CreateTransactionHistoryText(HWND parent, HINSTANCE hInstance){
    std::ifstream file("User Transaction History/" + currentUser + ".txt");
    std::string historyText;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        historyText += line + "\r\n";
    }
    if (historyText.empty()) {
        historyText = "No transaction history available.";
    }

    CreateWindowExA(
        0,
        "STATIC",
        historyText.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        40, 60, 700, 450,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateTransactionHistoryForm(HWND parent, HINSTANCE hInstance){
    CreateTransactionHistoryText(parent, hInstance);
    CreateTransactionHistoryBackButton(parent, hInstance);
}

void CreateTransactionHistoryBackButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Back"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 10, 90, 30,
        parent,
        (HMENU)ID_TRANSACTION_HISTORY_BACK_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateProfileDefaultUserImage(HWND parent, HINSTANCE hInstance){
    Gdiplus::Bitmap image(L"user.png");
    if (image.GetLastStatus() != Gdiplus::Ok) {
        return;
    }

    const int imageSize = 128;
    const double scale = std::min(
        static_cast<double>(imageSize) / image.GetWidth(),
        static_cast<double>(imageSize) / image.GetHeight()
    );
    const int scaledWidth = static_cast<int>(image.GetWidth() * scale);
    const int scaledHeight = static_cast<int>(image.GetHeight() * scale);
    Gdiplus::Bitmap scaledImage(imageSize, imageSize, PixelFormat32bppARGB);
    Gdiplus::Graphics graphics(&scaledImage);
    graphics.Clear(Gdiplus::Color(255, 255, 255, 255));
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    graphics.DrawImage(
        &image,
        (imageSize - scaledWidth) / 2,
        (imageSize - scaledHeight) / 2,
        scaledWidth,
        scaledHeight
    );

    HBITMAP bitmap = nullptr;
    if (scaledImage.GetHBITMAP(Gdiplus::Color(255, 255, 255), &bitmap) != Gdiplus::Ok || !bitmap) {
        return;
    }

    HWND imageControl = CreateWindowEx(
        0,
        TEXT("STATIC"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | SS_BITMAP,
        336, 145, imageSize, imageSize,
        parent,
        (HMENU)ID_PROFILE_IMAGE,
        hInstance,
        nullptr
    );
    if (!imageControl) {
        DeleteObject(bitmap);
        return;
    }
    SendMessage(imageControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bitmap);
}

void CreateProfileUserNameText(HWND parent, HINSTANCE hInstance){
    std::string username = "Username: " + currentUser;
    HWND label = CreateWindowExA(
        0,
        "STATIC",
        username.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        300, 300, 200, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateProfilePasswordText(HWND parent, HINSTANCE hInstance){
    std::string password = "Password: " + userInformationMap[currentUser].curUserPassword;
    HWND label = CreateWindowExA(
        0,
        "STATIC",
        password.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        300, 340, 200, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateProfileBalanceText(HWND parent, HINSTANCE hInstance){
    std::string balance = "Balance: P" + std::to_string(userInformationMap[currentUser].curUserBalance);
    HWND label = CreateWindowExA(
        0,
        "STATIC",
        balance.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        300, 380, 200, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateProfileTransactionsText(HWND parent, HINSTANCE hInstance){
    std::string transactions = "Transactions: " + userInformationMap[currentUser].curUserTransactions;
    HWND label = CreateWindowExA(
        0,
        "STATIC",
        transactions.c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        300, 420, 400, 30,
        parent,
        nullptr,
        hInstance,
        nullptr
    );
}

void CreateProfileBackButton(HWND parent, HINSTANCE hInstance){
    CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("Back"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 10, 90, 30,
        parent,
        (HMENU)ID_PROFILE_BACK_BUTTON,
        hInstance,
        nullptr
    );
}

void CreateProfileForm(HWND parent, HINSTANCE hInstance){
    CreateProfileDefaultUserImage(parent, hInstance);
    CreateProfileUserNameText(parent, hInstance);
    CreateProfilePasswordText(parent, hInstance);
    CreateProfileBalanceText(parent, hInstance);
    CreateProfileTransactionsText(parent, hInstance);
    CreateProfileBackButton(parent, hInstance);
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

void LoadCurrentUserDetails(){
    if (currentUser.empty()) {
        return;
    }

    std::ifstream file("User Informations/" + currentUser + ".txt");
    if (!file) {
        return;
    }

    userInformation details{};
    std::string line;
    while (std::getline(file, line)) {
        const size_t delimiter = line.find(':');
        if (delimiter == std::string::npos) {
            continue;
        }

        const std::string field = line.substr(0, delimiter);
        std::string value = line.substr(delimiter + 1);
        const size_t valueStart = value.find_first_not_of(" \t\r");
        if (valueStart == std::string::npos) {
            value.clear();
        } else {
            value.erase(0, valueStart);
        }

        if (field == "Password") {
            details.curUserPassword = value;
        } else if (field == "Balance") {
            try {
                details.curUserBalance = std::stoll(value);
            } catch (...) {
                details.curUserBalance = 0;
            }
        } else if (field == "Transactions") {
            details.curUserTransactions = value;
        }
    }

    userInformationMap[currentUser] = details;
}

void RegisterUser(){
    CreateUserInformationTextFile();
    CreateUserLogsTextFile();
    CreateUserTransactionHistoryTextFile();
}

void LoginUser(){
    LoginUserLogs();
    LoadUserTransactionHistory();
}

void LoginUserLogs(){
    const std::string filePath = "User Logs/" + currentUser + ".txt";
    std::ofstream file(filePath, std::ios::app);
    if (!file) {
        return;
    }
    file << CreateTimeStamp() + " - Account Logged In"<< '\n';
}

void CreateUserInformationTextFile(){
    auto user = userMap.find(currentUser);
    if (user == userMap.end()) {
        return;
    }

    const std::string filePath = "User Informations/" + currentUser + ".txt";
    std::ofstream file(filePath);
    if (!file) {
        return;
    }

    file << "Username: " << currentUser << '\n'
         << "Password: " << user->second << '\n'
         << "Balance: 0\n"
         << "Transations: 0\n";
}

void CreateUserLogsTextFile(){
    auto user = userMap.find(currentUser);
    if(user == userMap.end()){
        return;
    }

    const std::string filePath = "User Logs/" + currentUser + ".txt";
    std::ofstream file(filePath);
    if (!file) {
        return;
    }
    file << CreateTimeStamp() + " - Account Created"<< '\n';
}

void CreateUserTransactionHistoryTextFile(){
    auto user = userMap.find(currentUser);
    if(user == userMap.end()){
        return;
    }
    const std::string filePath = "User Transaction History/" + currentUser + ".txt";
    std::ofstream file(filePath);
    if (!file) {
        return;
    }
    
}

std::string line;
void LoadUserTransactionHistory(){
    transactions.clear();
    auto user = userMap.find(currentUser);
    if(user == userMap.end()) return;

    const std::string filePath = "User Transaction History/" + currentUser + ".txt";
    std::ifstream file(filePath);

    while(std::getline(file, line)){
        transactions.push_back(line);
    }
}

std::string CreateTimeStamp(){
    const std::time_t now = std::time(nullptr);
    const char* timeText = std::ctime(&now);
    if (!timeText) {
        return "";
    }

    std::string timestamp(timeText);
    if (!timestamp.empty() && timestamp.back() == '\n') {
        timestamp.pop_back();
    }
    return timestamp;
}
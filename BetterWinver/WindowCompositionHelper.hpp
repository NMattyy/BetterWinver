//WindowCompositionHelper 2.1.2

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3, 
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4, 
    ACCENT_INVALID_STATE = 5
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

typedef void (WINAPI* PfnSetPreferredAppMode)(int appMode);
typedef void (WINAPI* PfnFlushMenuThemes)(void);
//BetterWinver 1.8.1

inline void windowTheme(HWND hwnd) {
    BOOL dark = isDarkModeEnabled;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    if (compCheck >= 22000) {
        int backdropType = DWMSBT_MAINWINDOW; 
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

        MARGINS margins = { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT dpi = GetDpiForWindow(hwnd);

    float margin = ScaleValueF(30.0f, dpi);
    int btnW = ScaleValue(75, dpi);
    int btnH = ScaleValue(24, dpi);

    btn.rect.right = (long)(rc.right - margin);
    btn.rect.left = (long)(btn.rect.right - btnW);
    btn.rect.bottom = (long)(rc.bottom - ScaleValue(20, dpi));
    btn.rect.top = (long)(btn.rect.bottom - btnH);

    if (!pTextFormatBody) {
        CreateTextFormats(hwnd);
    }
}

inline void clearBackground(ID2D1HwndRenderTarget* pWindowRenderTarget) {
    pWindowRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    if (compCheck >= 22000) {
        pWindowRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0.0f));
        pWindowRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    } else {
        pWindowRenderTarget->Clear(isDarkModeEnabled ? D2D1::ColorF(0.12f, 0.12f, 0.12f) : D2D1::ColorF(D2D1::ColorF::White));
        pWindowRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
    }
}

inline void UpdateButtonState(HWND hwnd, LPARAM lp, bool isDown) {
    POINT pt = { LOWORD(lp), HIWORD(lp) };
    bool wasHover = btn.hover;
    btn.hover = PtInRect(&btn.rect, pt);

    if (wasHover != btn.hover || isDown) {
        InvalidateRect(hwnd, &btn.rect, FALSE);
    }
}
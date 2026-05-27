# BetterWinver
> [!Warning]
> This application has been made only as a training and just for educational purposes.

| BetterWinver | Logo |
| :--- | :---: |
| BetterWinver is an application written in c++ for Windows 10 1809 (Build 17763) x64 & ARM64 and newer (Designed for Windows 11) that aims to recreate "About Windows" (Winver) of Microsoft Windows by upgrading its graphics, by adding dark theme support or Acrylic (Build <22621) and Mica (Build 22621+) background effects, and its functionality, by using more modern API like Direct2D v3 as render engine. | <img src="https://raw.githubusercontent.com/NMattyy/BetterWinver/refs/heads/main/Preview/BetterWinver%20logo.png" width="400"> |

## Usage
Since version 1.8.2, BetterWinver comes with an installer or a portable .exe file. Either way, to use the application, you have to install the certificate that comes with every release of the app (just the first time)

**As this is a small project I couldn't buy a signed certificate, so I had to create It by myself. But this implies that It doesn't work in any computer except the ones where the certificate is installed, so if you want to install the app or if you have Smart App Control - SAC enabled to use the portable file, you have to install the certificate**

### Certificate installation guide
- Download either the portable executable or the installer bundle, then right click it and go to "Properties"
- Then go to "Digital signature" and click the certificate, then click "details"
- Now click "View certificate", then press "Install certificate" 
- Now another window will open, now in "Store Location" you have to select "Local machine", then select "Place all certificates in the following store"
- Now browse the certificate store until you find "Trusted people", select It and proceed
- Now you can click "Next" to install the certificate
- After that you should be able to use the installer or use the portable version even if you have Smart App Control enabled

### Boot arguments
If you want, you can customize BetterWinver by forcing some settings like language, theme or username.
Boot arguments list:
- **-forcedarkmode** (to force dark mode)
- **-forcelightmode** (to force light mode)
- **-disableacrylic** (to disable acrylic)
- **-customusername "*username*"** (to change the username that will be shown)
- **-lang *language*** (to change BetterWinver's language [*en* for English, *it* for Italian, *fr* for French, *es* for Spanish, *de* for German and *pt* for Portuguese)

## ℹ️ Credits
- [@Microsoft](https://github.com/microsoft) for Microsoft Windows

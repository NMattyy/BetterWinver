# BetterWinver
> [!Warning]
> This application has been made only for educational purposes using mainly Gemini AI assistant to let me learn C++ better and Windows's API.

| BetterWinver | Preview |
| :--- | :---: |
| BetterWinver is an application written in c++ for Windows 10 x64 & ARM64 and newer that aims to recreate "About Windows" (Winver) of Microsoft Windows by upgrading its graphics, like by adding dark theme supports (Win 10+) or Mica background effect (Win 11), and its functionality by using more modern API like d2d as render engine. | <img src="https://github.com/NMattyy/BetterWinver/blob/main/Preview/Windows 11.png?raw=true" width="400"> |

## Usage
Since version 1.8.2, BetterWinver comes with an installer or a portable .exe file. Either way, to use the application, you have to install the certificate that comes with every release of the app (just the first time)

**As this is a small project I couldn't buy a signed certificate, so I had to create It by myself. But this implies that It doesn't work in any computer except the ones where the certificate is installed, so if you want to install the app or if you have Smart App Control - SAC enabled to use the portable file, you have to install the certificate**

### Certificate instllation guide
- Download either the portable executable or the installer bundle, then right click it and go to "Properties"
- Then go to "Digital signature" and click the certificate, then click "details"
- Now click "View certificate", if the certificate is released by "NMattyy" and expires on 04/18/2031 you can proceed installing It by pressing "Install certificate" 
- Now another window will open, now in "Store Location" you have to select "Local machine", then select "Place all certificates in the following store"
- Now browse the certificate store until you find "Trusted people", select It and proceed
- Now you can click "Next" to install the certificate
- After that you should be able to use the installer and use the portable version even if you have Smart App Control enabled

## ℹ️ Credits
- [@Microsoft](https://github.com/microsoft) for Microsoft Windows

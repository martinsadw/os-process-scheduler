@echo off
gcc -O2 -Wall main.c process.c rrprio.c sched.c -o out.exe -s
REM gcc -O2 main.c process.c rrprio.c sched.c -o out.exe

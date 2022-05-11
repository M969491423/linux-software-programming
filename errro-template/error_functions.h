#include "tlpi_hdr.h"

#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

void errMsg(const char *format, ...);

#ifdef __GUNC__

#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

//以下三个诊断调用系统调用和库函数时所发生的错误
void errExit(const char *format, ...) NORETURN; //函数 errMsg()会在标准错误设备上打印消息

void err_exit(const char *format, ...) NORETURN; //

void errExitEN(int errnum, const char *format, ...) NORETURN;
//以下三个函数诊断其他类型的错误
void fatal(const char *format, ...) NORETURN;

void usageErr(const char *format) NORETURN;

void cmdLineErr(const char *format) NORETURN;

#endif
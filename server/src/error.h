#ifndef ERROR_H
#define ERROR_H

#define	MAXLINE	4096 // Max text line length

void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);

#endif


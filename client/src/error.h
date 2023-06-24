#ifndef ERROR_H
#define ERROR_H

#define	MAXLINE	4096 // Max text line length

void err_dump(char const*, ...);
void err_msg(char const*, ...);
void err_quit(char const*, ...);
void err_ret(char const*, ...);
void err_sys(char const*, ...);

#endif


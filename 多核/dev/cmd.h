#ifndef _CMD_H_
#define _CMD_H_
void cmdsend(char*, char*, char(*)[], char*, char[]);
void cal(int, char*, char*, char(*)[], char*, char[]);
void logic(int, char*, char*, char(*)[], char*, char[]);
int cmdcmp(char*, char*, char(*)[], char*, char[]);
void cmdgoto(int, char*, char*, int*);
void io(int, char*, char(*)[], char[]);
#endif

#pragma once

extern void elogOpen(char const*);
extern void elogClose(void);
extern void elogMessage(char const*, int);

// 2012
extern void elogMessage(int, char const*, ...); 

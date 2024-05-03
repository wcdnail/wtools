#if !defined(__WCD_PRINTING_ROOT)
#define __WCD_PRINTING_ROOT

extern unsigned char *defFont;

extern bool loadFont8x16(char *, unsigned char *);
extern bool loadFont8x16(char *);

extern void drawString(int, int, char *, unsigned, LPDIRECTDRAWSURFACE);
extern void drawString(int, int, char *, unsigned);

#endif
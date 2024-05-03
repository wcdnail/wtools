#if !defined(__WCD_PREDEF)
#define __WCD_PREDEF

#define PI 3.1415926535897932384626433832785

typedef struct {
	double X, Y, Z;
	int x, y;
	double u, v;
	double uz, vz, z1;
} vertex;

typedef struct {
	vertex *v1, *v2, *v3;
	unsigned short *tex;
} face;

typedef void (spixfunc)(int, int, unsigned);
typedef void (hlinefunc)(int, int, int, unsigned);

#endif
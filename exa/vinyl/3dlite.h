#if !defined(__WCD_3DLITE)
#define __WCD_3DLITE

#include <math.h>
#include "gfx.h"
#include "structs.h"

class _3dlite {
protected:
	double perspc;

	int vn, pn;
	vertex *v;
	face *p;

public:
	_3dlite(): v(NULL), vn(0), p(NULL), pn(0), perspc(1024) {}

	~_3dlite() { 
		if (v!=NULL) delete v; 
		if (p!=NULL) delete p;
	}

	_3dlite(int cnt): perspc(1024) {
		vn = cnt;
		v = new vertex[vn];
	}

	_3dlite(int cnt, int pnt): perspc(1024) {
		vn = cnt;
		v = new vertex[vn];
		pn = pnt;
		p = new face[pn];
	}

	void set(int i, vertex X) { v[i] = X; }

	void set(int i, double X, double Y, double Z) {
		v[i].X = X;
		v[i].Y = Y;
		v[i].Z = Z;
	}

	virtual void rotate(double, double, double);
	virtual void move(double, double, double);

	virtual void project(vertex *A) {
		A->x = (int) ( scrx/2 + A->X*perspc / (A->Z+perspc) );
		A->y = (int) ( maxy/2 - A->Y*perspc / (A->Z+perspc) );
	}

	virtual void projection(int, int);
	virtual void projection() { projection(scrx>>1, maxy>>1); }
	
};

class Axises: public _3dlite {
public:
	Axises(): _3dlite(4) { 	
		set(0, 0, 0, 0);
		set(1, 100, 0, 0);
		set(2, 0, 100, 0);
		set(3, 0, 0, 100);
	}

	Axises(int len): _3dlite(4) { 
		set(0, 0, 0, 0);
		set(1, len, 0, 0);
		set(2, 0, len, 0);
		set(3, 0, 0, len);
	}

	void show(unsigned xc, unsigned yc, unsigned zc) {
		bres_line(v[0].x, v[0].y, v[1].x, v[1].y, xc);
		bres_line(v[0].x, v[0].y, v[2].x, v[2].y, yc);
		bres_line(v[0].x, v[0].y, v[3].x, v[3].y, zc);
	}
};

#endif

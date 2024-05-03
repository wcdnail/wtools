#include "StdAfx.h"
#include "3dlite.h"


void _3dlite::rotate(double vx, double vy, double vz) {
	if (v>0) {
		double x, y, z;

		for (int i=0; i<vn; i++) {
			// вращаю по X
			y = cos(vx) * v[i].Y - sin(vx) * v[i].Z;
            z = sin(vx) * v[i].Y + cos(vx) * v[i].Z;
			v[i].Y = y;
			v[i].Z = z;
			// вращаю по Y
            x =  cos(vy) * v[i].X + sin(vy) * v[i].Z;
            z = -sin(vy) * v[i].X + cos(vy) * v[i].Z;
			v[i].X = x;
			v[i].Z = z;
			// вращаю по Z
			x = cos(vz) * v[i].X - sin(vz) * v[i].Y;
			y = sin(vz) * v[i].X + cos(vz) * v[i].Y;
			v[i].X = x;
			v[i].Y = y;
		}
	}
}

void _3dlite::move(double nx, double ny, double nz) {
	if (v>0) {
		for (int i=0; i<vn; i++) {
			v[i].X += nx;
			v[i].Y += ny;
			v[i].Z += nz;
		}
	}
}

void _3dlite::projection(int sx, int sy) {
	if (v>0) {
		for (int i=0; i<vn; i++) {
			v[i].x = (int) ( sx + v[i].X*perspc / (v[i].Z+perspc) );
			v[i].y = (int) ( sy - v[i].Y*perspc / (v[i].Z+perspc) );
		}
	}
}

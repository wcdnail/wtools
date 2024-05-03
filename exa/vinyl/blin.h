#if !defined(__WCD_BLIN_VINIL)
#define __WCD_BLIN_VINIL

#include <stdio.h>
#include "gfx.h"
#include "3dlite.h"

template <typename T>
inline T safe_div(T a, T b)
{
    return a / (b == 0 ? 1 : b);
}

class Blin: public _3dlite {
	double radius;
    unsigned short tex[256][256];

public:
	Blin(): _3dlite(33, 8), radius(160) {
		double a = 0, st = PI*2/vn;

		set(0, 0, 0, 0);
		for (int i=1; i<vn; i++) {
			v[i].X = radius * cos(a);
			v[i].Y = radius * sin(a);
			v[i].Z = 0;
			a += st;
		}

		int j=1;
		for (int i=0; i<pn; i++) {
			p[i].v1 = &v[0];
			p[i].v2 = &v[j];
			p[i].v3 = &v[j+1];
			p[i].tex = (unsigned short *)tex;
			j++;
		}

		for (int i=0; i<pn; i++) {
			p[i].v1->u = 0;
			p[i].v1->v = 0;
			p[i].v2->u = 0;
			p[i].v2->v = 255;
			p[i].v3->u = 255;
			p[i].v3->v = 0;
		}
	}

	Blin & operator<<(char const* n) {
		FILE *f = NULL;
        fopen_s(&f, n, "r");
		if (f!=NULL) {
            //fseek(f, sizeof(BITMAPINFOHEADER), SEEK_SET);
			fread(&tex, sizeof(tex), 1, f);
			fclose(f);
		}
		return (*this);
	}

	void test_tex16(int x, int y) {
		unsigned short *dst = (unsigned short *)ddsd.lpSurface;
        int a = maxx>>1, b = x+y*a;
		for (int j=0; j<256; j++) {
			for (int i=0; i<256; i++) dst[b+i] = tex[j][i];
			b+=a;
		}
	}

	void showedges() {
		if (vn>0) {
			projection();

            int i = 0;
			for (i=1; i<vn; i++) 
				bres_line(v[i-1].x, v[i-1].y, v[i].x, v[i].y, 0xffff);

			bres_line(v[i-1].x, v[i-1].y, v[0].x, v[0].y, 0xffff);
		}
	}

	void affine_poly(face *f) {
		vertex *a, *b, *c, *tmp_vertex;
		int current_sx, current_sy;
		double tmp, k, x_start, x_end, u_start, u_end, v_start, v_end;
		double u, v, du, dv;

		int dest = 0;
		unsigned short *dst = (unsigned short *)ddsd.lpSurface;

		a = f->v1;
		b = f->v2;
		c = f->v3;

		project(a);
		project(b);
		project(c);

		if (a->y > b->y) { tmp_vertex = a; a = b; b = tmp_vertex; }
		if (a->y > c->y) { tmp_vertex = a; a = c; c = tmp_vertex; }
		if (b->y > c->y) { tmp_vertex = b; b = c; c = tmp_vertex; }

		if ((int)c->y == (int)a->y) return;

		k = safe_div((b->y - a->y), (c->y - a->y));
		x_start = a->x + (c->x - a->x) * k;
		u_start = a->u + (c->u - a->u) * k;
		v_start = a->v + (c->v - a->v) * k;
		x_end = b->x;
		u_end = b->u;
		v_end = b->v;
		du = safe_div((u_start - u_end), (x_start - x_end));
		dv = safe_div((v_start - v_end), (x_start - x_end));

		
		for (current_sy = (int)a->y; current_sy <= (int)c->y; current_sy++) {
    
			k = safe_div((current_sy - a->y), (c->y - a->y));
			x_start = a->x + (c->x - a->x) * k;
			u_start = a->u + (c->u - a->u) * k;
			v_start = a->v + (c->v - a->v) * k;

			if (current_sy >= b->y) {
 				k = safe_div((current_sy - b->y), (c->y - b->y));
				x_end = b->x + (c->x - b->x) * k;
				u_end = b->u + (c->u - b->u) * k;
				v_end = b->v + (c->v - b->v) * k;
			} else {
				k = safe_div((current_sy - a->y), (b->y - a->y));
				x_end = a->x + (b->x - a->x) * k;
				u_end = a->u + (b->u - a->u) * k;
				v_end = a->v + (b->v - a->v) * k;
			}

			if (x_start > x_end) {
				tmp = x_start; x_start = x_end; x_end = tmp;
				tmp = u_start; u_start = u_end; u_end = tmp;
				tmp = v_start; v_start = v_end; v_end = tmp;
			}

			dest = current_sy * (maxx>>1) + (int)x_start;
    
			u = u_start;
			v = v_start;

			for (current_sx = (int)x_start; current_sx <= (int)x_end; current_sx++) {
   				dst[dest++] = f->tex[((int)v) * 256 + (int)u];
				u += du;
				v += dv;
			}
		}
	}

	void show() {
		affine_poly(&p[0]);
	}
};

#endif
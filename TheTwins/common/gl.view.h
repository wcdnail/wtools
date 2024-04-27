#ifndef _CF_gl_view_h__
#define _CF_gl_view_h__

#include <gl/gl.h>

namespace Cf
{
    inline void Setup3DView(int cx, int cy, double fovy = 60.0, double zNear = 1.0, double zFar = 200.0)
    {
        ::glViewport(0, 0, cx, cy);

        ::glMatrixMode(GL_PROJECTION);
        ::glLoadIdentity();
        float aspectRatio = (float)cx / (float)(cy == 0 ? 1 : cy);
        ::gluPerspective(fovy, aspectRatio, zNear, zFar);

        ::glMatrixMode(GL_MODELVIEW);
        ::glLoadIdentity();
    }

#if 0
    inline void Setup3DView2(unsigned short cx, unsigned short cy)
    {
        static const double range = 100;

        double modelMatrix[16];
        double projMatrix[16];

        int viewport[4];

        if(cy == 0) 
            cy = 1;

        ::glViewport(0, 0, cx, cy);
        ::glMatrixMode(GL_PROJECTION);
        ::glLoadIdentity();

        if (cx <= cy)
            ::glOrtho(-range, range, -range*cy/cx, range*cy/cx, -range*10000, range*10000);
        else
            ::glOrtho(-range*cx/cy, range*cx/cy, -range, range, -range*10000, range*10000);

        ::glMatrixMode(GL_MODELVIEW);
        ::glLoadIdentity();

        ::glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
        ::glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
        ::glGetIntegerv(GL_VIEWPORT, viewport);

        ::gluUnProject(0, 0, 1, modelMatrix, projMatrix, viewport, &win_xmin, &win_ymin, &winz);
        ::gluUnProject (cx, cy, 1, modelMatrix, projMatrix, viewport, &win_xmax, &win_ymax, &winz);
    }
#endif
}

#endif // _CF_gl_view_h__

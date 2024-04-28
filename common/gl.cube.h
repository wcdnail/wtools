#ifndef _CF_gl_cube_h__
#define _CF_gl_cube_h__

#include <gl/gl.h>

namespace Cf
{
    inline void RenderCube()
    {
        static const double cubeVert[][4] =
        {
            { -10.0, -10.0,  10.0, 1.0 },
            {  10.0, -10.0,  10.0, 1.0 },
            {  10.0,  10.0,  10.0, 1.0 },
            { -10.0,  10.0,  10.0, 1.0 },
            { -10.0, -10.0, -10.0, 1.0 },
            {  10.0, -10.0, -10.0, 1.0 },
            {  10.0,  10.0, -10.0, 1.0 },
            { -10.0,  10.0, -10.0, 1.0 },
        };

        static const unsigned cubeQuad[6][4] = 
        {
            { 0, 1, 2, 3 },
            { 1, 5, 6, 2 },
            { 3, 2, 6, 7 },
            { 4, 0, 3, 7 },
            { 5, 4, 7, 6 },
            { 4, 5, 1, 0 },
        };

        static const double cubeNorm[6][3] = 
        {
            {  0.0,  0.0,  1.0 },
            {  1.0,  0.0,  0.0 },
            {  0.0,  1.0,  0.0 },
            { -1.0,  0.0,  0.0 },
            {  0.0,  0.0, -1.0 },
            {  0.0, -1.0,  0.0 },
        };

        ::glBegin(GL_QUADS);

        for (int i=0; i<6; i++)
        {
            ::glNormal3dv(cubeNorm[i]);

            for (int j=0; j<4; j++)
                ::glVertex4dv(cubeVert[cubeQuad[i][j]]);
        }

        ::glEnd();
        ::glFlush();
    }
}

#endif // _CF_gl_cube_h__


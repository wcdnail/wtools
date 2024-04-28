#ifndef _CF_gl_torus_h__
#define _CF_gl_torus_h__

#include <cmath>
#include <gl/gl.h>

namespace Cf
{
    inline void RenderTorus(int maj, int min, float majRadius, float minRadius, bool shaded = true)
    {
        ::glPushAttrib(GL_ENABLE_BIT);

        static const double PI = 3.1415926535897932384626433832795;
        double majStep = 2.0 * PI / (maj == 0 ? 1 : maj);
        double minStep = 2.0 * PI / (min == 0 ? 1 : min);

        float opacity = 255;
        float ambiMat[4] = { 0.2f, 0.5f, 0.7f, opacity };
        float diffMat[4] = { 0.7f, 0.7f, 0.7f, opacity };
        float specMat[4] = { 1.0f, 1.0f, 1.0f, opacity };
        float specRef[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        int shine = 128;
        opacity = 255;

        ::glEnable(GL_COLOR_MATERIAL);
        ::glEnable(GL_AUTO_NORMAL);
        ::glEnable(GL_NORMALIZE);
        ::glDepthFunc(GL_LESS);

        ::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambiMat);
        ::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat);
        ::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat);

        ::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specRef);
        ::glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, shine);

        if (!shaded)
            ::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (int i = 0; i < maj; ++i) 
        {
            double a0 = i * majStep;
            double a1 = a0 + majStep;
            GLdouble x0 = cos(a0);
            GLdouble y0 = sin(a0);
            GLdouble x1 = cos(a1);
            GLdouble y1 = sin(a1);

            ::glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= min; ++j) 
            {
                double b = j * minStep;
                GLdouble c = cos(b);
                GLdouble r = minRadius * c + majRadius;
                GLdouble z = minRadius * sin(b);

                ::glNormal3d(x0 * c, y0 * c, z / minRadius);
                ::glTexCoord2d(i / (GLdouble) maj, j / (GLdouble) min);
                ::glVertex3d(x0 * r, y0 * r, z);

                ::glNormal3d(x1 * c, y1 * c, z / minRadius);
                ::glTexCoord2d((i + 1) / (GLdouble) maj, j / (GLdouble) min);
                ::glVertex3d(x1 * r, y1 * r, z);
            }
            ::glEnd();
        }

        ::glPopAttrib();
    }
}

#endif // _CF_gl_torus_h__


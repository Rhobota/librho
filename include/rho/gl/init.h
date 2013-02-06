#ifndef __rho_gl_init_h__
#define __rho_gl_init_h__


namespace rho
{
namespace gl
{


///////////////////////////////////////////////////////////////////////////////
// 2d stuff
///////////////////////////////////////////////////////////////////////////////

void init2d(int width, int height);   // also sets up projection

///////////////////////////////////////////////////////////////////////////////
// 3d stuff
///////////////////////////////////////////////////////////////////////////////

void init3d(bool cullFront, bool cullBack);  // does not set up projection

void orthogonalProjection(int windowWidth, int windowHeight,
                          double width, double depth);

void perspectiveProjection(int windowWidth, int windowHeight,
                           double fovy, double near, double far);


}    // namespace gl
}    // namespace rho


#endif    // __rho_gl_init_h__

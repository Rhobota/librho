#include <rho/gl/tLight.h>
#include <rho/gl/tArtist.h>
#include <rho/gl/init.h>
#include <rho/geo/tMesh.h>
#include <rho/geo/tBox.h>
#include <rho/geo/tVector.h>
#include <rho/tCrashReporter.h>

#include <GL/glfw.h>

#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;


// Window constants
const bool kFullscreen = true;
const int  kAlphaBits = 8;
const int  kDepthBits = 32;    // usually gets coerced to 24
const int  kStencilBits = 8;

// OpenGL constants
const double kFOVY = 60.0;     // field of view -- in degrees in the y-direction
const double kNear = 1.0;      // the near clipping plane distance
const double kFar = 100.0;     // the far clipping plane distance
const bool   kCullBack = false;

// App constants
const double kStepSize = 0.1;
const double kPixelsPerRadian = 300.0;
const double kCameraPhiEpsilon = 0.0001;
const string kObjFilePath = "LegoMan.obj";


void GLFWCALL windowsResized(int width, int height)
{
    glViewport(0, 0, width, height);
    rho::gl::perspectiveProjection(width, height, kFOVY, kNear, kFar);
}

void GLFWCALL keyCallback(int key, int state)
{
    if (state == GLFW_PRESS)
    {
    }
    else if (state == GLFW_RELEASE)
    {
    }
}

int main()
{
    rho::tCrashReporter::init();

    // Print instructions to the user.
    cout << "Use RIGHT/LEFT/UP/DOWN/n/m to control the light." << endl;
    cout << "Use w/a/s/d/space/lshift and the mouse to control the camera." << endl;
    cout << endl;

    // Initialize GLFW.
    if (! glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    // Get the current desktop video mode.
    GLFWvidmode currMode;
    glfwGetDesktopMode(&currMode);
    int width = kFullscreen ? currMode.Width : currMode.Width/2;
    int height = kFullscreen ? currMode.Height : currMode.Height/2;

    // Open an OpenGL window. (GLFW_WINDOW or GLFW_FULLSCREEN)
    if (! glfwOpenWindow(width, height,
                currMode.RedBits, currMode.GreenBits, currMode.BlueBits,
                kAlphaBits, kDepthBits, kStencilBits,
                kFullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW))
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set the window attributes.
    glfwSetWindowTitle("OBJ File Reader");
    glfwSetWindowSizeCallback(windowsResized);
    glfwSetKeyCallback(keyCallback);
    glfwSwapInterval(1);                           // vsync on

    // Init the OpenGL state that we need.
    rho::gl::init3d(false, kCullBack);

    // Light and camera state.
    rho::geo::tVector cameraPos(rho::geo::tVector::point(10.0, 0.0, 0.0));
    rho::geo::tVector cameraDir(rho::geo::tVector(-1.0, 0.0, 0.0));
    int mouseX, mouseY;
    glfwGetMousePos(&mouseX, &mouseY);

    // Drawables.
    rho::refc<rho::iArtist> artist(new rho::gl::tArtist(rho::gl::kFilled));
    rho::geo::tMesh mesh(kObjFilePath);
    mesh.setArtist(artist);
    rho::geo::tBox lightBox(
            rho::geo::tVector::point(-0.3, -0.3, -0.3),
            rho::geo::tVector::point(0.3, 0.3, 0.3));
    lightBox.setArtist(artist);

    // The single light source.
    rho::geo::tVector lightPos(rho::geo::tVector::origin());
    rho::gl::tLight light;
    light.enable();

    // Main loop.
    int running = GL_TRUE;
    while (running)
    {
        // Clear the old framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Setup the camera.
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
                  cameraPos.x+cameraDir.x,
                  cameraPos.y+cameraDir.y,
                  cameraPos.z+cameraDir.z,
                  0, 0, 1);

        // Set the light's position.
        light.setLocation(lightPos);

        // Draw a cube at the light's position.
        glPushMatrix();
        glTranslated(lightPos.x, lightPos.y, lightPos.z);
        glColor3d(0.0, 1.0, 0.0);
        lightBox.draw();
        glPopMatrix();

        // Draw the obj in the center.
        glPushMatrix();
        glTranslated(0, 0, 0);
        glColor3d(1.0, 0.0, 0.0);
        mesh.draw();
        glPopMatrix();

        // Swap front and back rendering buffers and poll for events.
        glfwSwapBuffers();

        // Check if ESC key was pressed or window was closed.
        running = !glfwGetKey(GLFW_KEY_ESC) &&
            glfwGetWindowParam(GLFW_OPENED);

        // Move the light.
        if (glfwGetKey(GLFW_KEY_RIGHT))
            lightPos.x += kStepSize;
        if (glfwGetKey(GLFW_KEY_LEFT))
            lightPos.x -= kStepSize;
        if (glfwGetKey(GLFW_KEY_UP))
            lightPos.y += kStepSize;
        if (glfwGetKey(GLFW_KEY_DOWN))
            lightPos.y -= kStepSize;
        if (glfwGetKey('N'))
            lightPos.z += kStepSize;
        if (glfwGetKey('M'))
            lightPos.z -= kStepSize;

        // Update the camera direction.
        int newMouseX, newMouseY;
        glfwGetMousePos(&newMouseX, &newMouseY);
        double alphaXY = -(newMouseX-mouseX) / kPixelsPerRadian;
        double alphaZ = -(newMouseY-mouseY) / kPixelsPerRadian;
        mouseX = newMouseX; mouseY = newMouseY;
        cameraDir = cameraDir.rotatedZ(alphaXY);
        double phi = cameraDir.phi() + alphaZ;
        phi = (phi < -rho::geo::kPI/2+kCameraPhiEpsilon) ? -rho::geo::kPI/2+kCameraPhiEpsilon : phi;
        phi = (phi > rho::geo::kPI/2-kCameraPhiEpsilon) ? rho::geo::kPI/2-kCameraPhiEpsilon : phi;
        cameraDir.setPhi(phi);

        // Move the camera.
        rho::geo::tVector flatCameraDir = cameraDir.unit();
        flatCameraDir.setPhi(0);
        if (glfwGetKey('W'))
            cameraPos += cameraDir.unit() * kStepSize;
        if (glfwGetKey('S'))
            cameraPos -= cameraDir.unit() * kStepSize;
        if (glfwGetKey('A'))
            cameraPos += flatCameraDir.rotatedZ(rho::geo::kPI/2) * kStepSize;
        if (glfwGetKey('D'))
            cameraPos -= flatCameraDir.rotatedZ(rho::geo::kPI/2) * kStepSize;
        if (glfwGetKey(' '))
            cameraPos.z += kStepSize;
        if (glfwGetKey(GLFW_KEY_LSHIFT))
            cameraPos.z -= kStepSize;
    }

    // Close window and terminate GLFW.
    glfwTerminate();

    // Exit program.
    return 0;
}

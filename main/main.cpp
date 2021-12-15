/*
  CSCI 596 Final Project, USC
*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "basicPipelineProgram.h"
#include "openGLMatrix.h"
#include "imageIO.h"
#include "openGLHeader.h"
#include "glutHeader.h"
#include "Bmp.h"
#include "Util.h"
#include <math.h>
#include "httplib.h"

#include <iostream>
#include <cstring>


#if defined(WIN32) || defined(_WIN32)
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#if defined(WIN32) || defined(_WIN32)
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif

using namespace std;

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, Z_TRANSLATE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };
glm::vec3 camPos = { 0, 0, 5 };
glm::vec3 camCenter = { 0, 0, 0 };
glm::vec3 camUp = { 0, 1, 0};

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";

ImageIO * heightmapImage;

GLuint triVertexBuffer, triColorVertexBuffer, triNormalBuffer, textCoordBuffer;
GLuint triVertexArray;
GLuint texId;
int sizeMesh;

OpenGLMatrix matrix;
BasicPipelineProgram * pipelineProgram;

// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}

void displayFunc()
{
  // render some stuff...
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.LoadIdentity();
  glm::vec4 curCamPos = { camPos.x, camPos.y, camPos.z, 1 };
  glm::vec4 curCamUp = { camUp.x, camUp.y, camUp.z, 1 }; //get mat4 here to add translation to it
  glm::vec4 curCamCenter = { camCenter.x, camCenter.y, camCenter.z, 1 };

  glm::mat4 matRot = OpenGLMatrix::GetXRotMatrix(landRotate[0]);
  matRot = matRot * OpenGLMatrix::GetYRotMatrix(landRotate[1]);
  float xTrans = landTranslate[0];
  float yTrans = landTranslate[1];
  float zTrans = landTranslate[2];
  // cout << "xTrans: " << xTrans << endl;
  // cout << "yTrans: " << yTrans << endl;
  // cout << "zTrans: " << zTrans << endl;
  glm::mat4 matTrans = OpenGLMatrix::GetTransMatrix(xTrans, yTrans, zTrans);
  glm::mat4 mat = matRot * matTrans;

  curCamPos = mat * curCamPos;
  curCamCenter = matTrans * curCamCenter;
  curCamUp = matRot * curCamUp;
  matrix.LookAt(curCamPos.x, curCamPos.y, curCamPos.z, curCamCenter.x, curCamCenter.y, curCamCenter.z, curCamUp.x, curCamUp.y, curCamUp.z);

  float view[16];
  matrix.GetMatrix(view);

  //results of multiplying view with light vector (0, 0, 1)
  // float viewLightDirection[3] = { view[8] + 2, view[9] + 1, view[10] };
  //what if multiplied by another vector (say 1, 2, 5)
  float lightVector[4] = { -4, 1, 0, 1 };
  // float viewLightDirection[3] = { view[8] , view[9] , view[10] };
  float viewLightDirection[3] = { 
    (view[0] * lightVector[0]) + (view[4] * lightVector[1]) + (view[8] * lightVector[2]), 
    (view[1] * lightVector[0]) + (view[5] * lightVector[1]) + (view[9] * lightVector[2]), 
    (view[2] * lightVector[0]) + (view[6] * lightVector[1]) + (view[10] * lightVector[2]),  
  };
  // cout << 

  float m[16];
  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.GetMatrix(m);

  float p[16];
  matrix.SetMatrixMode(OpenGLMatrix::Projection);
  matrix.GetMatrix(p);

  float n[16];
  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.GetNormalMatrix(n); 
  //
  // bind shader
  pipelineProgram->Bind();
  pipelineProgram->SetViewLightDirection(viewLightDirection);

  // set variable
  pipelineProgram->SetModelViewMatrix(m);
  pipelineProgram->SetProjectionMatrix(p);
  pipelineProgram->SetNormalMatrix(n);

  glBindVertexArray(triVertexArray);
  glBindTexture(GL_TEXTURE_2D, texId);
  // sizeMesh = sizeMesh;
  glDrawArrays(GL_TRIANGLES, 0, sizeMesh);

  glutSwapBuffers();
}

void idleFunc()
{
  // do some stuff... 

  // for example, here, you can save the screenshots to disk (to make the animation)

  // make the screen update 
  glutPostRedisplay();
}

void reshapeFunc(int w, int h)
{
  glViewport(0, 0, w, h);

  matrix.SetMatrixMode(OpenGLMatrix::Projection);
  matrix.LoadIdentity();
  matrix.Perspective(54.0f, (float)w / (float)h, 0.01f, 100.0f);
}

void mouseMotionDragFunc(int x, int y)
{
  
  // mouse has moved and one of the mouse buttons is pressed (dragging)

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };
  switch (controlState)
  {
    // translate the landscape
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
        landTranslate[0] += mousePosDelta[0] * 0.01f;
        landTranslate[2] -= mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        landTranslate[2] += mousePosDelta[1] * 0.01f;
      }
      
      break;

    // rotate the landscape
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        landRotate[0] += mousePosDelta[1];
        landRotate[1] -= mousePosDelta[0];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        landRotate[2] += mousePosDelta[1];
      }
      if (rightMouseButton)
      {
        landTranslate[0] += mousePosDelta[0] * 0.01f;
        landTranslate[1] -= mousePosDelta[1] * 0.01f;
      }
      break;


    // scale the landscape
    case Z_TRANSLATE:
      if (leftMouseButton)
      {
        landTranslate[2] += mousePosDelta[1]* 0.01f;
      }
      break;
      // if (leftMouseButton)
      // {
      //   // control x,y scaling via the left mouse button
      //   landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
      //   landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      // }
      // if (middleMouseButton)
      // {
      //   // control z scaling via the middle mouse button
      //   landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      // }
      // if (rightMouseButton)
      // {
      //   landTranslate[2] += mousePosDelta[1] * 0.01f;
      // }
      // break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}


void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // keep track of whether CTRL and SHIFT keys are pressed
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = Z_TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = Z_TRANSLATE;
    break;
    case GLUT_ACTIVE_ALT:
      controlState = Z_TRANSLATE;

    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
      controlState = ROTATE;
    break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
      cout << "You pressed the spacebar." << endl;
    break;

    case 'x':
      // take a screenshot
      saveScreenshot("screenshot.jpg");
    break;
  }
}

void initScene(int argc, char *argv[])
{
  // load the image from a jpeg disk file to main memory
  // heightmapImage = new ImageIO();
  // if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  // {
  //   cout << "Error reading image " << argv[1] << "." << endl;
  //   exit(EXIT_FAILURE);
  // }

  //THIS DATA IS TEMPORARY, IT SHOULD BE REPLACED BY WHAT IS RETRIEVED FROM API
  int earthquakesTotal = 3;
  float latitudeList[3] = { 34.15, -29.119, -29.888};
  float longitudeList[3] = { -118.34, 26.083140, -71.621 };
  float magnitudeList[3] = { 7.0, 2.3, 4.0 };
  // float temp_latitude = 34.15;
  // float temp_longitude = -118.34;
  float temp_magnitude = 4.5;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  float radius = 1.0f;
  float sectorCount = 72;
  float stackCount = 36;

  float x, y, z, xy, xy1, z1;                              // vertex position
  float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
  float s, t;                                     // vertex texCoord
  float x11, y11, x21, y21, x12, y12, x22, y22, z11, z12;

  float sectorStep = 2 * M_PI / sectorCount;
  float stackStep = M_PI / stackCount;
  float sectorAngle = 0, stackAngle, stackAngle1 = 0, sectorAngle1;

  int totalVertices = (sectorCount + 1) * (stackCount + 1) * 6;
  glm::vec3 vertices[totalVertices];
  glm::vec3 normals[totalVertices];
  glm::vec2 texCords[totalVertices];

  //todo: remove this, this is just for testing
  glm::vec4 color[totalVertices];

  for(int i = 0; i <= stackCount; ++i)
  {
      float latitude = (i - 20) * -5;
      float nextLatitude = (i - 19) * -5;
      // if (stackAngle1 != 0)
        stackAngle1 = stackAngle;
      // else 
      //   stackAngle1 = M_PI / 2 - (stackCount - 1) * stackStep;
      stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
      xy = radius * cosf(stackAngle);             // r * cos(u)
      z11 = radius * sinf(stackAngle);              // r * sin(u)

      xy1 = radius * cosf(stackAngle1);
      z12 = radius * sinf(stackAngle1);

      // add (sectorCount+1) vertices per stack
      // the first and last vertices have same position and normal, but different tex coords
      for(int j = 0; j <= sectorCount; ++j)
      {
          glm::vec4 curColor = {0, 0, 0, 0};
          float longitute = (j - 36) * 5;
          float nextLongitude = (j - 35) * 5;
          for (int k = 0; k < earthquakesTotal; k++)
          {
            float temp_longitude = longitudeList[k];
            float temp_latitude = latitudeList[k];
            float temp_magnitude = magnitudeList[k];
            if (longitute < temp_longitude && temp_longitude <= nextLongitude
              && latitude >= temp_latitude && temp_latitude > nextLatitude)
            {
              float green= 1 - (temp_magnitude / 9.0f);
              if (curColor.x == 0 || green < curColor.y) //if color has already been assigned to this area, color has to be related to strongest magnitude
              {
                curColor.x = 1;
                curColor.y = green;
              }
            }
          } 
          sectorAngle = j * sectorStep;           // starting from 0 to 2pi
          sectorAngle1 = (j + 1) * sectorStep;
          // vertex position (x, y, z)
          x11 = xy * cosf(sectorAngle);
          y11 = xy * sinf(sectorAngle);
          x21 = xy * cosf(sectorAngle1);
          y21 = xy * sinf(sectorAngle1);
          x12 = xy1 * cosf(sectorAngle);
          y12 = xy1 * sinf(sectorAngle);
          x22 = xy1 * cosf(sectorAngle1);
          y22 = xy1 * sinf(sectorAngle1);

          int index = (j + (sectorCount * i)) * 6;
          vertices[index] = {x11, y11, z11};
          vertices[1 + index] = {x22, y22, z12};
          vertices[2 + index] = {x12, y12, z12};
          vertices[3 + index] = {x11, y11, z11};
          vertices[4 + index] = {x22, y22, z12};
          vertices[5 + index] = {x21, y21, z11};
          

          // normalized vertex normal (nx, ny, nz)
          nx = x * lengthInv;
          ny = y * lengthInv;
          nz = z * lengthInv;
          // normals[index] = {nx, ny, nz};
          normals[index] = {x11 * lengthInv, y11 * lengthInv, z11 * lengthInv};
          normals[1 + index] = {x22 * lengthInv, y22 * lengthInv, z12 * lengthInv};
          normals[2 + index] = {x12 * lengthInv, y12 * lengthInv, z12 * lengthInv};
          normals[3 + index] = {x11 * lengthInv, y11 * lengthInv, z11 * lengthInv};
          normals[4 + index] = {x22 * lengthInv, y22 * lengthInv, z12 * lengthInv};
          normals[5 + index] = {x21 * lengthInv, y21 * lengthInv, z11 * lengthInv};

          // vertex tex coord (s, t) range between [0, 1]
          float s1 = (float)j / sectorCount;
          float t1 = (float)i / stackCount;
          float s2 = (float)(j + 1) / sectorCount;
          float t2 = (float)(i - 1) / stackCount;

          texCords[index] = {s1, t1};
          texCords[index + 1] = {s2, t2};
          texCords[index + 2] = {s1, t2};
          texCords[index + 3] = {s1, t1};
          texCords[index + 4] = {s2, t2};
          texCords[index + 5] = {s2, t1};


          color[index] = curColor;
          color[1 + index] = curColor;
          color[2 + index] = curColor;
          color[3 + index] = curColor;
          color[4 + index] = curColor;
          color[5 + index] = curColor;
      }
  }

  sizeMesh = totalVertices;

  glGenBuffers(1, &triVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, triVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sizeMesh, vertices,
               GL_STATIC_DRAW);
  glGenBuffers(1, &triNormalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, triNormalBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sizeMesh, normals,
               GL_STATIC_DRAW);


  glGenBuffers(1, &triColorVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, triColorVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * sizeMesh, color, GL_STATIC_DRAW);


  glGenBuffers(1, &textCoordBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, textCoordBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * sizeMesh, texCords, GL_STATIC_DRAW);

  pipelineProgram = new BasicPipelineProgram;
  int ret = pipelineProgram->Init(shaderBasePath);
  if (ret != 0) abort();

  glGenVertexArrays(1, &triVertexArray);
  glBindVertexArray(triVertexArray);
  glBindBuffer(GL_ARRAY_BUFFER, triVertexBuffer);
  GLuint loc =
      glGetAttribLocation(pipelineProgram->GetProgramHandle(), "position");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, triNormalBuffer);
  loc =
      glGetAttribLocation(pipelineProgram->GetProgramHandle(), "normal");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, triColorVertexBuffer);

  loc = glGetAttribLocation(pipelineProgram->GetProgramHandle(), "color");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, (const void *)0);

  // glBindBuffer(GL_ARRAY_BUFFER, texId);

  glBindBuffer(GL_ARRAY_BUFFER, textCoordBuffer);
  loc = glGetAttribLocation(pipelineProgram->GetProgramHandle(), "texCoord");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);

  glGenTextures(1, &texId);

  glEnable(GL_DEPTH_TEST);

  
  pipelineProgram->Bind();
  float la[4] = { 0.5f, 0.5f, 0.5f, 1 };
  float ld[4] = { 0.5f, 0.5f, 0.5f, 1 };
  float ls[4] = { 0, 0, 0, 1 };
  float ka[4] = { 0.5f, 0.5f, 0.5f, 0.5f }; 
  float kd[4] = { 0.5f, 0.5f, 0.5f, 0.5f }; 
  float ks[4] = { 0, 0, 0, 0 }; 
  float alpha = 2;
  pipelineProgram->SetPhong(la, ld, ls, ka, kd, ks, alpha);

  std::cout << "GL error: " << glGetError() << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// load raw image as a texture
///////////////////////////////////////////////////////////////////////////////
GLuint loadTexture(const char* fileName, bool wrap)
{
    Image::Bmp bmp;
    if(!bmp.read(fileName))
        return 0;     // exit if failed load image

    // get bmp info
    int width = bmp.getWidth();
    int height = bmp.getHeight();
    const unsigned char* data = bmp.getDataRGB();
    // cout <<  << endl;
    std::cout << std::isprint(data[0]) << std::endl;

    cout << std::isprint(data[1]) << endl;
    cout << std::isprint(data[2]) << endl;
    GLenum type = GL_UNSIGNED_BYTE;    // only allow BMP with 8-bit per channel

    // We assume the image is 8-bit, 24-bit or 32-bit BMP
    GLenum format;
    int bpp = bmp.getBitCount();
    if(bpp == 8)
        format = GL_LUMINANCE;
    else if(bpp == 24)
        format = GL_RGB;
    else if(bpp == 32)
        format = GL_RGBA;
    else
        return 0;               // NOT supported, exit

    // gen texture ID
    GLuint texture;
    glGenTextures(1, &texture);

    // set active texture and configure it
    glBindTexture(GL_TEXTURE_2D, texture);

    // // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // copy texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // build our texture mipmaps
    switch(bpp)
    {
    case 8:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_LUMINANCE, type, data);
        break;
    case 24:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, type, data);
        break;
    case 32:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, type, data);
        break;
    }

    return texture;
}

int main(int argc, char *argv[])
{
  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  #ifdef __APPLE__
    // This is needed on recent Mac OS X versions to correctly display the window.
    glutReshapeWindow(windowWidth - 1, windowHeight - 1);
  #endif

  // tells glut to use a particular display function to redraw 
  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);

  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif

  httplib::Params params;
  params.emplace("format", "geojson");
  params.emplace("starttime", "2014-01-01");
  params.emplace("endtime", "2014-01-02");
  
  httplib::SSLClient cli("earthquake.usgs.gov");
  cli.set_ca_cert_path("./ca-bundle.crt");
  cli.enable_server_certificate_verification(true);

  std::string path_with_query = httplib::append_query_params("/fdsnws/event/1/query", params);

  // httplib::Result res = cli.Get("/fdsnws/event/1/query?endtime=2014-01-02&format=geojson&starttime=2014-01-01", [&](const char *data, size_t data_length) {
  //     std::cout << "data came through" << endl;
  //     std::cout << string(data, data_length) << endl;
  //     return true;
  //   });
  // httplib::Result res = cli.Get(path_with_query.c_str());
  httplib::Result res = Util::query(params);

  if (res) {
      int status = res->status; // should be 200 if succeed
      string header_value = res->get_header_value("Content-Type");
      string body = res->body; // content

      cout << status << endl;
      cout << header_value << endl;
      cout << body << endl;
  }
  else {
      httplib::Error error = res.error();
      cout << "error code: " << error << endl;
  }

  // cout << res->status << endl;
  // if (!res)
  // {
  //   cout << "res is null" << endl;
  // }
  

  initScene(argc, argv);

  // texId = loadTexture("moon1024.bmp", true);
  texId = loadTexture("earth2048.bmp", true);
  // sink forever into the glut loop
  glutMainLoop();
}



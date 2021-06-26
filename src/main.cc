#include "app.h"
#include "apps/cube/cubeApp.h"

Application* gApplication;

int main(int argc, char *argv[])
{
   gApplication = new CubeApplication;
   gApplication->init();
   
   while (gApplication->update());
   
   gApplication->destroy();
   return 0;
}


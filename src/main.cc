#include "app.h"
#include "apps/main/mainApp.h"

Application* gApplication;

int main(int argc, char *argv[])
{
   gApplication = new MainApplication;
   gApplication->init();
   
   while (gApplication->update());
   
   gApplication->destroy();
   return 0;
}


#include "app.h"
#include "sandbox.cc"


int main(int argc, char *argv[])
{
   Application* app = new SandboxApplication;
   app->init();
   
   while (app->update());
   
   app->destroy();
   return 0;
}


#pragma once

#include <vector>
#include "app.h"

class MainApplication : public Application
{
   std::vector<ApplicationRep*> apps;
   std::vector<const char*> names;
   int selectedIndex = 0;
   
public:
   DECLARE_APPLICATION(MainApplication);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;
};

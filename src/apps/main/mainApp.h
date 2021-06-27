#pragma once

#include "app.h"

class MainApplication : public Application
{
public:
   DECLARE_APPLICATION(MainApplication);

   virtual void onWindowSizeUpdate(int width, int height) override;

   virtual void onInit() override;
   virtual void onDestroy() override;
   virtual void onUpdate(double dt) override;
   virtual void onRenderImGUI(double dt) override;
};

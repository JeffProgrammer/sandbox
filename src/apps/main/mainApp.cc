#include <imgui.h>
#include <glad/glad.h>
#include "apps/main/mainApp.h"

IMPLEMENT_APPLICATION(MainApplication);

void MainApplication::onWindowSizeUpdate(int width, int height)
{

}

void MainApplication::onInit()
{
   apps = ApplicationRep::getListOfApplications();
   
   names.clear();
   for (const ApplicationRep* rep : apps)
   {
      if (rep->mName == "MainApplication")
         continue;
      
      names.push_back(rep->mName.c_str());
   }
}

void MainApplication::onDestroy()
{

}

void MainApplication::onUpdate(double dt)
{
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT);
}

void MainApplication::onRenderImGUI(double dt)
{
   ImGui::NewFrame();
   
   ImGui::SetNextWindowPos(
      ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
      ImGuiCond_Always,
      ImVec2(0.5f,0.5f)
   );
   ImGui::SetNextWindowSize(ImVec2(800, 600));

   ImGui::Begin("Application");
   ImGui::ListBox("", &selectedIndex, names.data(), (int)names.size());
   ImGui::End();
   
   ImGui::Render();
}

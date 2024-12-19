#include "Window.h"

int main()
{
  try
  {
    Window mainWindow({1600, 900}, "3D-fluid-simulator");
    mainWindow.EventLoop();
  }
  catch (std::exception const& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
  return 0;
}

#include <3D-fluid-simulator.h>

int main()
{
	std::srand(0);
	try {
		Window mainWindow({ 1280, 720 }, "Laxuv");
		mainWindow.EventLoop();
	}
	catch(std::exception const& e){
		std::cerr << e.what() << std::endl;
		return -1;
	}
	return 0;
}

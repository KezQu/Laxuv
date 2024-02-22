﻿// Laxuv.cpp : Defines the entry point for the application.
//

#include "Laxuv.h"

int main()
{
	try {
		Window mainWindow({ 1920, 1080 }, "Laxuv");
		mainWindow.EventLoop();
	}
	catch(std::exception const& e){
		std::cerr << e.what() << std::endl;
		return -1;
	}
	return 0;
}

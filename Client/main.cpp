#include "Client.h"

int main(int argc, char* argv[])
{
	auto app = new Client();

	if (argc > 1)
		app->IP = argv[1];

	app->run("AIE", 1280, 720, false);

	delete app;

	return 0;
}
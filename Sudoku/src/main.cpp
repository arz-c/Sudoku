#include "App.h"

App app;

int main(int argc, char *argv[]) {
	app.init("Sudoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, false);
	
	while(app.isRunning()) {
		app.handleEvents();
		app.render();
	}

	app.clean();
	return 0;
}
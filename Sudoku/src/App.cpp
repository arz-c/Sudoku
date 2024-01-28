#include "App.h"

App::App() :
	m_board(),
	b_isRunning(false),
	m_window(nullptr),
	m_renderer(nullptr),
	m_rectWidth(0),
	m_rectHeight(0),
	m_tileData(),
	m_fontRectData(),
	m_fontTextureData(),
	m_fontDataLengths(),
	m_selectedTile {-1, -1},
	m_font(nullptr),
	m_bigFont(nullptr),
	m_colour(),
	m_tileLastNum() {}

void App::init(const char *title, const int xpos, const int ypos, const int width, const int height, const bool isFullscreen) {
	// Init Window
	if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		//std::cout << "Subsystems initialized!..." << std::endl;

		m_window = SDL_CreateWindow(title, xpos, ypos, width, height, isFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		if(m_window)
			std::cout << "Window created!..." << std::endl;

		m_renderer = SDL_CreateRenderer(m_window, -1, 0);
		if(m_renderer)
			//SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
			std::cout << "Renderer created!..." << std::endl;

		if(TTF_Init() == 0)
			std::cout << "TTF initialized!..." << std::endl;
		else
			std::cout << "Error while initializing TTF: " << TTF_GetError() << std::endl;

		b_isRunning = true;
		std::cout << "App initialized successfully!" << std::endl;
	} else {
		std::cout << "Error while initializing SDL: " << SDL_GetError() << std::endl;
		b_isRunning = false;
	}

	// Init Other
	m_rectWidth = width / 9;
	m_rectHeight = height / 9;

	m_font = TTF_OpenFont("BadaBoomBB.ttf", App::SMALL_FONT_SIZE);
	m_bigFont = TTF_OpenFont("BadaBoomBB.ttf", App::BIG_FONT_SIZE);
	m_colour = {0, 0, 0, 255};

	char startingBoard[9][9] = {
		{0, 0, 1, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 2},
		{0, 0, 5, 0, 7, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 9, 0, 0, 0, 4},
		{0, 0, 0, 2, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 4},
		{0, 0, 0, 0, 0, 0, 0, 0, 9},
		{0, 0, 0, 0, 0, 0, 0, 0, 0}
	};

	for(int x = 0; x < 9; x++) {
		m_tileData[x][9] = SDL_Rect {x * m_rectWidth, (2 + 1) * m_rectHeight, m_rectWidth, App::THICKNESS};
		m_tileData[x][10] = SDL_Rect {x * m_rectWidth, (5 + 1) * m_rectHeight, m_rectWidth, App::THICKNESS};
	}

	for(int y = 0; y < 9; y++) {
		m_tileData[9][y] = SDL_Rect {(2 + 1) * m_rectWidth, y * m_rectHeight, App::THICKNESS, m_rectHeight};
		m_tileData[10][y] = SDL_Rect {(5 + 1) * m_rectWidth, y * m_rectHeight, App::THICKNESS, m_rectHeight};
	}

	for(int x = 0; x < 9; x++) {
		for(int y = 0; y < 9; y++) {
			m_tileData[x][y] = SDL_Rect {y * m_rectWidth, x * m_rectHeight, m_rectWidth, m_rectHeight};
			m_fontDataLengths[x][y] = 0;
			m_tileLastNum[x][y] = -1;

			if(startingBoard[x][y] != 0) {
				m_selectedTile[0] = x;
				m_selectedTile[1] = y;
				applyText((char) startingBoard[x][y] + 0x30);
				enlargeLast();
			}

		}
	}

	m_selectedTile[0] = 0;
	m_selectedTile[1] = 0;

	std::cout << "Variables initialized successfully!" << std::endl;
}

App::~App() {}

void App::clean() {
	for(int x = 0; x < 9; x++) {
		for(int y = 0; y < 9; y++) {
			for(int i = 0; i < 9; i++) {
				if(m_fontDataLengths[x][y] > 0)
					SDL_DestroyTexture(m_fontTextureData[x][y][i]);
			}
		}
	}
	TTF_CloseFont(m_font);
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);

	TTF_Quit();
	SDL_Quit();

	std::cout << "App cleaned successfully!" << std::endl;
}

void App::setSelected(const int xpos, const int ypos) {
	for(int x = 0; x < 9; x++) {
		for(int y = 0; y < 9; y++) {
			SDL_Rect currentTileData = m_tileData[x][y];
			if(xpos > currentTileData.x &&
			   xpos < currentTileData.x + m_rectWidth &&
			   ypos > currentTileData.y &&
			   ypos < currentTileData.y + m_rectHeight) {
				m_selectedTile[0] = x;
				m_selectedTile[1] = y;
				return;
			}
		}
	}
}

void App::applyText(char num) {
	//std::cout << num << std::endl;

	int sx = m_selectedTile[0];
	int sy = m_selectedTile[1];

	//char temp[2] = {0};
	//temp[0] = num;
	SDL_Surface *surface = TTF_RenderText_Solid(m_font, &num, m_colour);
	SDL_Texture *text = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);

	int *n = &m_fontDataLengths[sx][sy];
	if(*n == -1)
		*n = 0;
	int x = *n % 3;
	int y = floor(*n / 3);
	SDL_Rect tileRect = m_tileData[sx][sy];
	int w, h;
	SDL_QueryTexture(text, NULL, NULL, &w, &h);
	//std::cout << tileRect.x + m_rectWidth / 2 - w / 2 << " " << tileRect.y + m_rectHeight / 2 - h / 2 << std::endl;
	SDL_Rect rect = {
		tileRect.x + x * m_rectWidth / 3 + m_rectWidth / 3 / 2 - w / 2,
		tileRect.y + y * m_rectHeight / 3 + m_rectHeight / 3 / 2 - h / 2,
		w, h
	};

	/*
	center coords:
	tileRect.x + m_rectWidth / 2 - w / 2,
	tileRect.y + m_rectHeight / 2 - h / 2
	*/

	m_tileLastNum[sx][sy] = num;
	m_fontRectData[sx][sy][*n] = rect;
	m_fontTextureData[sx][sy][*n] = text;
	if(*n < 9)
		(*n)++;
}

void App::enlargeLast() {
	int sx = m_selectedTile[0];
	int sy = m_selectedTile[1];

	if(m_tileLastNum[sx][sy] <= 0)
		return;

	//std::cout << m_tileLastNum[sx][sy] << std::endl;
	char temp[2] = {0};
	temp[0] = m_tileLastNum[sx][sy];
	SDL_Surface *surface = TTF_RenderText_Solid(m_bigFont, temp, m_colour);
	SDL_Texture *text = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Rect tileRect = m_tileData[sx][sy];
	int w, h;
	SDL_QueryTexture(text, NULL, NULL, &w, &h);

	SDL_Rect rect = {
		tileRect.x + m_rectWidth / 2 - w / 2,
		tileRect.y + m_rectHeight / 2 - h / 2,
		w, h
	};

	m_fontRectData[sx][sy][0] = rect;
	m_fontTextureData[sx][sy][0] = text;
	m_fontDataLengths[sx][sy] = -1;
}

void App::handleEvents() {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type) {
		case SDL_QUIT:
			b_isRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if(event.button.button == SDL_BUTTON_LEFT) {
				int x, y;
				SDL_GetMouseState(&x, &y);
				setSelected(x, y);
			}
			break;
		case SDL_KEYDOWN:
			int code = event.key.keysym.sym;
			//std::cout << code << std::endl;
			if(code >= 49 && code <= 57) { // number keys
				applyText((char) (code - 48) + 0x30);
			} else {
				switch(code) {
					case 8: // backspace
						if(m_fontDataLengths[m_selectedTile[0]][m_selectedTile[1]] > 0)
							m_fontDataLengths[m_selectedTile[0]][m_selectedTile[1]]--;
						else if(m_fontDataLengths[m_selectedTile[0]][m_selectedTile[1]] == -1)
							m_fontDataLengths[m_selectedTile[0]][m_selectedTile[1]] = 0;
						break;
					case 1073741912: // numpad enter
					case 13: // enter
						enlargeLast();
						break;
					case 1073741903: // right-arrow
						if(m_selectedTile[1] < 8)
							m_selectedTile[1]++;
						break;
					case 1073741904: // left-arrow
						if(m_selectedTile[1] > 0)
							m_selectedTile[1]--;
						break;
					case 1073741905: // down-arrow
						if(m_selectedTile[0] < 8)
							m_selectedTile[0]++;
						break;
					case 1073741906: // up-arrow
						if(m_selectedTile[0] > 0)
							m_selectedTile[0]--;
						break;
				}
			}
	}
}

void App::update() {

}

void App::render() {
	SDL_RenderClear(m_renderer);

	for(int x = 0; x < 11; x++) {
		for(int y = 0; y < 11; y++) {
			SDL_Rect rect = m_tileData[x][y];
			//std::cout << currentTileData[0] << " " << currentTileData[1] << std::endl;

			if(x == m_selectedTile[0] && y == m_selectedTile[1]) // selected
				SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
			else // unselected
				SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

			if(x >= 9 || y >= 9)
				SDL_RenderFillRect(m_renderer, &rect);
			else
				SDL_RenderDrawRect(m_renderer, &rect);
		}
	}

	for(int x = 0; x < 9; x++) {
		for(int y = 0; y < 9; y++) {
			for(int i = 0; i < 9; i++) {
				if(i < m_fontDataLengths[x][y])
					SDL_RenderCopy(m_renderer, m_fontTextureData[x][y][i], NULL, &(m_fontRectData[x][y][i]));
				else if(m_fontDataLengths[x][y] == -1) {
					SDL_RenderCopy(m_renderer, m_fontTextureData[x][y][0], NULL, &(m_fontRectData[x][y][0]));
					break;
				}
			}
		}
	}

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	SDL_RenderPresent(m_renderer);
}
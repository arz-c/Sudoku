#pragma once

#include <iostream>
#include "SDL.h"
#include "SDL_ttf.h"

class App {
public:
	App();
	void init(const char *title, const int xpos, const int ypos, const int width, const int height, const bool isFullscreen);

	~App();
	void clean();
	
	void handleEvents();
	void update();
	void render();

	bool isRunning() {
		return b_isRunning;
	};
private:
	static const int THICKNESS = 4;
	static const int SMALL_FONT_SIZE = 20;
	static const int BIG_FONT_SIZE = 50;

	bool b_isRunning;
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;

	int m_board[9][9];

	int m_rectWidth, m_rectHeight;
	SDL_Rect m_tileData[11][11];
	SDL_Rect m_fontRectData[9][9][9];
	SDL_Texture *m_fontTextureData[9][9][9];
	int m_fontDataLengths[9][9];
	char m_tileLastNum[9][9];
	int m_selectedTile[2];

	TTF_Font *m_font;
	TTF_Font *m_bigFont;
	SDL_Color m_colour;

	void setSelected(int x, int y);
	void applyText(char num);
	void removeLast();
	void enlargeLast();
};
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <windows.h>
#include <iostream>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset); 
// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color); 
// rysowanie prostok¹ta o d³ugoœci boków l i k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
//zakoñczenie programu
int finish_program(SDL_Surface* charset, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer);
//legenda
void legend(char* text, SDL_Surface* charset, SDL_Surface* screen);
//struktura auta
struct carType {
	double spawnTimer = 0;
	int car_x = SCREEN_WIDTH/2+1;
	int car_y = 0;
	bool exist = false;
	double time_limit = 0;
	double deathTimer = 0;
	bool shot = false;
};
//tworzenie aut
void spawn(struct carType* p, SDL_Surface* screen, SDL_Surface* car_bmp, struct carType* r, struct carType* s, int immortality_x);
//wyœwietlanie drogi
void load_road(int& roadY, int roadHeight, int roadWidth, SDL_Surface* road, SDL_Surface* screen, SDL_Surface* car, double x, double y, SDL_Window* window);
//wybuch offroad
void offroad(double& x, double& y, double& time_offroad, double delta, SDL_Surface* screen, SDL_Surface* kaboom, bool& pause, double& lifes, bool endlessLifeTime, double& lostLifeScore, double score, bool god_mode);
//spychanie oponenta
void pushing(double& x, double& y, struct carType* p);
//zliczanie puntków
void scoreSystem(double x, bool& pointsFreeze, double& score_timer, double& delta, double& score);
//atak r34
void r34_atack(double y, double x, struct carType* p);
//fps
void fpsCount(double& fpsTimer, double delta, int& frames, double& fps); 
//nag³ówek
void header(SDL_Surface* screen, char* text, double fps, double score, double worldTime, SDL_Surface* charset, int czerwony, int niebieski, double licznik, double lifes); 
//wczytywanie bmp
void loading_bmp(SDL_Surface*& charset, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer, SDL_Surface*& road, SDL_Surface*& car, SDL_Surface*& normal_bmp, SDL_Surface*& supra_bmp, SDL_Surface*& r34_bmp, SDL_Surface*& kaboom, SDL_Surface*& strzal, SDL_Surface*& grzib, SDL_Surface*& gameOver_bmp);
//obs³uga zdarzeñ
void events(SDL_Event& event, int& quit, bool& pointsFreeze, double& score_timer, double delta, double& score, int& roadY, struct carType* p, struct carType* r, struct carType* s, double& x, bool& pause, double& worldTime, double& etiSpeed, char* text, SDL_Surface* charset, SDL_Surface* screen, bool &gun, double &gun_x, double& gun_y, bool& new_game, bool &saving, bool &loading, int& cursor_y, double& lifes, double& licznik, char* tabBuff[]);
//zapchniêcie wroga
void carDestruction(struct carType* p, SDL_Surface* kaboom, double& score, bool& carHit, double& timeSinceHit, double& delta, bool& pointsFreeze, SDL_Surface* screen); 
//zepchniêcie cywila
void civilianDestruction(struct carType* p, SDL_Surface* kaboom, bool& pointsFreeze, SDL_Surface* screen, double& distance, double& dystansPoMorderstwie); 
//zderzenie
void collision(struct carType* p, double& x, double& y, SDL_Surface* kaboom, SDL_Surface* screen, bool& pause, double& timeSinceHit, double& delta, double& lifes, bool endlessLifeTime, double& lostLifeScore, double score, bool god_mode);
//atak supry
void supra_atack(double x, struct carType* p);
//sprawdzanie œmierci
void deathChecker(double &lifes, bool endlessLifeTime, double &lostLifeScore, double score, bool god_mode) {
	if ((!endlessLifeTime) && (!god_mode)) {
		lifes--;
		lostLifeScore = score;
	}
};
//zastrzelenie cywila
void shotCivilian(bool& gun, SDL_Surface* screen, double& gun_y, double& gun_x, SDL_Surface* strzal, struct carType* p, bool& pointsFreeze, double delta, SDL_Surface* kaboom); 
//zastrzelenie wroga
void shotEnemy(bool& gun, SDL_Surface* screen, double& gun_y, double& gun_x, SDL_Surface* strzal, struct carType* p, double delta, SDL_Surface* kaboom, double& score); 
//zdobywanie serc co 5000pkt
void getLifes(double licznik, bool& endlessLifeTime, double& lifes, double& lostLifeScore, double score); 
//niesmietelnosc (power-up)
void powerup(double& immortality_timer, bool& grzybeczek, int& immortality_x, int& immortality_y, double x, double y, SDL_Surface* screen, double& god_mode_timer, bool& god_mode, double delta, SDL_Surface* grzib, struct carType* p, struct carType* r, struct carType* s); 
//cos tam
void loadChosen(int wybranySave, double &score, double &worldTime, double &lifes, double &licznik, char* tabBuff[]) {
	char* finalFileDecision = tabBuff[wybranySave];
	FILE* pointerDisapointer = fopen(finalFileDecision, "r");
	if (!pointerDisapointer) {
		printf("lipa");
		return;
	}
	fclose(pointerDisapointer);
};

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	char text[128], fileName[261];
	int t1, t2, quit = 0, frames = 0, rc, roadWidth = SCREEN_WIDTH, roadHeight = SCREEN_HEIGHT, roadY = -450, immortality_x = SCREEN_WIDTH / 2, immortality_y = 0, cursor_y = 115;
	double delta, worldTime = 0, fpsTimer = 0, fps = 0, distance = 0, etiSpeed = 1, x = SCREEN_WIDTH / 2, y = 400, score = 0, time_offroad = 0, score_timer = 0, god_mode_timer = 0;
	double timeSinceHit = 0, dystansPoMorderstwie = 0, timer = 0, gun_x = 0, gun_y = y - 70, lifes = 3, licznik = 90, immortality_timer = 0, pointsBuylife = 0, lostLifeScore = 0;
	bool pause = false, pointsFreeze = false, carHit = false, gun = false, civilianCarHit = false, fiveSecondsPassed = false, endlessLifeTime = true, grzybeczek = false;
	bool god_mode = false, new_game = false, saving = false, loading = false, readingGameState = false;
	SDL_Event event;
	SDL_Surface *screen, *charset, *car, *background, *kaboom, *road, *normal_bmp, *supra_bmp, *r34_bmp, *strzal, *grzib, *gameOver_bmp;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "Patryk Miszke 193249");
	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_ShowCursor(SDL_DISABLE);
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00), zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00), czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00), niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	// wczytywanie bmp
	loading_bmp(charset, screen, scrtex, window, renderer, road, car, normal_bmp, supra_bmp, r34_bmp, kaboom, strzal, grzib, gameOver_bmp);
	t1 = SDL_GetTicks();
	srand(time(NULL));

	carType normal;			normal.time_limit = 10;
	carType supra;			supra.time_limit = 15;
	carType r34;			r34.time_limit = 5;

	char* tabBuff[10];

	while(!quit) {
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		if (pause == false) {
			worldTime += delta;
			distance += etiSpeed * delta;
			SDL_FillRect(screen, NULL, czarny);
			//wyœwietlanie drogi
			load_road(roadY, roadHeight, roadWidth, road, screen, car, x, y, window);
			//zliczanie punktów
			scoreSystem(x, pointsFreeze, score_timer, delta, score);
			//licznik czasowy
			immortality_timer += delta;
			normal.spawnTimer += delta;
			supra.spawnTimer += delta;
			r34.spawnTimer += delta;
			//tworzenie aut
			spawn(&normal, screen, normal_bmp, &supra, &r34, immortality_x);
			spawn(&supra, screen, supra_bmp, &normal, &r34, immortality_x);
			spawn(&r34, screen, r34_bmp, &normal, &supra, immortality_x);
			//przepychanie/atak
			pushing(x, y, &normal);
			pushing(x, y, &supra);
			pushing(x, y, &r34);
			r34_atack(y, x, &r34);
			supra_atack(x, &supra);
			//wypchniecie z planszy
			carDestruction(&supra, kaboom, score, carHit, timeSinceHit, delta, pointsFreeze, screen);
			carDestruction(&r34, kaboom, score, carHit, timeSinceHit, delta, pointsFreeze, screen);
			civilianDestruction(&normal, kaboom, pointsFreeze, screen, distance, dystansPoMorderstwie);
			//strzelañsko
			shotCivilian(gun, screen, gun_y, gun_x, strzal, &normal, pointsFreeze, delta, kaboom);
			shotEnemy(gun, screen, gun_y, gun_x, strzal, &supra, delta, kaboom, score);
			shotEnemy(gun, screen, gun_y, gun_x, strzal, &r34, delta, kaboom, score);
			//zdobywanie zyc
			getLifes(licznik, endlessLifeTime, lifes, lostLifeScore, score);
			//immortal
			powerup(immortality_timer, grzybeczek, immortality_x, immortality_y, x, y, screen, god_mode_timer, god_mode, delta, grzib, &normal, &supra, &r34);
			if (licznik > 0) {
				licznik -= delta;
			}
		}
		if (lifes <= 0) {
			DrawSurface(screen, gameOver_bmp, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			pause = true;
		}
		if (new_game == true) {
			worldTime = 0;
			score = 0;
			normal.exist = false;
			supra.exist = false;
			r34.exist = false;
			grzybeczek = false;
			immortality_y = 0;
			immortality_x = 0;
			immortality_timer = 0;
			endlessLifeTime = true;
			licznik = 90;
			distance = 0;
			lifes = 3;
			pause = false;
			new_game = false;
		}

		if (saving == true) {
			time_t t = time(0);
			struct tm* now = localtime(&t);

			char buffer[80];
			char saveBuffer[1000];
			strftime(buffer, 80, "%d.%m.%Y-%H.%M.%S.txt", now);
			snprintf(saveBuffer, sizeof(saveBuffer), "zapisane\\%s", buffer);

			FILE* file = fopen(saveBuffer, "wb");
			//sprawdzanie pliku
			double toSave[4] = { score, worldTime, licznik, lifes };
			double bongo = 0;
			for (int i = 0; i < 4; i++) {
				bongo = toSave[i] + 0.1;
				char tablica[sizeof(bongo)+1];
				tablica[sizeof(bongo)] = '\0';
				memcpy(tablica, &bongo, sizeof(bongo));
				fputs(tablica, file);
				//fgets()
			}
			fclose(file);
			saving = false;
		}

		if (readingGameState == true) {
			char b[8];
			char fileTab[] = "zapisane\\";
			char fileBuffer[1000];
			fileName[260] = '\0';
			snprintf(fileBuffer, sizeof(fileBuffer), "zapisane\\%s", fileName);
			FILE* fileRead = fopen(fileBuffer, "rb");
			double odczyt = 0;
			/*for (int i = 0; i < 2; i++) {
				fread(&b, 8, 1, fileRead);
				odczyt = *((double*)b);
			}*/
			fread(&b, 8, 1, fileRead);
			odczyt = *((double*)b);
			sprintf(text, "Odczyt = %.1lf", odczyt);
			DrawString(screen, 400, 300, text, charset);
			pause = true;
			fclose(fileRead);
		}

		
		collision(&normal, x, y, kaboom, screen, pause, timer, delta, lifes, endlessLifeTime, lostLifeScore, score, god_mode);
		collision(&supra, x, y, kaboom, screen, pause, timer, delta, lifes, endlessLifeTime, lostLifeScore, score, god_mode);
		collision(&r34, x, y, kaboom, screen, pause, timer, delta, lifes, endlessLifeTime, lostLifeScore, score, god_mode);
		offroad(x, y,  time_offroad, delta, screen, kaboom, pause, lifes, endlessLifeTime, lostLifeScore, score, god_mode);
		fpsCount(fpsTimer, delta, frames, fps);
		// Nag³ówek
		header(screen, text, fps, score, worldTime, charset, czerwony, niebieski, licznik, lifes);
		//Legenda
		if (lifes > 0) {
			legend(text, charset, screen);
		}
		
		if (loading == true) {
			pause = true;
			DrawRectangle(screen, 200, 80, 250, 300, czerwony, niebieski);
			DrawString(screen, 220, cursor_y, "->", charset);
			sprintf(text, "Wczytaj gre");
			DrawString(screen, 270, 90, text, charset);
			int i = 115;
			int number = 1;
			WIN32_FIND_DATAA data;
			HANDLE hFind = FindFirstFileA(".\\zapisane\\*", &data);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if (data.cFileName[0] != '.') {
						strcpy(fileName, data.cFileName);
						tabBuff[number-1] = fileName;
						sprintf(text, "%d %s", number, fileName);
						DrawString(screen, 250, i, text, charset);
						i += 20;
						number++;
					}
				} while (FindNextFileA(hFind, &data));	
			}
			FindClose(hFind);
		}
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		// obs³uga zdarzeñ (o ile jakieœ zasz³y) 
		events(event, quit, pointsFreeze, score_timer, delta, score, roadY, &normal, &supra, &r34, x, pause, worldTime, etiSpeed, text, charset, screen, gun, gun_x, gun_y, new_game, saving, loading, cursor_y, lifes, licznik, tabBuff);
		frames++;
		};
	// zwolnienie powierzchni 
	SDL_FreeSurface(car);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};

	void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
		int px, py, c;
		SDL_Rect s, d;
		s.w = 8;
		s.h = 8;
		d.w = 8;
		d.h = 8;
		while (*text) {
			c = *text & 255;
			px = (c % 16) * 8;
			py = (c / 16) * 8;
			s.x = px;
			s.y = py;
			d.x = x;
			d.y = y;
			SDL_BlitSurface(charset, &s, screen, &d);
			x += 8;
			text++;
		};
	};
	void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
		SDL_Rect dest;
		dest.x = x - sprite->w / 2;
		dest.y = y - sprite->h / 2;
		dest.w = sprite->w;
		dest.h = sprite->h;
		SDL_BlitSurface(sprite, NULL, screen, &dest);
	};
	void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
		int bpp = surface->format->BytesPerPixel;
		Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
		*(Uint32*)p = color;
	};
	void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
		for (int i = 0; i < l; i++) {
			DrawPixel(screen, x, y, color);
			x += dx;
			y += dy;
		};
	};
	void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
		Uint32 outlineColor, Uint32 fillColor) {
		int i;
		DrawLine(screen, x, y, k, 0, 1, outlineColor);
		DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
		DrawLine(screen, x, y, l, 1, 0, outlineColor);
		DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
		for (i = y + 1; i < y + k - 1; i++)
			DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};
	int finish_program(SDL_Surface* charset, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer) {
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	void legend(char* text, SDL_Surface* charset, SDL_Surface* screen) {
		sprintf(text, "spacja - strzal");
		DrawString(screen, 495, 300, text, charset);
		sprintf(text, "l - wczytanie gry");
		DrawString(screen, 495, 320, text, charset);
		sprintf(text, "s - zapis");
		DrawString(screen, 495, 340, text, charset);
		sprintf(text, "p - pauza (on/off)");
		DrawString(screen, 495, 360, text, charset);
		sprintf(text, "strzalki - ruch");
		DrawString(screen, 495, 380, text, charset);
		sprintf(text, "Esc - wyjscie");
		DrawString(screen, 495, 400, text, charset);
		sprintf(text, "\030 - przyspieszenie");
		DrawString(screen, 495, 420, text, charset);
		sprintf(text, "\031 - zwolnienie");
		DrawString(screen, 495, 440, text, charset);
		sprintf(text, "n - nowa gra");
		DrawString(screen, 495, 460, text, charset);
	};
	void load_road(int& roadY, int roadHeight, int roadWidth, SDL_Surface* road, SDL_Surface* screen, SDL_Surface* car, double x, double y, SDL_Window* window) {
		roadY += 2;
		for (int i = roadY; i < roadY + SCREEN_HEIGHT; i += roadHeight) {
			SDL_Rect dest;
			dest.x = 0;
			dest.y = i;
			dest.w = roadWidth;
			dest.h = roadHeight;
			SDL_BlitSurface(road, NULL, screen, &dest);
		}
		DrawSurface(screen, car, x, y);
		if (roadY >= 0) {
			roadY = -400;
		}
		SDL_UpdateWindowSurface(window);
		SDL_Delay(6);
	};
	void offroad(double& x, double& y, double& time_offroad, double delta, SDL_Surface* screen, SDL_Surface* kaboom, bool& pause, double& lifes, bool endlessLifeTime, double& lostLifeScore, double score, bool god_mode) {
		if (!((x > SCREEN_WIDTH / 4) && (x < 3 * SCREEN_WIDTH / 4))) {
			time_offroad += delta;
			if (time_offroad > 1) {
				if (!god_mode) {
					DrawSurface(screen, kaboom, x, y);
					pause = true;
				}
				if (time_offroad > 2) {
					x = SCREEN_WIDTH / 2;
					y = 400;
					deathChecker(lifes, endlessLifeTime, lostLifeScore, score, god_mode);
					pause = false;
				}
			}
		}
		else {
			time_offroad = 0;
		}
	};
	void spawn(struct carType* p, SDL_Surface* screen, SDL_Surface* car_bmp, struct carType* r, struct carType* s, int immortality_x) {
		if ((p->spawnTimer > p->time_limit) && (p->exist == false)) {
			p->car_x = (rand() % 280) + 180;
			if ((abs(p->car_x - r->car_x)>40) && (abs(p->car_x - s->car_x)>40) && (abs(p->car_x - immortality_x)>40)) {
				p->car_y = 0;
				p->exist = true;
				p->spawnTimer = 0.0;
			}
		}
		if (p->exist) {
			DrawSurface(screen, car_bmp, p->car_x, p->car_y);
			p->car_y++;
			if (p->car_y > SCREEN_HEIGHT + 60) {
				p->exist = false;
			}
		}
	};
	void pushing(double& x, double& y, struct carType* p) {
		//wjazd z lewej
		if (
			(((p->car_x - 22) - (x + 17)) < 0)
			&& (((y - 45) - (p->car_y + 45)) < 0)
			&& (((y + 45) - (p->car_y - 45)) > 0)
			&& (p->car_x > x)
			&& (p->exist == true)
			) {
			p->car_x += 15;
			x -= 10;
		}
		//wjazd z prawej
		else if (
			(((x - 17) - (p->car_x + 22)) < 0)
			&& (((y - 45) - (p->car_y + 45)) < 0)
			&& (((y + 45) - (p->car_y - 45)) > 0)
			&& (p->car_x < x)
			&& (p->exist == true)
			) {
			p->car_x -= 15;
			x += 10;
		}
	};
	void scoreSystem(double x, bool& pointsFreeze, double& score_timer, double& delta, double& score) {
		if ((x > SCREEN_WIDTH / 4) && (x < 3 * SCREEN_WIDTH / 4)) {
			if (pointsFreeze == false) {
				score_timer += delta;
				if (score_timer > 1) {
					score += 50;
					score_timer = 0;
				}
			}
		}
	};
	void r34_atack(double y, double x, struct carType* p) {
		if (((y - p->car_y) < 200) && (x != p->car_x) && (y + 45 > p->car_y) && ((x > SCREEN_WIDTH / 4) && (x < 3 * SCREEN_WIDTH / 4))) {
			if (x > p->car_x) {
				p->car_x += 1;
				p->car_y += 1;
			}
			if (x < p->car_x) {
				p->car_x -= 1;
				p->car_y += 1;
			}
		}
	};
	void supra_atack(double x, struct carType* p) {
		if ((x != p->car_x) && p->exist) {
			if (x > SCREEN_WIDTH / 2) {
				if (p->car_x < 3 * SCREEN_WIDTH / 4 - 20) {
					p->car_x++;
				}
			}
			if (x < SCREEN_WIDTH / 2) {
				if (p->car_x > SCREEN_WIDTH / 4 + 20) {
					p->car_x--;
				}
			}
		}
	};
	void fpsCount(double& fpsTimer, double delta, int& frames, double& fps) {
		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};
	};
	void header(SDL_Surface* screen, char* text, double fps, double score, double worldTime, SDL_Surface* charset, int czerwony, int niebieski, double licznik, double lifes) {
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 50, czerwony, niebieski);
		sprintf(text, "Patryk Miszke 193249  %.0lf klatek/s", fps);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
		sprintf(text, "Wynik= %.0lf  Czas trwania = %.1lf s ", score, worldTime);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
		if (licznik >= 0) {
			sprintf(text, "Licznik = %.1lf s", licznik);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 40, text, charset);
		}
		else {
			sprintf(text, "Pozostale zycia: %.0lf", lifes);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 40, text, charset);
		}
	};
	void loading_bmp(SDL_Surface*& charset, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer, SDL_Surface*& road, SDL_Surface*& car, SDL_Surface*& normal_bmp, SDL_Surface*& supra_bmp, SDL_Surface*& r34_bmp, SDL_Surface*& kaboom, SDL_Surface*& strzal, SDL_Surface*& grzib, SDL_Surface*& gameOver_bmp) {
		charset = SDL_LoadBMP("./cs8x8.bmp");
		if (charset == NULL) {
			printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};
		SDL_SetColorKey(charset, true, 0x000000);

		road = SDL_LoadBMP("./droga2.bmp");
		if (road == NULL) {
			printf("SDL_LoadBMP(droga2.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		car = SDL_LoadBMP("./autoszefa.bmp");
		if (car == NULL) {
			printf("SDL_LoadBMP(autoszefa.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		normal_bmp = SDL_LoadBMP("./normik.bmp");
		if (normal_bmp == NULL) {
			printf("SDL_LoadBMP(normik.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		supra_bmp = SDL_LoadBMP("./supra.bmp");
		if (supra_bmp == NULL) {
			printf("SDL_LoadBMP(supra.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		r34_bmp = SDL_LoadBMP("./paulwalker.bmp");
		if (r34_bmp == NULL) {
			printf("SDL_LoadBMP(paulwalker.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		kaboom = SDL_LoadBMP("./fire.bmp");
		if (kaboom == NULL) {
			printf("SDL_LoadBMP(fire.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		strzal = SDL_LoadBMP("./strzal.bmp");
		if (kaboom == NULL) {
			printf("SDL_LoadBMP(strzal.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		grzib = SDL_LoadBMP("./grzib.bmp");
		if (charset == NULL) {
			printf("SDL_LoadBMP(grzib.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};

		gameOver_bmp = SDL_LoadBMP("./gameover.bmp");
		if (charset == NULL) {
			printf("SDL_LoadBMP(gameover.bmp) error: %s\n", SDL_GetError());
			finish_program(charset, screen, scrtex, window, renderer);
		};
	};
	void events(SDL_Event& event, int& quit, bool& pointsFreeze, double& score_timer, double delta, double& score, int& roadY, struct carType* p, struct carType* r, struct carType* s, double& x, bool& pause, double& worldTime, double& etiSpeed, char* text, SDL_Surface* charset, SDL_Surface* screen, bool &gun, double& gun_x, double& gun_y, bool &new_game, bool &saving, bool &loading, int &cursor_y, double& lifes, double& licznik, char* tabBuff[]) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_UP) {
					if (pointsFreeze == false) {
						score_timer += delta;
						if (score_timer > 0.5) {
							score += 50;
							score_timer = 0;
						}
					}
					roadY += 10;
					p->car_y += 5;
					r->car_y += 5;
					s->car_y += 5;
					if ((loading == true) && (cursor_y > 120)){
						cursor_y -= 20;
					}
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					SDL_Delay(12); 
					if ((loading == true) && (cursor_y < 340)) {
							cursor_y += 20;
						}
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					if ((x < SCREEN_WIDTH - 27.5) && (pause == false))
						x += 25;
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					if ((x > 27.5) && (pause == false))
						x -= 25;
				}
				else if (event.key.keysym.sym == SDLK_n) {
					new_game = true;
				}
				else if ((event.key.keysym.sym == SDLK_p) && pause == false) {
					pause = true;
					sprintf(text, "Pauza");
					DrawString(screen, 250, 300, text, charset);

				}
				else if ((event.key.keysym.sym == SDLK_p) && pause == true) {
					pause = false;
				}
				else if ((event.key.keysym.sym == SDLK_SPACE) && pause == false) {
					if (gun == false) {
						gun = true;
						gun_x = x;
						gun_y = 330;
					}
				}
				else if (event.key.keysym.sym == SDLK_s) {
					saving = true;
				}
				else if (event.key.keysym.sym == SDLK_l) {
					loading = true;
				}
				else if (event.key.keysym.sym == SDLK_RETURN) {
					if (loading == true) {
						int wybranySave = cursor_y - 115;
						wybranySave /= 20;
						loadChosen(wybranySave, score, worldTime, lifes, licznik, tabBuff);
					}
				}
				break;
			case SDL_KEYUP:
				etiSpeed = 1.0;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
	};
	void carDestruction(struct carType* p, SDL_Surface* kaboom, double& score, bool& carHit, double& timeSinceHit, double& delta, bool& pointsFreeze, SDL_Surface* screen) {
		if ((!((p->car_x > SCREEN_WIDTH / 4) && (p->car_x < 3 * SCREEN_WIDTH / 4))) && (p->car_y > 0)) {
			DrawSurface(screen, kaboom, p->car_x, p->car_y);
			if (carHit == false) {
				if (pointsFreeze == false) {
					score += 300;
				}
				timeSinceHit = 0;
				carHit = true;
			}
			else {
				timeSinceHit += delta;
				if (timeSinceHit > 5) {
					carHit = false;
				}
			}
		}
	};
	void civilianDestruction(struct carType* p, SDL_Surface* kaboom, bool& pointsFreeze, SDL_Surface* screen, double& distance, double& dystansPoMorderstwie) {
		if ((!((p->car_x > SCREEN_WIDTH / 4) && (p->car_x < 3 * SCREEN_WIDTH / 4))) && (p->car_y > 0)) {
			DrawSurface(screen, kaboom, p->car_x, p->car_y);
			pointsFreeze = true;
			dystansPoMorderstwie += distance;
		}
		else if (dystansPoMorderstwie > 50) {
			pointsFreeze = false;
			dystansPoMorderstwie = 0;
		}
	};
	void collision(struct carType* p, double& x, double& y, SDL_Surface* kaboom, SDL_Surface* screen, bool& pause, double& timeSinceHit, double& delta, double& lifes, bool endlessLifeTime, double &lostLifeScore, double score, bool god_mode) {
		if (
			(p->car_x == x)
			&& ((y - 45) - (p->car_y + 45) < 0)
			&& (p->exist == true)
			&& (y > p->car_y)
			) {
			if (p->car_x == SCREEN_WIDTH / 2) {
				p->car_x + 10;
			}
			else if(!god_mode){
				DrawSurface(screen, kaboom, x, y);
				pause = true;
				timeSinceHit += delta;
				if (timeSinceHit > 2) {
					x = SCREEN_WIDTH / 2;
					y = 400;
					deathChecker(lifes, endlessLifeTime, lostLifeScore, score, god_mode);
					pause = false;
					timeSinceHit = 0;
				}
			}
		}if ((p->car_x == SCREEN_WIDTH/2) && (p->car_x == x)) {
			p->car_x += 10;
		}
	};
	void shotCivilian(bool& gun, SDL_Surface* screen, double& gun_y, double& gun_x, SDL_Surface* strzal, struct carType* p, bool& pointsFreeze, double delta, SDL_Surface* kaboom) {
		if (gun == true) {
			DrawSurface(screen, strzal, gun_x, gun_y);
			gun_y -= 2;
			if (
				(gun_x > (p->car_x - 22))
				&& (gun_x < (p->car_x + 22))
				&& ((gun_y - 15) < (p->car_y + 45))
				&& (p->car_y < gun_y)
				&& (p->car_y > 0)
				&& (p->exist == true)
				&& (p->shot != true)
				) {
				DrawSurface(screen, kaboom, p->car_x, p->car_y);
				SDL_Delay(20);
				p->exist = false;
				pointsFreeze = true;
				p->shot = true;
			}
			else if ((gun_y < 0) && (p->shot != true)) {
				gun = false;
			}
		}
		if (p->shot == true) {
			p->deathTimer += delta;
			if (p->deathTimer > 5) {
				pointsFreeze = false;
				p->deathTimer = 0;
				p->shot = false;
				gun = false;
			}
		}
	};
	void shotEnemy(bool& gun, SDL_Surface* screen, double& gun_y, double& gun_x, SDL_Surface* strzal, struct carType* p, double delta, SDL_Surface* kaboom, double& score) {
		if (gun == true) {
			DrawSurface(screen, strzal, gun_x, gun_y);
			gun_y -= 2;
			if (
				(gun_x > (p->car_x - 22))
				&& (gun_x < (p->car_x + 22))
				&& ((gun_y - 15) < (p->car_y + 45))
				&& (p->car_y < gun_y)
				&& (p->car_y > 0)
				&& (p->exist == true)
				&& (p->shot != true)
				) {
				DrawSurface(screen, kaboom, p->car_x, p->car_y);
				SDL_Delay(20);
				p->exist = false;
				p->shot = true;
			}
			else if ((gun_y < 0) && (p->shot != true)) {
				gun = false;
			}
		}
		if (p->shot == true) {
			p->deathTimer += delta;
			if (p->deathTimer > 1) {
				score += 300;
				p->deathTimer = 0;
				p->shot = false;
				gun = false;
			}
		}
	};
	void getLifes(double licznik, bool& endlessLifeTime, double& lifes, double& lostLifeScore, double score) {
		if (licznik < 0) {
			endlessLifeTime = false;
			if (lifes < 3 && score - lostLifeScore >= 5000) {
				lifes++;
				lostLifeScore = score;
			}
		}
	};
	void powerup(double& immortality_timer, bool& grzybeczek, int& immortality_x, int& immortality_y, double x, double y, SDL_Surface* screen, double& god_mode_timer, bool& god_mode, double delta, SDL_Surface* grzib, struct carType* p, struct carType* r, struct carType* s) {
		if ((immortality_timer > 60) && (grzybeczek == false)) {
			immortality_x = (rand() % 280) + 180;
			if ((abs(immortality_x - p->car_x) > 20) && (abs(immortality_x - r->car_x) > 20) && (abs(immortality_x - s->car_x) > 20)) {
				grzybeczek = true;
			}
		}
		if (grzybeczek == true) {
			DrawSurface(screen, grzib, immortality_x, immortality_y);
			immortality_y++;
			if (immortality_y > SCREEN_HEIGHT + 40) {
				immortality_y = 0;
				immortality_timer = 0;
				grzybeczek = false;
			}
			if (
				((y - 45) - (immortality_y + 20) < 0)
				&& (y + 45 >= immortality_y)
				&& (x - 35 < immortality_x)
				&& (x + 35 > immortality_x)) {
				god_mode_timer = 0;
				god_mode = true;
				grzybeczek = false;
				immortality_y = 0;
				immortality_timer = 0;

			}
		}
		if (god_mode == true) {
			god_mode_timer += delta;
			if (god_mode_timer > 10) {
				god_mode = false;
			}
		}
	};





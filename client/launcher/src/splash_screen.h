#pragma once

#define SPLASH_HEIGHT 512
#define SPLASH_WIDTH 512
#define SPLASH_PATH "resources/splash-screen.bmp"

#define PROGRESS_BAR_HEIGHT 10

#define PROGRESS_BAR_BG_RED 94
#define PROGRESS_BAR_BG_GREEN 33
#define PROGRESS_BAR_BG_BLUE 0

#define PROGRESS_BAR_FG_RED 220
#define PROGRESS_BAR_FG_GREEN 40
#define PROGRESS_BAR_FG_BLUE 0

#define SPLASH_SCREEN_HEIGHT (SPLASH_HEIGHT + PROGRESS_BAR_HEIGHT)
#define SPLASH_SCREEN_WIDTH SPLASH_WIDTH

namespace gtamp
{
namespace launcher
{
class splash_screen
{
public:
	static void show();
	static void close();

	static void set_progress(int progress);
	static int get_progress();

private:
	inline static bool _destroy = false;
	inline static int _progress = 0;

	static void ui_loop();
};
}; // namespace launcher
}; // namespace gtamp
#pragma once

#define SPLASH_HEIGHT 512
#define SPLASH_WIDTH 512
#define SPLASH_PATH "resources/splash-screen.bmp"

namespace gtamp
{
namespace launcher
{
class splash_screen
{
public:
	static void show();
	static void close();

private:
	inline static bool _destroy = false;

	static void ui_loop();
};
}; // namespace launcher
}; // namespace gtamp
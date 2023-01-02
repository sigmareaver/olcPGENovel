#pragma once

#include <assert.h>
#include <random>
#include <string>

#define URANGE(min, val, max)		(val) < (min) ? (min) : ((val) > (max) ? (max) : (val))

namespace Utils
{

	constexpr int KB_MOD_CTRL	= 1 << 0;
	constexpr int KB_MOD_SHIFT	= 1 << 1;

#if defined(OLC_PLATFORM_WINAPI)
	bool GetCapsLockState();
#elif defined(OLC_PLATFORM_X11)
	bool GetCapsLockState(X11::Display& olc_display);
#elif defined(OLC_PLATFORM_GLUT)
	bool GetCapsLockState();
#elif defined(OLC_PLATFORM_EMSCRIPTEN)
	bool GetCapsLockState();
#endif // GetCapsLockState()

	bool IsValidFilename(const char* filename);
	bool IsAlphanumeric(const char* string);
	const char* FlagString(const std::string& flags, const size_t size, const char* const* flag_names);
	int RandRange(int min, int max);
	double RandRange(double min, double max);
	//double rand_range_d(double min, double max);

#if 0 // not implemented
	template<size_t>
	class std::bitset;

	template<enum e, size_t size>
	class bitset : public std::bitset<size>
	{
	public:

	};
#endif

}

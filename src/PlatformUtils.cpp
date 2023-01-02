
#include <string>

#include <olcPixelGameEngine/olcPixelGameEngine.h>
#include "PlatformUtils.h"


namespace Utils
{

	// Platform implementations of getting capslock state.
#if defined(OLC_PLATFORM_WINAPI)
	bool GetCapsLockState()
	{
		return ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
	}
#elif defined(OLC_PLATFORM_X11)
	bool GetCapsLockState(X11::Display& olc_display)
	{
		unsigned int state = 0;
		X11::XkbGetIndicatorState(olc_display, XkbUseCoreKbd, &state);
		return ((state & 1u) != 0);
	}
#elif defined(OLC_PLATFORM_GLUT)
	bool GetCapsLockState()
	{
		static_assert(false, "Not implemented on GLUT.");
		return false;
	}
#elif defined(OLC_PLATFORM_EMSCRIPTEN)
	bool GetCapsLockState()
	{
//		EM_JS(bool, jsGetCapsLockState, (), { return event.getModifierState("CapsLock"); });
//		return jsGetCapsLockState();
		return EM_ASM_BOOL({ return event.getModifierState("CapsLock"); });
	}
#endif // GetCapsLockState()

	// For now, valid filenames will be alphanumeric and - . _ ~
	bool IsValidFilename(const char* filename)
	{
		bool contains_alphanum = false;

		if (!filename)
			return false;

		if (filename[strlen(filename)-1] == '.')
			return false;

		for (int i = strlen(filename) - 1; i >= 0; --i)
		{
			const char c = filename[i];
			if (c >= 'A' && c <= 'Z')
			{
				contains_alphanum = true;
				continue;
			}
			else if (c >= 'a' && c <= 'z')
			{
				contains_alphanum = true;
				continue;
			}
			else if (c >= '0' && c <= '9')
			{
				contains_alphanum = true;
				continue;
			}
			else if (c == '-' || c == '.' || c == '_' || c == '~')
				continue;
			else
				return false;
		}

		if (!contains_alphanum)
			return false;

		return true;
	}

	bool IsAlphanumeric(const char* string)
	{
		if (!string)
			return false;
		for (int i = strlen(string) - 1; i >= 0; --i)
		{
			const char c = string[i];
			if (c >= 'A' && c <= 'Z')
				return true;
			if (c >= 'a' && c <= 'z')
				return true;
			if (c >= '0' && c <= '9')
				return true;
		}
		return false;
	}

	const char* FlagString(const std::string& flags, const size_t size, const char* const* flag_names)
	{
		static std::string buf;
		size_t i = 0;

		buf.clear();
		for (auto it = flags.begin(); it != flags.end() && i < size; it++, i++)
		{
			if ((*it) == '1')
			{
				if (buf.length() > 0)
					buf.push_back(' ');
				buf.append(flag_names[i]);
			}
		}

		return buf.c_str();
	}

	int RandRange(int min, int max)
	{
		//assert(min > max);
		static std::random_device rd;
		static std::mt19937 rng(rd()); // Mersenne-Twister
		std::uniform_int_distribution<int> uni(min, max);
		return uni(rng);
	}

	double RandRange(double min, double max)
	{
		//assert(min > max);
		static std::random_device rd;
		static std::mt19937 rng(rd()); // Mersenne-Twister
		std::uniform_real_distribution<double> uni(min, max);
		return uni(rng);
	}

}

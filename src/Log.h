
#pragma once

#include <string>
#include <format>
#include <iostream>

#include "Event.h"


#define MAX_STRING_LENGTH		2024
#define MSL						MAX_STRING_LENGTH
#define MAX_INPUT_LENGTH		1024
#define MIL						MAX_INPUT_LENGTH

#define LOG(msg, ...)			Logging::Log(Logging::Channel::Debug, msg, __VA_ARGS__)
#define WARN(msg, ...)			Logging::Log(Logging::Channel::Warning, msg, __VA_ARGS__)
#define ERR(msg, ...)			Logging::Log(Logging::Channel::Error, msg, __VA_ARGS__)

namespace Logging
{
	enum class Channel
	{
		/*Local, Global, Tell, OOC,*/ Debug, Warning, Error
	};

	extern const char* const ChannelString[];

	template<typename... Args>
	inline void Log(const Channel channel, const std::string& fmt, const Args... args)
	{
		std::string buf = std::format("{} {}", ChannelString[static_cast<size_t>(channel)], std::format(fmt, args...));
		Event::Event* event = new Event::Event(Event::Type::LOG);
		event->AddParam(&channel, sizeof(Channel));
		event->AddParam(buf.c_str(), strlen(buf.c_str()) + 1);

		switch (channel)
		{
		default:
		//case Channel::Local:
		//case Channel::Global:
		//case Channel::Tell:
		//case Channel::OOC:
		//	Event::Handler::GetInstance() << event;
		//	break;
		case Channel::Debug:
		case Channel::Warning:
		case Channel::Error:
#ifdef _DEBUG
			Event::Handler::GetInstance() << event;
#endif
			std::cerr << buf << std::endl;
			break;
		}
	}

}

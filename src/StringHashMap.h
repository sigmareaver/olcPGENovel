#pragma once

#include "Log.h"

#define STR_ALLOC(x)	StringHashMap::str_alloc((x))
#define STR_FREE(x)		{if ((x) != nullptr && StringHashMap::str_free((x)) != -1)		\
							(x) = nullptr;}
#define QUICK_LINK(x)	StringHashMap::quick_link((x))


namespace StringHashMap
{
	char* str_alloc(const char* str);
	char* quick_link(char* str);
	int str_free(char* str);
	void show_hash(int count);
	void bucket_dump(int bucket);
	void check_hash(const char* str);
	void hash_stats();
	bool in_hash_table(const char * str);

	typedef unsigned int HASH;
	HASH GetStringFastHash(const char* buf);
}


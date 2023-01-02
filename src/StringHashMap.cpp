
#include <string.h>
#include <iostream>

#include "StringHashMap.h"
#include "Log.h"

namespace StringHashMap
{
	using namespace Logging;

	static constexpr size_t STR_HASH_SIZE = 1024;
	typedef unsigned int HASH;

	struct StringHashData
	{
		StringHashData* next;
		unsigned short refs;
		unsigned short length;
		HASH hash;
	};


	StringHashData* g_string_hash_map[STR_HASH_SIZE];


	HASH GetStringFastHash(const char* buf)
	{
		HASH hash;
		unsigned char c;

		if (buf == NULL)
			return 0;

		hash = 0;
		c = *buf++;

		if (c)
		{
			do
			{
				if (c >= 'a' && c <= 'z')
					c &= (unsigned char)0b11011111; // ~0x20
				hash = (hash << 4) + c;
				HASH x = hash & 0xF0000000L;
				if (x)
				{
					hash ^= (x >> 24);
					hash &= ~x;
				}
				c = *buf++;
			} while (c);
		}
		return hash;
	}

	char* str_alloc(const char* str)
	{
		int len, bucket, psize;
		StringHashData* ptr;
		HASH hash;

		len = strlen(str);
		psize = sizeof(StringHashData);
		bucket = len % STR_HASH_SIZE;
		hash = GetStringFastHash(str);
		for (ptr = g_string_hash_map[bucket]; ptr; ptr = ptr->next)
		{
			if (len == ptr->length && hash == ptr->hash)
			{
				if (ptr->refs < 65535)
					++ptr->refs;
				return reinterpret_cast<char*>(ptr) + psize;
			}
		}
		ptr = reinterpret_cast<StringHashData*>(new char[len + psize + 1]);
		ptr->refs = 1;
		ptr->length = len;
		ptr->hash = hash;
		if (len)
			strcpy_s((reinterpret_cast<char*>(ptr) + psize), len+1, str);
		else
			strcpy_s((reinterpret_cast<char*>(ptr) + psize), 1, "");
		ptr->next = g_string_hash_map[bucket];
		g_string_hash_map[bucket] = ptr;

		return reinterpret_cast<char*>(ptr) + psize;
	}

	char* quick_link(char* str)
	{
		StringHashData* ptr;

		ptr = reinterpret_cast<StringHashData*>(str - sizeof(StringHashData));
		if (ptr->refs == 0)
		{
			Log(Channel::Warning, "quick_link(): bad pointer");
			return nullptr;
		}
		if (ptr->refs < 65535)
			++ptr->refs;
		else
			Log(Channel::Warning, "quick_link(): max ref count reached");
		return str;
	}

	int str_free(char* str)
	{
		int len, bucket;
		struct StringHashData* ptr, * ptr2, * ptr2_next;

		len = strlen(str);
		bucket = len % STR_HASH_SIZE;
		ptr = reinterpret_cast<StringHashData*>(str - sizeof(StringHashData));
		
		if (ptr->refs == 65535)  /* permanent */
			return ptr->refs;
		if (ptr->refs == 0)
		{
			Log(Channel::Warning, "str_free(): bad pointer");
			return -1;
		}
		if (--ptr->refs == 0)
		{
			if (g_string_hash_map[bucket] == ptr)
			{
				g_string_hash_map[bucket] = ptr->next;
				delete reinterpret_cast<char*>(ptr);
				return 0;
			}
			for (ptr2 = g_string_hash_map[bucket]; ptr2; ptr2 = ptr2_next)
			{
				ptr2_next = ptr2->next;
				if (ptr2_next == ptr)
				{
					ptr2->next = ptr->next;
					delete reinterpret_cast<char*>(ptr);
					return 0;
				}
			}
			Log(Channel::Warning, "str_free(); pointer not found for string: {}", str);
			return -1;
		}
		return ptr->refs;
	}

	void show_hash(int count)
	{
		struct StringHashData* ptr;
		int x, c;

		for (x = 0; x < count; x++)
		{
			for (c = 0, ptr = g_string_hash_map[x]; ptr; ptr = ptr->next, c++);
			std::cout << " " << c;
		}
		std::cout << std::endl;
	}

	void bucket_dump(int bucket)
	{
		StringHashData* ptr;
		char* str;
		int c, psize;

		if (bucket > STR_HASH_SIZE || bucket < 0)
		{
			Log(Channel::Error, "hash_dump(): invalid bucket");
			return;
		}
		psize = sizeof(StringHashData);
		for (c = 0, ptr = g_string_hash_map[bucket]; ptr; ptr = ptr->next, c++)
		{
			//str = (char*)(((long)ptr) + psize);
			str = reinterpret_cast<char*>(ptr) + psize;
			Log(Channel::Debug, "Len:{:<4d} Lnks:{:<5d} Str: {}", ptr->length, ptr->refs, str);
		}
		if (c)
			Log(Channel::Debug, "Total strings in bucket {:d}: {:d}", bucket, c);
	}

	void check_hash(const char* str)
	{
		int len, bucket, psize, p = 0, c;
		struct StringHashData* ptr, * fnd;
		HASH hash;

		len = strlen(str);
		psize = sizeof(StringHashData);
		bucket = len % STR_HASH_SIZE;
		hash = GetStringFastHash(str);
		for (fnd = NULL, ptr = g_string_hash_map[bucket], c = 0; ptr; ptr = ptr->next, c++)
		{
			if (len == ptr->length && hash == ptr->hash)
			{
				fnd = ptr;
				p = c + 1;
			}
		}
		if (fnd)
			Log(Channel::Debug, "Hashmap info on string: {}\r\nLinks: {:d}  Position: {:d}/{:d}  Bucket: {:d}  Length: {:d}  Hash: {:d}",
				str, fnd->refs, p, c, bucket, fnd->length, fnd->hash);
		else
			Log(Channel::Warning, "'{}' not found.", str);
	}

	void hash_stats()
	{
		struct StringHashData* ptr;
		int x, c, total, totlinks, unique, bytesused, wouldhave, hilink;

		totlinks = unique = total = bytesused = wouldhave = hilink = 0;
		for (x = 0; x < STR_HASH_SIZE; x++)
		{
			for (c = 0, ptr = g_string_hash_map[x]; ptr; ptr = ptr->next, c++)
			{
				total++;
				if (ptr->refs == 1)
					unique++;
				if (ptr->refs > hilink)
					hilink = ptr->refs;
				totlinks += ptr->refs;
				bytesused += (ptr->length + 1 + sizeof(StringHashData));
				wouldhave += ((ptr->refs * sizeof(StringHashData)) + (ptr->refs * (ptr->length + 1)));
			}
		}
		Log(Channel::Debug, "Hash strings allocated:{:<8d}  Total links: {:d}\r\nString bytes allocated:{:<8d}  Bytes saved: {:d}\r\nUnique (wasted) links: {:<8d}  Hi-Link count: {:d}",
			total, totlinks, bytesused, wouldhave - bytesused, unique, hilink);
	}

	void show_high_hash(int top)
	{
		struct StringHashData* ptr;
		int x, psize;
		char* str;

		psize = sizeof(struct StringHashData);
		for (x = 0; x < STR_HASH_SIZE; x++)
			for (ptr = g_string_hash_map[x]; ptr; ptr = ptr->next)
				if (ptr->refs >= top)
				{
					str = (char*)(((long)ptr) + psize);
					Log(Channel::Debug, "Links: {:<5d}  String: {}", ptr->refs, str);
				}
	}

	bool in_hash_table(const char* str)
	{
		int len, bucket, psize;
		StringHashData* ptr;

		len = strlen(str);
		psize = sizeof(StringHashData);
		bucket = len % STR_HASH_SIZE;
		for (ptr = g_string_hash_map[bucket]; ptr; ptr = ptr->next)
			if (len == ptr->length && str == ((char*)ptr + psize))
				return true;
		return false;
	}

}

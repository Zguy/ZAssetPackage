/*The MIT License (MIT)

Copyright (c) 2014 Johannes Häggqvist

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/
#include "path.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

namespace cli
{
	void cleanPath(std::string &path)
	{
		for (std::string::iterator it = path.begin(); it != path.end(); ++it)
		{
			if ((*it) == '\\')
				(*it) = '/';
		}

		if (path.back() == '*')
			path.pop_back();
		if (path.back() == '/')
			path.pop_back();
	}

	bool isDirectory(const std::string &path)
	{
#ifdef _WIN32
		DWORD attrib = GetFileAttributesA(path.c_str());
		return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
#else
		struct stat sb;
		return (stat(path.c_str(), &sb) == 0 && (S_ISDIR(sb.st_mode)));
#endif
	}

	bool createPath(const std::string &path)
	{
		std::string::size_type end = path.find_last_of('/')+1;
		for (std::string::size_type index = path.find_first_of('/', 0); index < end; index = path.find_first_of('/', index+1))
		{
			std::string part = path.substr(0, index);
			if (!isDirectory(part))
			{
#ifdef _WIN32
				if (CreateDirectoryA(part.c_str(), nullptr) == 0)
					return false;
#else
				if (mkdir(part.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
					return false;
#endif
			}
		}

		return true;
	}
}

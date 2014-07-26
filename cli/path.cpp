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

#include <sys/stat.h>
#include <direct.h>

namespace cli
{
	void cleanPath(std::string &path)
	{
		for (std::string::iterator it = path.begin(); it != path.end(); ++it)
		{
			if ((*it) == '\\')
				(*it) = '/';
		}
	}

	bool isDirectory(const std::string &path)
	{
		struct stat sb;
		return (stat(path.c_str(), &sb) == 0 && (sb.st_mode & _S_IFMT) == _S_IFDIR);
	}

	bool createPath(const std::string &path)
	{
		std::string::size_type end = path.find_last_of('/')+1;
		for (std::string::size_type index = path.find_first_of('/', 0); index < end; index = path.find_first_of('/', index+1))
		{
			std::string part = path.substr(0, index);
			if (!isDirectory(part))
			{
				if (_mkdir(part.c_str()) != 0)
					return false;
			}
		}

		return true;
	}
}

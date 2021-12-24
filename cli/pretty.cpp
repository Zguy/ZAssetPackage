/*The MIT License (MIT)

Copyright (c) 2015 Johannes Häggqvist

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
#include "pretty.h"

#include <sstream>
#include <iomanip>

namespace cli
{
	namespace
	{
		template<typename T>
		inline std::string toString(const T &t, int precision = 10)
		{
			std::stringstream ss;
			ss << std::fixed << std::setprecision(precision) << t;
			return ss.str();
		}
	}

	std::string getPrettyVersion(ZAP::Version version)
	{
		switch (version)
		{
		case ZAP::Version::V1_0: return "1.0";
		default: return "Unknown";
		}
	}
	std::string getPrettyCompression(ZAP::Compression compression)
	{
		switch (compression)
		{
		case ZAP::Compression::NONE: return "None";
		case ZAP::Compression::LZ4:  return "LZ4";
		default: return "Unknown";
		}
	}
	std::string getPrettySize(std::uint32_t size)
	{
		static const char *suffixes[] = { " B", " KiB", " MiB", " GiB", " TiB" };
		static const int suffixesSize = 5;

		int suffIndex = 0;
		double newSize = size;

		while ((newSize >= 1024.0) && (suffIndex < suffixesSize - 1))
		{
			++suffIndex;
			newSize /= 1024.0;
		}

		return toString(newSize, 2) + suffixes[suffIndex];
	}
}

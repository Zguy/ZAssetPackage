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
#include "pack.h"
#include "path.h"

#include <ZAP/ArchiveBuilder.h>

#include <string>
#include <iostream>

#include "options.h"

namespace cli
{
	namespace
	{
		void addDirectory(ZAP::ArchiveBuilder &archive, const std::string &path, bool recursive)
		{

		}
		void addPath(ZAP::ArchiveBuilder &archive, const std::string &path, bool recursive)
		{
			if (isDirectory(path))
			{
				if (recursive)
					addDirectory(archive, path, recursive);
			}
			else
			{
				archive.addFile(path, path);
			}
		}
	}

	int pack(option::Parser &parse, option::Option *options)
	{
		std::string outPath = "./archive.zap";
		if (options[PACK].arg != nullptr)
			outPath = options[PACK].arg;

		bool recursive = (options[RECURSIVE] != nullptr);

		ZAP::ArchiveBuilder archive;

		for (int i = 0; i < parse.nonOptionsCount(); ++i)
		{
			std::string path = parse.nonOption(i);
			cleanPath(path);
			addPath(archive, path, recursive);
		}

		ZAP::Compression compression = ZAP::COMPRESS_NONE;
		if (options[COMPRESS].arg != nullptr)
			compression = static_cast<ZAP::Compression>(std::atoi(options[COMPRESS].arg));

		if (!archive.buildFile(outPath, compression))
		{
			std::cerr << "Could not build archive" << std::endl;
			return 1;
		}

		return 0;
	}
}

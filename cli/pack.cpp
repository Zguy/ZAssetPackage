/*The MIT License (MIT)

Copyright (c) 2021 Johannes Häggqvist

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
#include "options.h"

#include <ZAP/ArchiveBuilder.h>

#include <string>
#include <iostream>
#include <cstring>

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <dirent.h>
#endif

namespace cli
{
	namespace
	{
		void addPath(ZAP::ArchiveBuilder &archive, const std::string &path)
		{
			archive.addFile(path, path);
		}
		void addDirectory(ZAP::ArchiveBuilder &archive, const std::string &path, bool recursive)
		{
		#ifdef _WIN32
			HANDLE dir;
			WIN32_FIND_DATAA ent;

			if ((dir = FindFirstFileA((path + "/*").c_str(), &ent)) != INVALID_HANDLE_VALUE)
			{
				do
				{
					if ((std::strcmp(ent.cFileName, ".") == 0) || (std::strcmp(ent.cFileName, "..") == 0))
						continue;

					if (ent.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (recursive)
							addDirectory(archive, path + '/' + ent.cFileName, recursive);
					}
					else
					{
						addPath(archive, path + '/' + ent.cFileName);
					}
				} while (FindNextFileA(dir, &ent));

				FindClose(dir);
			}
		#else
			DIR *dir;
			struct dirent *ent;

			if ((dir = opendir(path.c_str())) != nullptr)
			{
				while ((ent = readdir(dir)) != nullptr)
				{
					if ((std::strcmp(ent->d_name, ".") == 0) || (std::strcmp(ent->d_name, "..") == 0))
						continue;

					if (ent->d_type == DT_DIR)
					{
						if (recursive)
							addDirectory(archive, path + '/' + ent->d_name, recursive);
					}
					else
					{
						addPath(archive, path + '/' + ent->d_name);
					}
				}

				closedir(dir);
			}
		#endif
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

			if (isDirectory(path))
				addDirectory(archive, path, recursive);
			else
				addPath(archive, path);
		}

		ZAP::Compression compression = ZAP::Compression::NONE;
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

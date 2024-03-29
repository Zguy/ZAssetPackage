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
#include "extract.h"
#include "pretty.h"
#include "options.h"
#include "path.h"

#include <ZAP/Archive.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

namespace cli
{
	static void print(const ZAP::Archive &archive)
	{
		std::cout <<
			"Version: " << getPrettyVersion(archive.getVersion()) <<
			"\nCompression: " << getPrettyCompression(archive.getCompression()) << (archive.isSupportedCompression() ? " (supported)" : " (unsupported)") <<
			"\nFile count: " << archive.getFileCount() <<
			"\n\n";

		ZAP::Archive::EntryList filelist;
		archive.getFileList(filelist);

		static const int fieldMargin = 1;
		int field0 = 4+fieldMargin, field1 = 10+fieldMargin, field2 = 12+fieldMargin;
		for (const ZAP::Archive::Entry *entry : filelist)
		{
			int newField0 = static_cast<int>(entry->virtual_path.size())+fieldMargin;
			int newField1 = static_cast<int>(getPrettySize(entry->compressed_size).size())+fieldMargin;
			int newField2 = static_cast<int>(getPrettySize(entry->decompressed_size).size())+fieldMargin;
			if (newField0 > field0)
				field0 = newField0;
			if (newField1 > field1)
				field1 = newField1;
			if (newField2 > field2)
				field2 = newField2;
		}

		int totalComp = 0, totalDecomp = 0;

		std::ios::fmtflags nameFlags = std::ios::left;
		std::ios::fmtflags sizeFlags = std::ios::right;

		std::cout <<
			std::setiosflags(nameFlags) <<
			std::setw(field0) << "Path" <<
			std::setiosflags(sizeFlags) <<
			std::setw(field1) << "Comp. size" <<
			std::setw(field2) << "Decomp. size" <<
			std::resetiosflags(sizeFlags) <<
			'\n';
		for (const ZAP::Archive::Entry *entry : filelist)
		{
			totalComp += entry->compressed_size;
			totalDecomp += entry->decompressed_size;
			std::cout <<
				std::setiosflags(nameFlags) <<
				std::setw(field0) << entry->virtual_path <<
				std::setiosflags(sizeFlags) <<
				std::setw(field1) << getPrettySize(entry->compressed_size) <<
				std::setw(field2) << getPrettySize(entry->decompressed_size) <<
				std::resetiosflags(sizeFlags) <<
				'\n';
		}

		std::cout <<
			std::setw(16) << "\nTotal comp.: " << getPrettySize(totalComp) <<
			std::setw(16) << "\nTotal decomp.: " << getPrettySize(totalDecomp) <<
			'\n';

		std::cout << std::flush;
	}

	int extract(option::Parser &parse, option::Option *options)
	{
		if (parse.nonOptionsCount() < 1)
		{
			std::cerr << "Filename required" << std::endl;
			return 1;
		}
		std::string path = parse.nonOption(0);

		ZAP::Archive archive;
		if (!archive.openFile(path))
		{
			std::cerr << "Could not open archive" << std::endl;
			return 1;
		}

		if (options[LIST])
		{
			print(archive);
		}
		else
		{
			std::string outPath = ".";
			if (parse.nonOptionsCount() >= 2)
			{
				outPath = parse.nonOption(1);
			}
			if (!isDirectory(outPath))
			{
				std::cerr << "Output is not a directory" << std::endl;
				return 1;
			}

			ZAP::Archive::EntryList list;
			archive.getFileList(list);

			for (const ZAP::Archive::Entry *entry : list)
			{
				std::string fullpath = (outPath + '/' + entry->virtual_path);
				cleanPath(fullpath);
				if (!createPath(fullpath))
				{
					std::cerr << "Could not create path " << fullpath << std::endl;
					continue;
				}

				std::fstream stream(fullpath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
				if (!stream.is_open())
				{
					std::cerr << "Could not create entry " << entry->virtual_path << std::endl;
					continue;
				}

				char *data;
				size_t size;

				bool extractSuccess = false;
				if (options[RAW])
					extractSuccess = archive.getRawData(entry, data, size);
				else
					extractSuccess = archive.getData(entry, data, size);

				if (!extractSuccess)
				{
					std::cerr << "Could not extract entry " << entry->virtual_path << std::endl;
					continue;
				}

				stream.write(data, size);

				delete[] data;
			}
		}

		return 0;
	}
}

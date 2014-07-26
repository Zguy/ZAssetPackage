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
#include "extract.h"
#include "pretty.h"
#include "options.h"

#include <ZAP/Archive.h>

#include <iostream>
#include <iomanip>

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
		for (const ZAP::Archive::Entry &entry : filelist)
		{
			int newField0 = entry.virtual_path.size()+fieldMargin;
			int newField1 = getPrettySize(entry.compressed_size).size()+fieldMargin;
			int newField2 = getPrettySize(entry.decompressed_size).size()+fieldMargin;
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
		for (const ZAP::Archive::Entry &file : filelist)
		{
			totalComp += file.compressed_size;
			totalDecomp += file.decompressed_size;
			std::cout <<
				std::setiosflags(nameFlags) <<
				std::setw(field0) << file.virtual_path <<
				std::setiosflags(sizeFlags) <<
				std::setw(field1) << getPrettySize(file.compressed_size) <<
				std::setw(field2) << getPrettySize(file.decompressed_size) <<
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
			std::cerr << "Could not open file" << std::endl;
			return 1;
		}

		if (options[PRINT])
		{
			print(archive);
		}
		else
		{

		}

		return 0;
	}
}

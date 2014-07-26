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

#include "ZAP/Archive.h"
#include "ZAP/ArchiveBuilder.h"

#include "optionparser.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

enum optionIndex
{
	UNKNOWN,
	HELP,
	PRINT,
	EXTRACT,
	PACK,
	COMPRESS
};

option::ArgStatus checkCompress(const option::Option &option, bool msg)
{
	ZAP::Compression compression = static_cast<ZAP::Compression>(std::atoi(option.arg));
	if (!ZAP::supportsCompression(compression))
		return option::ARG_ILLEGAL;
	else
		return option::ARG_OK;
}

const option::Descriptor usage[] =
{
	{ HELP,     0, "h", "help",     option::Arg::None, "--help, -h  \tPrint usage and exit" },
	{ PRINT,    0, "p", "print",    option::Arg::None, "--print, -p  \tPrint contents of archive." },
	{ COMPRESS, 0, "c", "compress", checkCompress,     "--compress, -c  \tSet compression for pack." },
	{0,0,0,0,0,0}
};

int extract(option::Parser &parse, option::Option *options);
int pack(option::Parser &parse, option::Option *options);

int main(int argc, char *argv[])
{
	argc-=(argc>0); argv+=(argc>0); // Skip program name
	option::Stats stats(true, usage, argc, argv);
	option::Option *options = new option::Option[stats.options_max];
	option::Option *buffer = new option::Option[stats.buffer_max];
	option::Parser parse(true, usage, argc, argv, options, buffer);

	if (parse.error())
	{
		std::cout << "Invalid options\n";
		option::printUsage(std::cout, usage);
		return 1;
	}

	if (options[HELP] || argc == 0 || (options[UNKNOWN].count() == parse.optionsCount()))
	{
		option::printUsage(std::cout, usage);
		return 0;
	}

	if (options[PRINT] || options[EXTRACT])
	{
		return extract(parse, options);
	}
	else if (options[PACK])
	{
		return pack(parse, options);
	}

	delete[] options;
	delete[] buffer;
	return 0;
}

template<typename T>
inline std::string toString(const T &t, int precision = 10)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << t;
	return ss.str();
}

std::string getPrettyVersion(ZAP::Version version)
{
	switch (version)
	{
	case ZAP::VERSION_1_0: return "1.0";
	default: return "Unknown";
	}
}
std::string getPrettyCompression(ZAP::Compression compression)
{
	switch (compression)
	{
	case ZAP::COMPRESS_NONE: return "None";
	case ZAP::COMPRESS_LZ4:  return "LZ4";
	default: return "Unknown";
	}
}
std::string getPrettySize(std::uint32_t size)
{
	static const char *suffixes[] = { " B", " KB", " MB", " GB", " TB" };
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

void print(const ZAP::Archive &archive)
{
	std::cout <<
		"Version: " << getPrettyVersion(archive.getVersion()) <<
		"\nCompression: " << getPrettyCompression(archive.getCompression()) <<
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
		std::cout << "Filename required" << std::endl;
		return 1;
	}
	std::string path = parse.nonOption(0);

	ZAP::Archive archive;
	if (!archive.openFile(path))
	{
		std::cout << "Could not open file" << std::endl;
		return 1;
	}

	if (options[PRINT])
	{
		print(archive);
	}
	else if (options[EXTRACT])
	{

	}

	return 0;
}

int pack(option::Parser &parse, option::Option *options)
{
	return 0;
}

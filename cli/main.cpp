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
#include "pack.h"

#include <ZAP/Archive.h>
#include <ZAP/ArchiveBuilder.h>

#include "options.h"

#include <iostream>
#include <memory>

option::ArgStatus checkPack(const option::Option &option, bool msg)
{
	if (option.arg != nullptr)
		return option::ARG_OK;
	else
		return option::ARG_IGNORE;
}

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
	{ cli::HELP,      0, "h", "help",      option::Arg::None, "--help, -h  \tPrint usage and exit" },
	{ cli::LIST,      0, "l", "list",      option::Arg::None, "--list, -p  \tPrint contents of archive." },
	{ cli::EXTRACT,   0, "e", "extract",   option::Arg::None, "--extract, -e  \tExtract contents of archive to directory." },
	{ cli::PACK,      0, "p", "pack",      checkPack,         "--pack, -p [output.zap]  \tPack files into archive." },
	{ cli::COMPRESS,  0, "c", "compress",  checkCompress,     "--compress, -c  \tSet compression for pack." },
	{ cli::RECURSIVE, 0, "r", "recursive", option::Arg::None, "--recursive, -r  \tRecursively add files to the archive." },
	{ cli::RAW,       0, "", "raw",        option::Arg::None, "--raw  \tExtract raw data (compressed)." },
	{0,0,0,0,0,0}
};

int main(int argc, char *argv[])
{
	argc-=(argc>0); argv+=(argc>0); // Skip program name
	option::Stats stats(true, usage, argc, argv);
	std::unique_ptr<option::Option[]> options(new option::Option[stats.options_max]);
	std::unique_ptr<option::Option[]> buffer(new option::Option[stats.buffer_max]);
	option::Parser parse(true, usage, argc, argv, options.get(), buffer.get());

	if (parse.error() || (options[cli::UNKNOWN].count() == parse.optionsCount()))
	{
		std::cerr << "Invalid options\n";
		option::printUsage(std::cout, usage);
		return 1;
	}

	if (options[cli::HELP] || argc == 0)
	{
		option::printUsage(std::cout, usage);
		return 0;
	}

	if (options[cli::LIST] || options[cli::EXTRACT])
	{
		return cli::extract(parse, options.get());
	}
	else if (options[cli::PACK])
	{
		return cli::pack(parse, options.get());
	}

	return 0;
}

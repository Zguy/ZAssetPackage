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
	{ cli::HELP,     0, "h", "help",     option::Arg::None, "--help, -h  \tPrint usage and exit" },
	{ cli::PRINT,    0, "p", "print",    option::Arg::None, "--print, -p  \tPrint contents of archive." },
	{ cli::COMPRESS, 0, "c", "compress", checkCompress,     "--compress, -c  \tSet compression for pack." },
	{0,0,0,0,0,0}
};

int main(int argc, char *argv[])
{
	argc-=(argc>0); argv+=(argc>0); // Skip program name
	option::Stats stats(true, usage, argc, argv);
	option::Option *options = new option::Option[stats.options_max];
	option::Option *buffer = new option::Option[stats.buffer_max];
	option::Parser parse(true, usage, argc, argv, options, buffer);

	if (parse.error())
	{
		std::cerr << "Invalid options\n";
		option::printUsage(std::cout, usage);
		return 1;
	}

	if (options[cli::HELP] || argc == 0 || (options[cli::UNKNOWN].count() == parse.optionsCount()))
	{
		option::printUsage(std::cout, usage);
		return 0;
	}

	if (options[cli::PRINT] || options[cli::EXTRACT])
	{
		return cli::extract(parse, options);
	}
	else if (options[cli::PACK])
	{
		return cli::pack(parse, options);
	}

	delete[] options;
	delete[] buffer;
	return 0;
}

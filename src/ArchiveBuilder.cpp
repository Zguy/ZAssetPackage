/*The MIT License (MIT)

Copyright (c) 2013 Johannes Häggqvist

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
#include <ZAP/ArchiveBuilder.h>
#include <ZAP/Version.h>

#include <cstdint>
#include <fstream>

namespace ZAP
{
	ArchiveBuilder::ArchiveBuilder()
	{
	}
	ArchiveBuilder::~ArchiveBuilder()
	{
	}

	bool ArchiveBuilder::addFile(const std::string &real_path, const std::string &virtual_path)
	{
		Entry entry;
		entry.real_path    = real_path;
		entry.virtual_path = virtual_path;

		return files.insert(entry).second;
	}

	bool ArchiveBuilder::removeFile(const std::string &virtual_path)
	{
		for (FileList::iterator it = files.begin(); it != files.end(); ++it)
		{
			const Entry &entry = (*it);
			if (entry.virtual_path == virtual_path)
			{
				files.erase(it);
				return true;
			}
		}
		return false;
	}

	void ArchiveBuilder::clearFiles()
	{
		files.clear();
	}

	std::size_t ArchiveBuilder::getFileCount() const
	{
		return files.size();
	}

	void ArchiveBuilder::getFileMap(std::map<std::string,std::string> &map)
	{
		map.clear();

		for (FileList::const_iterator it = files.cbegin(); it != files.cend(); ++it)
		{
			const Entry &entry = (*it);
			map.insert(std::make_pair(entry.virtual_path, entry.real_path));
		}
	}

	bool ArchiveBuilder::build(const std::string &filename, Compression compression)
	{
		std::ofstream stream(filename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (!stream.is_open())
		{
			return false;
		}

		if (compression < COMPRESS_NONE || compression >= COMPRESS_LAST)
		{
			return false;
		}

		static const std::uint16_t MAGIC  = 'AZ';
		static const std::uint8_t VERSION = static_cast<std::uint8_t>(VERSION_CUR);
		const std::uint8_t COMPRESSION = static_cast<std::uint8_t>(compression);

		// Header
		stream.write(reinterpret_cast<const char*>(&MAGIC), sizeof(std::uint16_t)); // Magic
		stream.write(reinterpret_cast<const char*>(&VERSION), sizeof(std::uint8_t)); // Version
		stream.write(reinterpret_cast<const char*>(&COMPRESSION), sizeof(std::uint8_t)); // Compression

		// Build lookup table
		std::uint32_t tableSize = files.size();
		stream.write(reinterpret_cast<const char*>(&tableSize), sizeof(std::uint32_t)); // Table size

		std::uint32_t *fillIn = new std::uint32_t[tableSize];
		std::uint32_t *currFillIn = &fillIn[0];
		for (FileList::const_iterator it = files.cbegin(); it != files.cend(); ++it)
		{
			const Entry &entry = (*it);

			stream.write(entry.virtual_path.c_str(), sizeof(char)*entry.virtual_path.size()+1);

			// We don't know these values yet
			(*currFillIn) = static_cast<std::uint32_t>(stream.tellp());
			static const std::uint32_t ZERO = 0;
			stream.write(reinterpret_cast<const char*>(&ZERO), sizeof(std::uint32_t)); // File index
			stream.write(reinterpret_cast<const char*>(&ZERO), sizeof(std::uint32_t)); // Original file size
			stream.write(reinterpret_cast<const char*>(&ZERO), sizeof(std::uint32_t)); // Archive file size

			++currFillIn;
		}
		currFillIn = &fillIn[0];

		// Build data block
		for (FileList::const_iterator it = files.cbegin(); it != files.cend(); ++it)
		{
			const Entry &entry = (*it);

			std::uint32_t index = static_cast<std::uint32_t>(stream.tellp());
			std::uint32_t original_filesize = 0;
			std::uint32_t archive_filesize = 0;

			std::ifstream entryFile(entry.real_path, std::ios::in | std::ios::binary | std::ios::ate);
			if (entryFile.is_open())
			{
				original_filesize = static_cast<std::uint32_t>(entryFile.tellg());
				entryFile.seekg(0);

				// Write file data
				char *filedata = new char[original_filesize];
				entryFile.read(filedata, original_filesize);

				if (compress(compression, filedata, original_filesize, archive_filesize))
				{
					stream.write(filedata, archive_filesize);
				}
				else
				{
					original_filesize = 0;
					archive_filesize  = 0;
				}

				delete[] filedata;
				filedata = nullptr;

				entryFile.close();
			}

			// Fill in header
			std::uint32_t bpos = static_cast<std::uint32_t>(stream.tellp());
			std::uint32_t pos = (*currFillIn);
			stream.seekp(pos);

			stream.write(reinterpret_cast<const char*>(&index), sizeof(std::uint32_t));
			stream.write(reinterpret_cast<const char*>(&original_filesize), sizeof(std::uint32_t));
			stream.write(reinterpret_cast<const char*>(&archive_filesize), sizeof(std::uint32_t));

			stream.seekp(bpos);

			++currFillIn;
		}

		delete[] fillIn;
		fillIn = nullptr;
		currFillIn = nullptr;

		return true;
	}
}

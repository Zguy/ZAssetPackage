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
#include <ZAP/Archive.h>

#include <fstream>
#include <sstream>

namespace
{
	static const std::istream::pos_type MAGIC_POS = 0;
	static const std::istream::pos_type TABLE_POS = 4;
}

namespace ZAP
{
	Archive::Archive() : stream(nullptr)
	{
	}
	Archive::Archive(const std::string &filename) : stream(nullptr)
	{
		openFile(filename);
	}
	Archive::Archive(const char *data, std::size_t size) : stream(nullptr)
	{
		openMemory(data, size);
	}
	Archive::~Archive()
	{
		close();
	}

	bool Archive::openFile(const std::string &filename)
	{
		std::ifstream *file = new std::ifstream(filename, std::ios::in | std::ios::binary);
		if (!file->is_open())
		{
			return false;
		}
		stream = file;
		return loadStream();
	}
	bool Archive::openMemory(const char *data, std::size_t size)
	{
		stream = new std::istringstream(std::string(data, size), std::ios::in | std::ios::binary);
		return loadStream();
	}
	void Archive::close()
	{
		delete stream;
		stream = nullptr;
		header = ArchiveHeader();
		lookupTable.clear();
	}
	bool Archive::isOpen() const
	{
		return (stream != nullptr);
	}

	Compression Archive::getCompression() const
	{
		return static_cast<Compression>(header.compression);
	}
	Version Archive::getVersion() const
	{
		return static_cast<Version>(header.version);
	}

	bool Archive::isSupportedCompression() const
	{
		return supportsCompression(getCompression());
	}

	bool Archive::hasFile(const std::string &virtual_path) const
	{
		return (lookupTable.find(virtual_path) != lookupTable.cend());
	}

	DataPointer Archive::getData(const std::string &virtual_path) const
	{
		if (!isOpen() || !isSupportedCompression())
		{
			return DataPointer();
		}

		EntryMap::const_iterator it = lookupTable.find(virtual_path);
		if (it == lookupTable.cend())
		{
			return DataPointer();
		}

		const ArchiveEntry &entry = (*it).second;

		if (entry.compressed_size == 0 || entry.decompressed_size == 0)
		{
			return DataPointer();
		}

		stream->seekg(entry.index);

		std::uint32_t size = entry.decompressed_size;
		char *data = new char[entry.compressed_size];
		stream->read(data, entry.compressed_size);

		Compression compression = static_cast<Compression>(header.compression);
		if (!decompress(compression, data, entry.compressed_size, size))
		{
			delete[] data;
			data = nullptr;
			size = 0;
		}

		return DataPointer(data, size);
	}

	std::uint32_t Archive::getDecompressedSize(const std::string &virtual_path) const
	{
		if (!isOpen())
		{
			return 0;
		}

		EntryMap::const_iterator it = lookupTable.find(virtual_path);
		if (it == lookupTable.cend())
		{
			return 0;
		}

		const ArchiveEntry &entry = (*it).second;
		return entry.decompressed_size;
	}
	std::uint32_t Archive::getCompressedSize(const std::string &virtual_path) const
	{
		if (!isOpen())
		{
			return 0;
		}

		EntryMap::const_iterator it = lookupTable.find(virtual_path);
		if (it == lookupTable.cend())
		{
			return 0;
		}

		const ArchiveEntry &entry = (*it).second;
		return entry.compressed_size;
	}

	std::size_t Archive::getFileCount() const
	{
		return lookupTable.size();
	}

	void Archive::getFileList(std::vector<std::string> &list)
	{
		list.clear();

		for (EntryMap::const_iterator it = lookupTable.cbegin(); it != lookupTable.cend(); ++it)
		{
			EntryMap::const_reference entry = (*it);
			list.push_back(entry.first);
		}
	}

	bool Archive::loadStream()
	{
		if (!parseHeader())
		{
			close();
			return false;
		}
		else
		{
			buildLookupTable();
			return true;
		}
	}
	bool Archive::parseHeader()
	{
		// We assume that stream is open
		stream->seekg(MAGIC_POS);
		stream->read(reinterpret_cast<char*>(&header.magic), sizeof(std::uint16_t));
		stream->read(reinterpret_cast<char*>(&header.version), sizeof(std::uint8_t));
		stream->read(reinterpret_cast<char*>(&header.compression), sizeof(std::uint8_t));

		if (header.magic != 'AZ')
			return false;
		if (header.version < VERSION_MIN || header.version > VERSION_MAX)
			return false;
		if (header.compression < COMPRESS_NONE || header.compression >= COMPRESS_LAST)
			return false;

		return true;
	}
	void Archive::buildLookupTable()
	{
		// We assume that stream is open
		stream->seekg(TABLE_POS);
		lookupTable.clear();

		std::uint32_t tableSize = 0;
		stream->read(reinterpret_cast<char*>(&tableSize), sizeof(uint32_t));

		for (uint32_t i = 0; i < tableSize; ++i)
		{
			std::string filename;

			char c = '\0';
			for(;;)
			{
				stream->read(&c, 1);

				if (c == '\0')
					break;

				filename += c;
			}

			ArchiveEntry entry;
			stream->read(reinterpret_cast<char*>(&entry.index), sizeof(uint32_t));
			stream->read(reinterpret_cast<char*>(&entry.decompressed_size), sizeof(uint32_t));
			stream->read(reinterpret_cast<char*>(&entry.compressed_size), sizeof(uint32_t));

			lookupTable.insert(std::make_pair(filename, entry));
		}
	}
}

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
#include <ZAP/Archive.h>

#include <fstream>
#include <sstream>

namespace
{
	const std::istream::pos_type MAGIC_POS = 0;
	const std::istream::pos_type TABLE_POS = 4;

	template<typename T>
	inline void readField(std::istream *stream, T *field)
	{
		stream->read(reinterpret_cast<char*>(field), sizeof(T));
	}
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

	bool Archive::getData(const std::string &virtual_path, char *&return_data, size_t &return_size) const
	{
		const ArchiveEntry *entry = getEntry(virtual_path);
		if (entry == nullptr || !isSupportedCompression())
			return false;

		if (entry->compressed_size == 0 || entry->decompressed_size == 0)
			return false;

		stream->seekg(entry->index);

		std::uint32_t size = entry->decompressed_size;
		char *data = new char[entry->compressed_size];
		stream->read(data, entry->compressed_size);

		if (!decompress(getCompression(), data, entry->compressed_size, size))
		{
			delete[] data;
			return false;
		}

		return_data = data;
		return_size = size;
		return true;
	}

	std::uint32_t Archive::getDecompressedSize(const std::string &virtual_path) const
	{
		const ArchiveEntry *entry = getEntry(virtual_path);
		if (entry == nullptr)
			return 0;

		return entry->decompressed_size;
	}
	std::uint32_t Archive::getCompressedSize(const std::string &virtual_path) const
	{
		const ArchiveEntry *entry = getEntry(virtual_path);
		if (entry == nullptr)
			return 0;

		return entry->compressed_size;
	}

	std::size_t Archive::getFileCount() const
	{
		return lookupTable.size();
	}

	void Archive::getFileList(std::vector<std::string> &list) const
	{
		for (EntryMap::const_reference entry : lookupTable)
		{
			list.push_back(entry.first);
		}
	}

	const Archive::ArchiveEntry *Archive::getEntry(const std::string &virtual_path) const
	{
		if (!isOpen())
			return nullptr;

		EntryMap::const_iterator it = lookupTable.find(virtual_path);
		if (it == lookupTable.cend())
			return nullptr;

		return &(*it).second;
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
		readField(stream, &header.magic);
		readField(stream, &header.version);
		readField(stream, &header.compression);

		if (header.magic != 'AZ')
			return false;
		if (header.version < VERSION_MIN || header.version > VERSION_MAX)
			return false;

		return true;
	}
	void Archive::buildLookupTable()
	{
		// We assume that stream is open
		stream->seekg(TABLE_POS);
		lookupTable.clear();

		std::uint32_t tableSize = 0;
		readField(stream, &tableSize);

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
			readField(stream, &entry.index);
			readField(stream, &entry.decompressed_size);
			readField(stream, &entry.compressed_size);

			lookupTable.emplace(filename, entry);
		}
	}
}

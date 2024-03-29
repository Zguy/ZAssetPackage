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
#ifndef ZAP_Archive_h__
#define ZAP_Archive_h__

#include <ZAP/Compression.h>
#include <ZAP/Version.h>

#include <cstdint>
#include <istream>
#include <unordered_map>
#include <string>
#include <vector>

namespace ZAP
{
	///\brief Used to load an archive.
	class Archive
	{
	public:
		///\brief File in archive.
		struct Entry
		{
			std::string virtual_path;        ///< Virtual path of the file.
			std::uint32_t index;             ///< Offset in the archive file.
			std::uint32_t decompressed_size; ///< Size of the file when decompressed in bytes.
			std::uint32_t compressed_size;   ///< Size of the file when compressed in bytes.
		};
		typedef std::vector<const Entry*> EntryList;

		///\brief Default constructor.
		Archive();

		///\brief Constructor that calls openFile().
		///\param filename Filename of the archive.
		Archive(const std::string &filename);

		///\brief Constructor that calls openMemory().
		///\param data A pointer to the data.
		///\param size Size of the data.
		Archive(const char *data, std::size_t size);

		~Archive();

		///\brief Opens an archive from file.
		///\param filename Filename of the archive.
		///\return false if the file can't be opened.
		bool openFile(const std::string &filename);

		///\brief Opens an archive from memory.
		///\param data A pointer to the data.
		///\param size Size of the data.
		///\return false if it fails.
		bool openMemory(const char *data, std::size_t size);

		///\brief Closes the archive.
		///
		/// It's not important to call this because it's called by the destructor.
		void close();

		///\brief Checks if this archive is opened.
		bool isOpen() const;

		///\brief Returns the compression method this archive uses.
		Compression getCompression() const;

		///\brief Returns the archive format version this archive is saved with.
		Version getVersion() const;

		///\brief Returns whether this build of the library supports the compression method used by this archive.
		bool isSupportedCompression() const;

		///\brief Checks if the archive contains a file.
		///\param virtual_path Full pathname of the virtual file.
		bool hasFile(const std::string &virtual_path) const;

		///\brief Extracts the data of a file.
		///\param virtual_path Full pathname of the virtual file.
		///\param [out] data The data, untouched if failed.
		///\param [out] size The data size, untouched if failed.
		///\return false if the virtual_path does not exist, or uses an unsupported compression.
		bool getData(const std::string &virtual_path, char *&data, std::size_t &size) const;
		bool getData(const Entry *entry, char *&data, std::size_t &size) const;

		///\brief Extracts the raw data of a file.
		///
		/// If the archive is compressed, this will return the compressed data.
		/// If not, this will return the same as getData.
		///\param virtual_path Full pathname of the virtual file.
		///\param [out] data The data, untouched if failed.
		///\param [out] size The data size, untouched if failed.
		///\return false if the virtual_path does not exist.
		bool getRawData(const std::string &virtual_path, char *&data, std::size_t &size) const;
		bool getRawData(const Entry *entry, char *&data, std::size_t &size) const;

		///\brief Returns a pointer to the Entry of a file.
		///\param virtual_path Full pathname of the virtual file.
		///\return null if the virtual_path does not exist.
		const Entry *getEntry(const std::string &virtual_path) const;

		///\brief Returns the number of files in the archive.
		std::size_t getFileCount() const;

		///\brief Gets the list of files in the archive.
		///\param [out] list The list.
		void getFileList(EntryList &list) const;

	private:
		struct Header
		{
			Header() : magic(0), version(0), compression(0) {}
			std::uint16_t magic;
			std::uint8_t version;
			std::uint8_t compression;
		};
		typedef std::unordered_map<std::string, Entry> EntryMap;

		bool loadStream();
		bool parseHeader();
		void buildLookupTable();

		std::istream *stream;

		Header header;
		EntryMap lookupTable;
	};
}

#endif // ZAP_Archive_h__

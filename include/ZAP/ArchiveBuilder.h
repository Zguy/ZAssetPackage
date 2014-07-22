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
#ifndef ZAP_ArchiveBuilder_h__
#define ZAP_ArchiveBuilder_h__

#include <ZAP/Compression.h>

#include <map>
#include <set>
#include <string>

namespace ZAP
{
	/**
	 * \brief	Used to create an archive.
	 */
	class ArchiveBuilder
	{
	public:
		/**
		 * \brief	Default constructor.
		 */
		ArchiveBuilder();
		~ArchiveBuilder();

		/**
		 * \brief	Adds a file to the archive.
		 *
		 * You can add the same file to multiple virtual paths,
		 * but you can not add multiple files to the same virtual path.
		 *
		 * \note This method does not check if the file exists.
		 *
		 * \param	real_path			Path to the file on the filesystem.
		 * \param	virtual_path	Path to the file in the archive (can be anything).
		 *
		 * \return	true if the file was added, false if the virtual path already exists.
		 */
		bool addFile(const std::string &real_path, const std::string &virtual_path);

		/**
		 * \brief	Removes a file from the archive.
		 *
		 * \param	virtual_path	Full pathname of the virtual file.
		 *
		 * \return	true if the file was removed, false if it didn't exist.
		 */
		bool removeFile(const std::string &virtual_path);

		/**
		 * \brief	Removes all files added to the archive.
		 */
		void clearFiles();

		/**
		 * \brief	Returns the number of files added to the archive.
		 */
		std::size_t getFileCount() const;

		/**
		 * \brief	Gets all added files in a map.
		 *
		 * The key is the virtual path and the value is the real path.
		 *
		 * \param [out]	map	The map.
		 */
		void getFileMap(std::map<std::string,std::string> &map);

		/**
		 * \brief	Builds the archive to a file.
		 * \note If a file cannot be found, a zero-length file will be stored.
		 *
		 * \param	filename	Filename to save the archive to.
		 * \param	compression	(optional) The compression method to use, defaults to none.
		 *
		 * \return	true if it succeeds, false if it fails.
		 */
		bool buildFile(const std::string &filename, Compression compression = COMPRESS_NONE);

		/**
		 * \brief Builds the archive to memory.
		 * \note If a file cannot be found, a zero-length file will be stored.
		 *
		 * \param [out] data	The resulting data, untouched if failed.
		 * \param [out] size	The resulting size, untouched if failed.
		 * \param compression	(optional) The compression method to use, defaults to none.
		 *
		 * \return true if it succeeds, false if it fails.
		 */
		bool buildMemory(char *&data, std::size_t &size, Compression compression = COMPRESS_NONE);

	private:
		bool build(std::ostream &stream, Compression compression);

		struct Entry
		{
			Entry(const std::string &real_path, const std::string &virtual_path) : real_path(real_path), virtual_path(virtual_path) {}
			bool operator<(const Entry &rhs) const
			{
				return (virtual_path < rhs.virtual_path);
			}
			std::string real_path;
			std::string virtual_path;
		};
		typedef std::set<Entry> FileList;
		FileList files;
	};
}

#endif // ZAP_ArchiveBuilder_h__

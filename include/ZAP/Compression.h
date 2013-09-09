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
#ifndef ZAP_Compression_h__
#define ZAP_Compression_h__

#include <cstdint>

/**
 * \brief ZAssetPackage.
 */
namespace ZAP
{
	/**
	 * \brief	The supported compression methods.
	 */
	enum Compression
	{
		COMPRESS_NONE = 0, ///< No compression.
		COMPRESS_LZ4  = 1, ///< LZ4 compression.
		COMPRESS_LAST
	};

	/**
	 * \brief	Compress data.
	 *
	 * \param	compression					The compression method.
	 * \param [in,out]	data			The data to compress. This will be delete[]d and replaced with the compressed data.
	 * \param	in_size							Size of the decompressed data.
	 * \param [out]	out_size			Size of the compressed data.
	 *
	 * \return	true if it succeeds, false if it fails.
	 */
	bool compress(Compression compression, char *&data, std::uint32_t in_size, std::uint32_t &out_size);

	/**
	 * \brief	Decompress data.
	 *
	 * \param	compression			The compression method.
	 * \param [in,out]	data	The data to decompress. This will be delete[]d and replaced with the decompressed data.
	 * \param	in_size					Size of the compressed data.
	 * \param	out_size				Size of the decompressed data.
	 *
	 * \return	true if it succeeds, false if it fails.
	 */
	bool decompress(Compression compression, char *&data, std::uint32_t in_size, std::uint32_t out_size);
}

#endif // ZAP_Compression_h__

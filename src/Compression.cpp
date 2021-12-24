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
#include <ZAP/Compression.h>
#include "Config.h"

#ifdef ZAP_COMPRESS_LZ4
	#include <lz4/lz4.h>
	#include <lz4/lz4hc.h>
#endif

namespace ZAP
{
	bool supportsCompression(Compression compression)
	{
		switch (compression)
		{
		case Compression::NONE: return true;
		#ifdef ZAP_COMPRESS_LZ4
		case Compression::LZ4: return true;
		#endif
		default: return false;
		}
	}

	bool compress(Compression compression, char *&data, std::uint32_t in_size, std::uint32_t &out_size)
	{
		if (data == nullptr)
		{
			return false;
		}

		switch (compression)
		{
			case Compression::NONE:
			{
				out_size = in_size;
				return true;
			}
			#ifdef ZAP_COMPRESS_LZ4
			case Compression::LZ4:
			{
				int out_capacity = LZ4_compressBound(in_size);
				char *out_data = new char[out_capacity];
				out_size = LZ4_compress_HC(data, out_data, in_size, out_capacity, LZ4HC_CLEVEL_DEFAULT);
				if (out_size > 0)
				{
					delete[] data;
					data = out_data;
					return true;
				}
				else
				{
					delete[] out_data;
					return false;
				}
			}
			#endif
			default:
			{
				return false;
			}
		}
	}

	bool decompress(Compression compression, char *&data, std::uint32_t in_size, std::uint32_t out_size)
	{
		switch (compression)
		{
			case Compression::NONE:
			{
				return true;
			}
			#ifdef ZAP_COMPRESS_LZ4
			case Compression::LZ4:
			{
				char *out_data = new char[out_size];
				if (LZ4_decompress_safe(data, out_data, in_size, out_size) > 0)
				{
					delete[] data;
					data = out_data;
					return true;
				}
				else
				{
					delete[] out_data;
					return false;
				}
			}
			#endif
			default:
			{
				return false;
			}
		}
	}
}

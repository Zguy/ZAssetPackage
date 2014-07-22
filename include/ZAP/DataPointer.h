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
#ifndef ZAP_DataPointer_h__
#define ZAP_DataPointer_h__

#include <cstdint>

namespace ZAP
{
	/**
	 * \brief	Associates a data pointer with its size.
	 */
	class DataPointer
	{
	public:
		/**
		 * \brief	Default constructor.
		 *
		 * This constructs a null DataPointer.
		 */
		DataPointer() : data(nullptr), size(0)
		{
		}

		/**
		 * \brief	Constructor.
		 *
		 * \param	data	Data.
		 * \param	size	Size of data.
		 */
		DataPointer(const char *data, std::uint32_t size) : data(data), size(size)
		{
		}

		DataPointer(const DataPointer &other) : data(other.data), size(other.size)
		{
		}
		~DataPointer()
		{
		}

		/**
		 * \brief	Assignment operator.
		 *
		 * \param	rhs	The right hand side.
		 */
		DataPointer &operator=(const DataPointer &rhs)
		{
			if (&rhs != this)
			{
				data = rhs.data;
				size = rhs.size;
			}
			return *this;
		}

		/**
		 * \brief	Deletes the data.
		 *
		 * You must call this method or manually delete[] the data.
		 */
		void deleteData()
		{
			delete[] data;
			data = nullptr;
			size = 0;
		}

		/**
		 * \brief	Checks if the data is null.
		 *
		 * \return	true if null, false if not.
		 */
		bool isNull() const
		{
			return (data == nullptr);
		}

		/**
		 * \brief	Returns the data.
		 */
		const char *getData() const
		{
			return data;
		}

		/**
		 * \brief	Returns the size of data.
		 */
		std::uint32_t getSize() const
		{
			return size;
		}

	private:
		const char *data;
		std::uint32_t size;
	};
}

#endif // ZAP_DataPointer_h__

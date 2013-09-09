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
#ifndef ZAP_Version_h__
#define ZAP_Version_h__

namespace ZAP
{
	/**
	 * \brief	Versions of the archive format.
	 */
	enum Version
	{
		VERSION_1_0 = 0, ///< Version 1.0.
		VERSION_MIN = VERSION_1_0, ///< The minimum version supported.
		VERSION_MAX = VERSION_1_0, ///< The maximum version supported.
		VERSION_CUR = VERSION_MAX ///< The default version.
	};
}

#endif // ZAP_Version_h__

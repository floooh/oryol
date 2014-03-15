///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Mathematics (glm.g-truc.net)
///
/// Copyright (c) 2005 - 2014 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref core
/// @file glm/core/dummy.cpp
/// @date 2011-01-19 / 2011-06-15
/// @author Christophe Riccio
///
/// GLM is a header only library. There is nothing to compile. 
/// dummy.cpp exist only a wordaround for CMake file.
///////////////////////////////////////////////////////////////////////////////////

#define GLM_FORCE_RADIANS
#define GLM_MESSAGES
#include "../glm.hpp"
#include <limits>
/*
#if(GLM_ARCH & GLM_ARCH_SSE2)
struct float4
{
	union
	{
		struct {float r, g, b, a;};
		struct {float s, t, p, q;};
		struct {float x, y, z, w;};
		__m128 data;
	};
};

int test_simd()
{
	float4 f;



	return 0;
}

#endif//GLM_ARCH
*/

template <class T = int>
class C;

template <class T>
class C
{
public:
	T value;
};

int main()
{
/*
#	if(GLM_ARCH & GLM_ARCH_SSE2)
		test_simd();
#	endif
*/ 

	C<> c;

	return 0;
}

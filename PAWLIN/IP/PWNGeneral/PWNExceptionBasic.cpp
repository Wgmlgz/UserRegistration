// (c) Pawlin Technologies Ltd. 2010
// http://www.pawlin.ru
// File: PWNExeptionBasic.h, c.pp 
// Purpose: Header file for PWNExeptionBasic class, that implements generic exception class with text message
// Author: Mikhail Yakovlev
// ALL RIGHTS RESERVED. USAGE OF THIS FILE FOR ANY REASON
// WITHOUT WRITTEN PERMISSION FROM
// PAWLIN TECHNOLOGIES LTD IS PROHIBITED
// ANY VIOLATION OF THE COPYRIGHT AND PATENT LAW WILL BE PROSECUTED
// FOR ADDITIONAL INFORMATION REFER www.pawlin.ru

#include "stdafx.h"
#include "PWNExceptionBasic.h"
#include <string.h>
#include <stdlib.h>


PWNExceptionBasic::PWNExceptionBasic(void)
	:myWhat(NULL), myDoFree(false)
{
}

PWNExceptionBasic::PWNExceptionBasic(const std::string & whatStr)
	:myWhat(NULL), myDoFree(false)
{
	copyStr(whatStr.c_str());
}

PWNExceptionBasic::PWNExceptionBasic(const char* whatStr)
	:myWhat(NULL), myDoFree(false)
{
	copyStr(whatStr);
}

PWNExceptionBasic::PWNExceptionBasic(const PWNExceptionBasic& that)
	: myWhat(NULL), myDoFree(false)
{
	*this = that;
}

PWNExceptionBasic::~PWNExceptionBasic(void) throw()
{
	tidy();
}

const char* PWNExceptionBasic::what() const throw()
{
	return myWhat;
}

PWNExceptionBasic& PWNExceptionBasic::operator= (const PWNExceptionBasic& that)
{
	if (this != &that)
	{
		tidy();
		if (that.myDoFree)
		{
			copyStr(that.myWhat);
		}
		else
		{
			myWhat = that.myWhat;
		}
	}

	return *this;
}

void PWNExceptionBasic::copyStr(const char* whatStr)
{
	if (whatStr != NULL)
		{
		const size_t bufSize = strlen(whatStr) + 1;

		myWhat = static_cast<char *>(malloc(bufSize));

		if (myWhat != NULL)
			{
#ifndef WIN32
			strcpy(const_cast<char *>(myWhat), whatStr);
#else
			strcpy_s(const_cast<char *>(myWhat),bufSize, whatStr);
#endif
			myDoFree = true;
			}
		}
}

void PWNExceptionBasic::tidy()
{
	if (myDoFree)
	{
		free(const_cast<char *>(myWhat));
	}

	myWhat = NULL;
	myDoFree = false;
}

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


#pragma once

#include <exception>
#include <string>

class PWNExceptionBasic: public std::exception
{
	const char* myWhat;
	bool myDoFree;
	void copyStr(const char* whatStr);
	void tidy();
public:
	PWNExceptionBasic(void);
	PWNExceptionBasic(const char* whatStr);
	PWNExceptionBasic(const std::string &whatStr);
	PWNExceptionBasic(const PWNExceptionBasic& that);
	virtual ~PWNExceptionBasic(void) throw();

	virtual const char* what() const throw();
	virtual PWNExceptionBasic& operator= (const PWNExceptionBasic& e);
};


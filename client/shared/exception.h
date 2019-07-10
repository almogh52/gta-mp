#pragma once

#include <exception>
#include <string>

#include "error.h"

namespace gtamp
{
class exception : public std::exception
{
private:
	error _code;
	std::string _message;
	std::string _format;

public:
	exception(const error code, const std::string message) : _code(code), _message(message) {
		_format = "Error Code: " + std::to_string(_code) + ". " + _message + "\n";
	}
	virtual const char *what() const throw()
	{
		return _format.c_str();
	}
};
} // namespace gtamp

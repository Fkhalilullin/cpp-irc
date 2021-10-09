#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>

#include <sstream>
#include <vector>

class Message {
public:
	Message(std::string); 
	~Message();

	// const std::string			&_getPrefix() const;	// [Parse without prefix]
	const std::string				&getCommand() const;
	const std::vector<std::string>	&getParamets() const;

private:
	Message();

	// std::string 			 		_prefix; 					// [Parse without prefix]
	std::string 			 		_command;
	std::vector<std::string>		_parameters;

	void							_parse(std::string str);
	void							_parseUtility(std::vector<std::string> vec_sep_space);
	void							_printTest();
	bool							_checkColon(const std::string &str);
	bool							_checkComma(const std::string &str);
	std::vector<std::string>		_split(const std::string &str, char delimeter);
};

#endif

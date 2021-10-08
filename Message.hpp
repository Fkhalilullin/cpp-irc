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
	const std::string				&_getCommand() const;
	const std::vector<std::string>	&_getParamets() const;

private:
	// std::string 			 		_prefix; 					// [Parse without prefix]
	std::string 			 		_command;
	std::vector<std::string>		_parameters;

	Message();
	void							_parse(std::string str);
	std::vector<std::string>		_split(const std::string &str, char delimeter);
	bool							_checkColon(const std::string &str);
	bool							_checkComma(const std::string &str);
	void							_parseUtility(std::vector<std::string> vec_sep_space);
	void							_printTest();
};

#endif

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>

#include <sstream>
#include <vector>

#include "User.hpp"

class Message {
public:
	Message(std::string, const User& ); 
	~Message();

	const std::string				&getPrefix() const;
	const std::string				&getCommand() const;
	const std::vector<std::string>	&getParamets() const;


private:
	Message();

	std::string 			 		_prefix;
	std::vector<std::string>		_parameters;
	std::string 			 		_command;

	bool							_isPrefix;
	bool							_isCommand;

	void							_parse( std::string str, const User& );
	void							_parseUtility( std::vector<std::string>, const User&);
	void							_printTest();
	bool							_checkColon( const std::string &str );
	bool							_checkComma( const std::string &str );
	std::vector<std::string>		_split( const std::string &str, char delimeter );
};

#endif

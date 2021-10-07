#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <vector>

class Message{
	public: // private
		std::string 			 _prefix;
		std::string 			 _command;
		std::vector<std::string> _parameters;

		Message();
		~Message();

		void							_parse(std::string str);
		const std::string				&_getPrefix() const;
		const std::string				&_getCommand() const;
		const std::vector<std::string>	&_getParamets() const;



};

#endif

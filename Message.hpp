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
		const std::string				&getPrefix() const;
		const std::string				&getCommand() const;
		const std::vector<std::string>	&getParamets() const;


};

#endif

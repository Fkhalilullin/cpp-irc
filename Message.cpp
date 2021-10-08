#include "Message.hpp"

Message::Message() {}

Message::~Message() {}

Message::Message(std::string str) {
    this->_parse(str);
}

const std::string				&Message::_getCommand() const {
    return this->_command;
}

const std::vector<std::string>	&Message::_getParamets() const {
    return this->_parameters;
}

void Message::_parse(std::string str) {
    std::vector<std::string> vec_sep_space; 
    std::vector<std::string> vec_sep_colon;

    if (_checkColon(str)) {
        std::string buf_str;

        vec_sep_colon = _split(str, ':');
        for (int i = 1; i < vec_sep_colon.size(); ++i) 
            buf_str += vec_sep_colon[i];
        vec_sep_space = _split(vec_sep_colon[0], ' ');
        _parseUtility(vec_sep_space);
        this->_parameters.push_back(buf_str);
    }
    else {
        vec_sep_space = _split(str, ' ');
        _parseUtility(vec_sep_space);
    }
    this->_printTest();
}

std::vector<std::string> Message::_split(const std::string &str, char delimeter) {
    std::vector<std::string> result;
    std::istringstream sstream(str);
    std::string tmp;

    while (std::getline(sstream, tmp, delimeter)) {
        if (!tmp.empty())
            result.push_back(tmp);
    }
    return result;
}

bool Message::_checkColon(const std::string &str) {
    for (int i = 0; i != str.size(); ++i) {
        if (str[i] == ':')
            return true;
    }
    return false;
}

bool Message::_checkComma(const std::string &str) {
    for (int i = 0; i != str.size(); ++i) {
        if (str[i] == ',')
            return true;
    }
    return false;
}

void Message::_parseUtility(std::vector<std::string> vec_sep_space) {
    std::vector<std::string> vec_sep_comma; 
    if (!vec_sep_space.empty()) {
        for (int i = 0; i != vec_sep_space.size(); ++i) {
            if (i == 0)
                this->_command = vec_sep_space[i]; 
            else {
                if (_checkComma(vec_sep_space[i])) {
                    vec_sep_comma = _split(vec_sep_space[i], ',');
                    for (int j = 0; j < vec_sep_comma.size(); ++j)
                         this->_parameters.push_back(vec_sep_comma[j]);
                }
                else
                    this->_parameters.push_back(vec_sep_space[i]);
            }
        }
    }
}

void Message::_printTest() {
    std::cout << std::endl;

    std::cout << "CMD: " << std::endl << this->_command << std::endl;
    
    std::cout << "PARAMETERS: " <<  std::endl;
    for (int i = 0; i < _parameters.size(); ++i) {
        std::cout << _parameters[i] << std::endl;
    }
}
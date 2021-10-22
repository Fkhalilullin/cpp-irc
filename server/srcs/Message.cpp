#include "../includes/Message.hpp"

Message::Message() {}

Message::Message(const Message &rhs) : _prefix(rhs._prefix),
                                       _parameters(rhs._parameters),
                                       _command(rhs._command),
                                       _isPrefix(rhs._isPrefix),
                                       _isCommand(rhs._isCommand)
{
}

Message::~Message() {}

Message::Message(std::string str, const User &usr)
    : _isPrefix(false), _isCommand(false)
{
    this->_parse(str, usr);
}

void Message::setCommand(const std::string &command)
{
    _command = command;
}

const std::string &Message::getCommand() const { return this->_command; }

const std::vector<std::string> &Message::getParamets() const { return this->_parameters; }

const std::string &Message::getPrefix() const { return this->_prefix; }

void Message::_parse(std::string str, const User &usr)
{
    std::vector<std::string> vec_sep_space;
    std::vector<std::string> vec_sep_colon;

    this->_prefix = usr.getNickname();

    if (str[0] == ':')
        this->_isPrefix = true;

    if (_checkColon(str))
    {
        std::string buf_str;

        vec_sep_colon = _split(str, ':');
        for (size_t i = 1; i < vec_sep_colon.size(); ++i)
            buf_str += vec_sep_colon[i];
        vec_sep_space = _split(vec_sep_colon[0], ' ');
        _parseUtility(vec_sep_space);
        this->_parameters.push_back(buf_str);
    }
    else
    {
        vec_sep_space = _split(str, ' ');
        _parseUtility(vec_sep_space);
    }
}

std::vector<std::string> Message::_split(const std::string &str, char delimeter)
{
    std::vector<std::string> result;
    std::istringstream sstream(str);
    std::string tmp;

    while (std::getline(sstream, tmp, delimeter))
    {
        if (!tmp.empty())
            result.push_back(tmp);
    }
    return result;
}

bool Message::_checkColon(const std::string &str)
{
    for (size_t i = 0; i != str.size(); ++i)
    {
        if (str[i] == ':')
            return true;
    }
    return false;
}

bool Message::_checkComma(const std::string &str)
{
    for (size_t i = 0; i != str.size(); ++i)
    {
        if (str[i] == ',')
            return true;
    }
    return false;
}

void Message::_parseUtility(std::vector<std::string> vec_sep_space)
{
    std::vector<std::string> vec_sep_comma;

    if (!vec_sep_space.empty())
    {
        for (size_t i = 0; i != vec_sep_space.size(); ++i)
        {
            if (_isPrefix && i == 0)
            {
                this->_prefix = vec_sep_space[i];
            }
            else if (!_isCommand)
            {
                this->_command = vec_sep_space[i];
                _isCommand = true;
            }
            else
            {
                if (_checkComma(vec_sep_space[i]))
                {
                    vec_sep_comma = _split(vec_sep_space[i], ',');
                    for (size_t j = 0; j < vec_sep_comma.size(); ++j)
                        this->_parameters.push_back(vec_sep_comma[j]);
                }
                else
                    this->_parameters.push_back(vec_sep_space[i]);
            }
        }
    }
}

void Message::_printTest()
{
    std::cout << std::endl;

    std::cout << "PREFIX: " << std::endl
              << this->_prefix << std::endl;

    std::cout << "CMD: " << std::endl
              << this->_command << std::endl;

    std::cout << "PARAMETERS: " << std::endl;
    for (size_t i = 0; i < _parameters.size(); ++i)
    {
        std::cout << _parameters[i] << std::endl;
    }
}

#pragma once
class EventException : public std::exception {
public:
    explicit EventException(const std::string& m) : message{m} {}
    virtual const char* what() const noexcept override {return message.c_str();}
private:
    std::string message = "";
};

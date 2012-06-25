#ifndef __ebObject_h__
#define __ebObject_h__


#include <exception>
#include <stdexcept>
#include <string>


namespace rho
{


class ebObject : public std::exception
{
    public:

        ebObject(std::string reason)
            : m_reason(reason)
        {
        }

        virtual std::string reason() const
        {
            return m_reason;
        }

        // Override from std::exception
        virtual const char* what() const throw()
        {
            return m_reason.c_str();
        }

        ~ebObject() throw() { }

    private:

        // Disable the default constructor.
        ebObject();

    private:

        std::string m_reason;
};


}   // namespace rho


#endif    // __ebObject_h__

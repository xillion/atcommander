#include "atcommander.h"

constexpr char ATCommander::OK[];
constexpr char ATCommander::AT[];
constexpr char ATCommander::WHITESPACE_NEWLINE[];

bool ATCommander::check_for_ok()
{
    ignore_whitespace_and_newlines();
    bool got_ok = input_match(OK);
    if(!got_ok) return false;
    skip_newline();
    return true;
}

void ATCommander::ignore_whitespace_and_newlines()
{
    char ch;

    while(is_match(ch = get(), WHITESPACE_NEWLINE));

    unget(ch);
}

void ATCommander::ignore_whitespace()
{
    char ch;

    while((ch = get()) == ' ');

    unget(ch);
}

bool ATCommander::skip_newline()
{
    // look for CRLF, LFCR, or LF alone.  CR alone not supported at this time
    ignore_whitespace();
    char ch = get();
    if(ch == 13)
    {
        ch = get();
        if(ch == 10)
        {
            return true;
        }

        unget(ch);
    }
    else if(ch == 10)
    {
        ch = get();
        if(ch == 13)
        {
            return true;
        }

        unget(ch);
        return true;
    }

    return false;
}



// retrieves a text string in input up to max size
// leaves any discovered delimiter cached
size_t ATCommander::input(char* input, size_t max)
{
    char ch;
    size_t len = 0;

    while(!is_delimiter(ch = get()) && len < max)
    {
        if(ch == -1) return 0;

        *input++ = ch;
        len++;
    }

    // FIX: be sure to check len also
    unget(ch);

    return len;
}


template <>
ATCommander& ATCommander::operator>>(const char* matchValue)
{
    if(!input_match(matchValue))
    {
        set_error("match", matchValue);
    }
    return *this;
}



template <>
ATCommander& ATCommander::operator>>(char* matchValue)
{
    input(matchValue, 100);
    return *this;
}


template <>
bool ATCommander::input(const char*& input)
{
    // be sure to FAIL here
    //static_assert("Cannot input into a static pointer");
    return false;
}




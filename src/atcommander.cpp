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
    int ch;
    size_t len = 0;

    while(!is_delimiter(ch = get()) && len < max && ch != -1)
    {
        *input++ = ch;
        len++;
    }

    // FIX: check if input can be assigned to still
    // (ensure we haven't exceeded max)
    *input = 0;

    if(ch != -1)
        // FIX: be sure to check len also
        unget(ch);

    return len;
}


void ATCommander::do_assign(const char *cmd)
{
    do_command(cmd);
    cout.put('=');
}


/*
template <>
ATCommander& ATCommander::operator>>(const char* matchValue)
{
    if(!input_match(matchValue))
    {
        set_error("match", matchValue);
    }
    return *this;
}
*/


/*
template <>
ATCommander& ATCommander::operator>>(char* inputValue)
{
    input(inputValue, 100);
    return *this;
}
*/
/*
template <>
bool ATCommander::input(const char*& input)
{
    // be sure to FAIL here
    //static_assert("Cannot input into a static pointer");
    return false;
} */

void ATCommander::do_request_prefix(const char *cmd)
{
    send_request(cmd);
    recv_request_echo_prefix(cmd);
}


namespace layer3 {

const char* MultiMatcher::do_match(const char* input, const char** keywords)
{
    const char* keyword;

    while((keyword = *keywords++))
    {
        if(strcmp(keyword, input) == 0) return keyword;
    }

    return nullptr;
}

bool MultiMatcher::parse(char c)
{
    const char** _keyword = &keywords[vpos];
    const char* keyword;

    // If we still have keywords to inspect
    while((keyword = *_keyword) != nullptr)
    {
        char ch = *(keyword + hpos);
        // see if this particular keyword at this particular horizontal
        // position IN the keyword matches incoming c
        if(ch == c)
        {
            // If so, bump hpos up as we have a "so far" match and prep
            // for the next one
            hpos++;
            return true;
        }
        else if(c == 13)
        {
            // FIX: eating CR for now, but we can do better (parse it out before we get here)
            return true;
        }
            // TODO: make incoming terminator more configurable
        else if(c == 10 && ch == 0)
        {
            // if we get here it means both incoming character and our own keyword
            // are at their delimiters, meaning an exact match
            match = true;
            // and we're done
            return false;
        }

        // if no match, then move forward (alphabetically)
        // down the line
        vpos++;
        _keyword++;

        // NOTE: this is what makes alphabetical ordering important
        // if we don't get a match, we compare the current const match to the
        // next potential const match to see if the prefix
        // at least matches so that we can then compare a suffix.  If prefix does
        // NOT match, that means we are done - again, things must be alphabetical
        // for that to work
        if (memcmp(keyword, *_keyword, hpos) != 0) return false;
    }

    // if we get to the end, then definitely no match
    return false;
}

}

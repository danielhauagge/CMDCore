//
//  OptionParser.cpp
//  OptionParser
//
//  Created by Daniel Hauagge on 4/29/12.
//  Copyright (c) 2012 Cornell University. All rights reserved.
//

#include "optparser"

CLC_NAMESPACE_BEGIN

#define PRINT_EXPR(expr) std::cout << "[optparse] " << __LINE__ << ": "<< #expr << " = " << (expr) << std::endl
#define PRINT_MSG(msg) std::cout << "[optparse] " << msg << std::endl

// ****************************************************************************
// * Constants controlling the formatting of the help message                 *
// ****************************************************************************

static const int COLUMN_WIDTH         = 80;
static const int LEFT_PADDING         = 2;
static const int OPTIONS_LEFT_PADDING = 2;
static const int OPTIONS_WIDTH        = 25;

// Number of spaces between options and their description
static const int OPT_DESC_PADDING     = 3;
static const int OPTDESC_WIDTH        = COLUMN_WIDTH - OPT_DESC_PADDING - OPTIONS_WIDTH;

// ****************************************************************************
// * Utilities                                                                *
// ****************************************************************************

static
void
replaceSubStr(std::string& str, const std::string& outStr, const std::string& inStr)
{
    size_t pos = str.find(outStr);
    if(pos != std::string::npos) {
        str.replace(pos, outStr.size(), inStr);
    }
}

/// Breaks a string into lines at most colWidth characters long
static
std::vector<std::string>
formatLines(const std::string& str_, int colWidth, int leftPadding = 0, bool breakIntoLines = true)
{
    const char* delims = " \n";
    if(!breakIntoLines) delims = "";

    char* str = new char[str_.size() + 1];
    strcpy(str, str_.c_str());

    std::stringstream leftPad;
    for(int i = 0; i < leftPadding; i++) leftPad << " ";

    std::stringstream line;
    std::vector<std::string> lines;
    std::string sep = "";
    for(char* token = strtok(str, delims); token != NULL; token = strtok(NULL, delims)) {
        if(line.str().size() + strlen(token) + leftPadding > colWidth && line.str().size()) {
            while(line.str().size() + leftPadding < colWidth) line << " ";
            lines.push_back(leftPad.str() + line.str());
            line.str("");
            sep = "";
        }
        line << sep << token;
        sep = " ";
    }
    if(line.str().size()) {
        while(line.str().size() + leftPadding < colWidth) line << " ";
        lines.push_back(leftPad.str() + line.str());
    }

    delete [] str;

    return lines;
}

static
std::vector<std::string>
mergeColumns(const std::vector<std::string>& lines1, int width1,
             const std::vector<std::string>& lines2, int width2,
             int colPadding = 0)
{
    std::vector<std::string> merged;
    for(int i1 = 0, i2 = 0; i1 < lines1.size() || i2 < lines2.size(); i1++) {
        std::stringstream line;

        if(i1 > (lines1.size() - 1)) {
            for(int j = 0; j < width1; j++) line << " ";
        } else {
            line << lines1[i1];
        }

        // If the first column is wider than the specified width
        // then do not copy text from the second block of text,
        // instead, push the line down.
        if(line.str().size() > width1) {
            for(int j = line.str().size(); j < colPadding + width2 + width1; j++) line << " ";
        } else {
            for(int j = 0; j < colPadding; j++) line << " ";

            if(i2 > (lines2.size() - 1)) {
                for(int j = 0; j < width2; j++) line << " ";
            } else {
                line << lines2[i2];
                i2++;
            }
        }

        merged.push_back(line.str());
    }

    return merged;
}

// ****************************************************************************
// * ConvertibleString                                                        *
// ****************************************************************************

OptionParser::ConvertibleString::ConvertibleString():
    std::string()
{
}

OptionParser::ConvertibleString::ConvertibleString(const std::string& other):
    std::string(other)
{
}

OptionParser::ConvertibleString::ConvertibleString(const char* other):
    std::string(other)
{
}

OptionParser::ConvertibleString&
OptionParser::ConvertibleString::operator=(const std::string& other)
{
    this->std::string::operator=(other);
    return *this;
}

OptionParser::ConvertibleString&
OptionParser::ConvertibleString::operator=(const char* other)
{
    this->std::string::operator=(other);
    return *this;
}

int
OptionParser::ConvertibleString::asInt() const
{
    return atoi(this->c_str());
}

double
OptionParser::ConvertibleString::asFloat() const
{
    return atof(this->c_str());
}

bool
OptionParser::ConvertibleString::asBool() const
{
    return atoi(this->c_str()) != 0;
}

// ****************************************************************************
// * OptionParser                                                             *
// ****************************************************************************

OptionParser::OptionParser(std::vector<ConvertibleString>* args,
                           std::map<std::string, ConvertibleString>* opts):
    _args(args), _opts(opts),
    _minArgs(0), _maxArgs(1000)
{
    assert(_args);
    assert(_opts);
    addFlag("help", "-h", "--help", "Show this help message and exit");
    addFlag("echo", "", "--echo-command", "Print command with options (usefull when logging)");
}

void
OptionParser::addOption(const std::string& dest,
                        const std::string& shortFlag,
                        const std::string& metavar,
                        const std::string& longFlag,
                        const std::string& description,
                        const std::string& defaultValue)
{
    using namespace std;

    stringstream opt;
    const char* sep = "";
    if(shortFlag.size()) {
        _flag2dest[shortFlag] = dest;
        _dest2flag[dest] = shortFlag;

        opt << shortFlag;
        if(metavar.size()) opt << "=" << metavar;
        sep = ", ";
    }
    if(longFlag.size()) {
        _flag2dest[longFlag] = dest;
        _dest2flag[dest] = longFlag;

        opt << sep << longFlag;
        if(metavar.size()) opt << "=" << metavar;
    }

    string desc(description);

    if(defaultValue.size() && !_boolFlags.count(dest)) {
        (*_opts)[dest] = defaultValue;
        replaceSubStr(desc, "%default", defaultValue);
    }

    // Format the help for this option
    vector<string> optFormatted = formatLines(opt.str(), OPTIONS_WIDTH, OPTIONS_LEFT_PADDING, false);
    vector<string> descFormatted = formatLines(desc, OPTDESC_WIDTH);
    vector<string> optdesc = mergeColumns(optFormatted, OPTIONS_WIDTH,
                                          descFormatted, OPTDESC_WIDTH, OPT_DESC_PADDING);

    for(vector<string>::iterator line = optdesc.begin(); line != optdesc.end(); line++) {
        _flagHelp << (*line) << "\n";
    }
}

void
OptionParser::addSection(const std::string& section)
{
    _flagHelp << "\n";
    for(int i = 0; i < OPTIONS_LEFT_PADDING; i++) _flagHelp << " ";
    _flagHelp << section << ":\n";
}

void
OptionParser::addFlag(const std::string& dest,
                      const std::string& shortFlag,
                      const std::string& longFlag,
                      const std::string& description,
                      bool valueWhenSet,
                      bool defaultValue)
{
    _boolFlags.insert(dest);
    addOption(dest, shortFlag, "", longFlag, description);
    (*_opts)[dest] = (defaultValue) ? "1" : "0";

    _optionFlag2value[shortFlag] = (valueWhenSet) ? "1" : "0";
    _optionFlag2value[longFlag] = (valueWhenSet) ? "1" : "0";
}

static
bool
isOption(const std::string& arg, std::string opt, bool& separatedByEqualSign)
{
    separatedByEqualSign = false;
    if(arg.compare(0, opt.size(), opt) == 0) {
        if(opt.size() < arg.size()) {
            if(arg[opt.size()] == '=') {
                separatedByEqualSign = true;
                return true;
            }
        }
        return true;
    }
    return false;
}

void
OptionParser::parse(int argc, const char* argv[])
{
    assert(_args);
    assert(_opts);

    _progName = std::string(argv[0]);

    // command for echo
    std::stringstream cmd;
    std::string sep = "";
    for (int i = 0; i < argc; i++) {
        cmd << sep << argv[i];
        sep = " ";
    }

    // Parsing
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            _args->push_back(argv[i]);
            continue;
        }

        bool foundOption = false;
        for(std::map<std::string, std::string>::iterator opt = _flag2dest.begin(); opt != _flag2dest.end(); opt++) {

            bool separatedByEqualSign;
            if(isOption(argv[i], opt->first, separatedByEqualSign)) {
                foundOption = true;
                if(_boolFlags.count(opt->second)) {
                    (*_opts)[opt->second] = _optionFlag2value[opt->first];
                } else {
                    if(separatedByEqualSign) {
                        (*_opts)[opt->second] = argv[i] + opt->first.size() + 1;
                    } else {
                        (*_opts)[opt->second] = argv[i + 1];
                        i++;
                    }
                }
                continue;
            }
        }

        if(!foundOption) {
            throw UnrecognizedOptionError(std::string(argv[i]));
        }
    }

    // If user asked for help at any point, then print help and exit
    if(atoi((*_opts)["help"].c_str())) {
        printHelp();
        exit(EXIT_SUCCESS);
        return;
    }

    // Did the user supply a valid number of arguments
    if(_args->size() < _minArgs || _args->size() > _maxArgs) {
        std::cerr << "ERROR: Invalid number of arguments supplied (" << _args->size() << " were given), run with -h for help" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(atoi((*_opts)["echo"].c_str())) {
        std::cout << cmd.str() << std::endl;
    }
}

void
OptionParser::setNArguments(int minArgs, int maxArgs)
{
    _minArgs = minArgs;
    _maxArgs = maxArgs;
}

void
OptionParser::printHelp() const
{
    using namespace std;

    const char* sep = "";
    if(_usage.size()) {
        cout << "USAGE:\n";
        for(int i = 0; i < _usage.size(); i++) {
            for(int j = 0; j < OPTIONS_LEFT_PADDING; j++) cout << " ";
            cout << _progName << " [OPTIONS] " << _usage[i] << "\n";
        }
        sep = "\n";
    }

    if(_description.size()) {
        cout << sep << "DESCRIPTION:\n";

        vector<string> desc = formatLines(_description, COLUMN_WIDTH, LEFT_PADDING);
        for(vector<string>::iterator descL = desc.begin(); descL != desc.end(); descL++) {
            cout << (*descL) << "\n";
        }

        sep = "\n";
    }

    cout << sep << "OPTIONS:\n"
         << _flagHelp.str();

    if( _copyright.size() ) {
        cout << "\nCopyright (C) " << _copyright << "\n";
    }
}

CLC_NAMESPACE_END
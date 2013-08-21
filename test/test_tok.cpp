#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <cassert>

#define PRINT_EXPR(expr) std::cout << #expr << " = " << (expr) << std::endl

static
std::vector<std::string>
breakIntoLines(const std::string& str_, int colWidth)
{
    const char* delims = " \n";

    char* str = new char[str_.size() + 1];
    strcpy(str, str_.c_str());

    std::stringstream reformated, line;
    std::vector<std::string> lines;
    for(char* token = strtok(str, delims); token != NULL; token = strtok(NULL, delims)) {
        if(line.str().size() + strlen(token) > colWidth) {
            while(line.str().size() <= colWidth) line << " ";
            reformated << line.str() << "|\n";
            lines.push_back(line.str());
            line.str("");
        }
        line << token << " ";
    }

    delete [] str;

    return lines;
}

static
std::vector<std::string>
mergeColumns(const std::vector<std::string>& lines1, const std::vector<std::string>& lines2, int colPadding = 0)
{
    int nLines = std::max(lines1.size(), lines2.size());

    assert(lines1.size());
    assert(lines2.size());

    int siz1 = lines1[0].size();
    int siz2 = lines2[0].size();

    std::vector<std::string> merged;
    for(int i = 0; i < nLines; i++) {
        std::stringstream line;

        if(i > (lines1.size() - 1)) {
            for(int j = 0; j < siz1; j++) line << " ";
        } else {
            line << lines1[i];
        }

        for(int j = 0; j < colPadding; j++) line << " ";

        if(i > (lines2.size() - 1)) {
            for(int j = 0; j < siz2; j++) line << " ";
        } else {
            line << lines2[i];
        }

        merged.push_back(line.str());
    }

    return merged;
}

int main(int argc, char const* argv[])
{
    std::string text("On a first call, the function expects a C string as\n argument for str, whose first character is used as the starting location to scan for tokens. In subsequent calls, the function expects a null pointer and uses the position right after the end of last token as the new starting location for scanning.");

    std::cout << "REFORMATED:" << std::endl;
    std::vector<std::string> lines = breakIntoLines(text, 30);
    for(int i = 0; i < lines.size(); i++) {
        std::cout << lines[i] << "|" << std::endl;
    }

    std::cout << "\nMERGED:" << std::endl;
    std::vector<std::string> lines2(lines);
    lines2.resize(2);
    std::vector<std::string> merged = mergeColumns(lines2, lines, 3);
    for(int i = 0; i < merged.size(); i++) {
        std::cout << merged[i] << "|" << std::endl;
    }

    return 0;
}
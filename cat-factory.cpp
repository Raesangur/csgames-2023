#include <iostream>
#include <string>

int main() {
    // Get prompt from console
    std::string line;
    std::cin >> line;

    // line for testing
    //line = "reqcahreetqttrcrtecatreqCAhreeTqTTrCrTeCAT";

    // remove all non-"c|a|t" characters
    std::string newline;
    for (char c : line)
    {
        if (c == 'c' || c == 'a' || c == 't')
            newline += c;
    }

    // Count number of occurences of the word "cat"
    int occurences = 0;
    std::string::size_type it = 0;
    while ((it = newline.find("cat", it)) != std::string::npos)
    {
        it += 3;
        occurences++;
    }

    std::cout << occurences;
}

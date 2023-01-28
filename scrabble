#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

std::array<int, 26> values = {
	1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10
};

class word
{
public:
	word(std::string w) : name{ w }
	{}
	bool check_match(std::string letters)
	{
		for (char c : name)
		{
			auto pos = letters.find(c, 0);
			if (pos == std::string::npos)
				return false;
			value += values[c - 'a'];
			letters.erase(pos, 1);
		}
		return possible = true;
	}

	std::string name;
	int value = 0;
	bool possible = false;
};

std::vector<std::string> example{
	"9",
	"because",
	"first",
	"these",
	"could",
	"which",
	"hichwq",
	"hicquwh"
};

int main() {
	std::vector<std::string> lines;
	std::string line;
	while (std::getline(std::cin, line)) {
		if (line.empty())
			break;
		lines.push_back(line);
	}
	//std::vector<std::string> lines = example;

	std::string letters = lines.back();
	lines.erase(lines.begin());
	lines.erase(lines.end() - 1);

	std::vector<word> words;
	for (std::string line : lines)
	{
		word newword{ line };
		if (newword.check_match(letters) == true)
			words.push_back(newword);
	}

	auto it = std::max_element(words.begin(), words.end(), [](word& l, word& r) {return l.value < r.value;  });
	std::cout << it->name;
}

#include <iostream>
#include <string>

int main() {
	std::string line;
	std::cin >> line;

	//line = "reqcahreetqttrcrtecatreqCAhreeTqTTrCrTeCAT";

	std::string newline;
	for (char c : line)
	{
		if (c == 'c' || c == 'a' || c == 't')
			newline += c;
	}

	int occurences = 0;
	std::string::size_type it = 0;
	while ((it = newline.find("cat", it)) != std::string::npos)
	{
		it += 3;
		occurences++;
	}

	std::cout << occurences;
}

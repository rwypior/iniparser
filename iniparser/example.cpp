#include "iniparser.hpp"

#include <iostream>

int main()
{
	Ini::Model model;
	Ini::Result resultExample = Ini::Parser().readFile("../resources/example.ini", model);
	std::string dataExample = model.sections["section"]["var"]->getValue();
	auto arrayDataExample = model.sections["array"]["nums"]->getVector();

	const auto &vecnested = model.sections["array"]["nested"]->getVector()->entries;

	auto a = vecnested[0]->getVector()->entries[0]->getValue();
	auto b = vecnested[0]->getVector()->entries[1]->getValue();
	auto c = vecnested[1]->getVector()->entries[0]->getValue();
	auto d = vecnested[1]->getVector()->entries[1]->getValue();
	
	return 0;
}

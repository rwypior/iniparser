#include "iniparser.hpp"

#include <iostream>

void readIni()
{
	Ini::Model model;
	Ini::Result resultExample = Ini::Parser().readFile("../resources/example.ini", model);

	std::cout << model.entries["int_variable"].toInt() << std::endl;
	std::cout << "Not-existing int entry: " << model.entries["non_existent_int"].toInt() << std::endl;
	std::cout << "Not-existing bool entry: " << model.entries["non_existent_bool"].toBool(false) << std::endl;

	auto& section = model.sections["section"];
	for (const auto& val : section)
	{
		std::cout << val.second->getValue() << std::endl;
	}

	const Ini::Section& constSection = model.sections["windows"];
	//std::cout << "Not-existing section int entry: " << constSection["non_existent_section_int"].toInt() << std::endl;

	Ini::Section& xxx = model.sections["asd"];
	Ini::VectorEntry* arrayDataExample = model.sections["array"]["nums"].getVector();

	Ini::Entry& vecnested = model.sections["array"]["nested"];
	Ini::VectorEntry& vecnestedvalues = *vecnested.getVector();

	std::cout << (*vecnestedvalues[0].getVector())[0].getValue() << std::endl;
	std::cout << (*vecnestedvalues[0].getVector())[1].getValue() << std::endl;
	std::cout << (*vecnestedvalues[1].getVector())[0].getValue() << std::endl;
	std::cout << (*vecnestedvalues[1].getVector())[1].getValue() << std::endl;
}

void writeIni()
{
	Ini::Model model;

	model.entries.insert("test", "bla");
	model.entries.insert("second", "42");
	model.entries.insert("entry_3", "qwerty");

	std::string aaa = std::to_string(1337);
	model.entries.insert("some_integer", 1337);
	model.entries.insert("some_bool", true);
	model.entries.insert("some_float", 13.37);
	model.entries.insert("some_double", 13.37654321);

	model.entries.insertVector("vector_entry", {"aaa", "bbb", "ccc"});

	Ini::VectorEntry &vec = model.entries.insertVector("vector_entry_2");
	vec.append(1);
	vec.append(2);
	vec.append(3);
	vec.append(Ini::VectorEntry({"b", "l", "a", "h"}));

	Ini::EntryContainer &someSection = model.appendSection("some_section");
	someSection["aaa"] = "qqqq";
	someSection["bbb"] = "wwww";
	someSection["ccc"] = "eeee";

	Ini::EntryContainer& anotherSection = model.appendSection("another_section");
	anotherSection["s1a"] = "abcdef";

	Ini::Section& sectionxxx = model.sections["xxx"];
	sectionxxx["ttt"] = 123;
	sectionxxx["ttt"] = 567;

	Ini::Parser().writeFile("../resources/created.ini", model);
}

int main()
{
	readIni();
	writeIni();
	
	return 0;
}

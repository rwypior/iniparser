#include <iniparser.hpp>

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

SCENARIO("Standard file can be parsed", "[ini]")
{
	GIVEN("Parser is created")
	{
		Ini::Parser parser;

		AND_GIVEN("A correct .ini file")
		{
			std::string path = "../resources/example.ini";

			WHEN("File is read")
			{
				Ini::Model model;
				Ini::Result res = parser.readFile(path, model);

				THEN("Result is successful")
				{
					REQUIRE(res.code == Ini::Result::Code::OK);
				}

				AND_THEN("Results are read correctly")
				{
					REQUIRE(model.entries["int_variable"].toString() == "42");
					REQUIRE(model.entries["string_variable"].toString() == "hello");
					REQUIRE(model.entries["string_variable2"].toString() == "h3ll0");
					REQUIRE(model.entries["bool_value"].toBool() == true);
					REQUIRE(model.entries["with_equal_characters"].toString() == "one=two=three");
					REQUIRE(model.entries["in_quoration"].toString() == "qwerty;uiop=1=2=3");
					REQUIRE(model.entries["outside_quoration"].toString() != "qwerty;uiop=1=2=3");
					REQUIRE(model.entries["multiline_quotation"].toString() != "first\nsecond\nthird");

					REQUIRE(model.sections["section"]["var"].toInt() == 1337);

					REQUIRE(model.sections["another"]["@using-odd_characters"].getValue() == "Hello world!");

					const auto &vec = model.sections["array"]["nums"].getVector()->entries;

					REQUIRE(vec[0]->getValue() == "1");
					REQUIRE(vec[1]->getValue() == "2");
					REQUIRE(vec[2]->getValue() == "3");

					const auto &vecnested = model.sections["array"]["nested"].getVector()->entries;

					REQUIRE(vecnested[0]->getVector()->entries[0]->getValue() == "1");
					REQUIRE(vecnested[0]->getVector()->entries[1]->getValue() == "2");
					REQUIRE(vecnested[1]->getVector()->entries[0]->getValue() == "a");
					REQUIRE(vecnested[1]->getVector()->entries[1]->getValue() == "b");
				}
			}
		}
	}
}
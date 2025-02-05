#include <iniparser/iniparser.hpp>

#include <catch2/catch_all.hpp>

SCENARIO("Including iniparser in two files wont give linker errors", "[ini]")
{
	GIVEN("Parser is included and created")
	{
		Ini::Parser parser;

		AND_GIVEN("A correct .ini file")
		{
			std::string path = "resources/example.ini";

			WHEN("File is read")
			{
				Ini::Model model;
				Ini::Result res = parser.readFile(path, model);

				THEN("Result is successful and no linker errors are thrown")
				{
					REQUIRE(res.code == Ini::Result::Code::OK);
				}
			}
		}
	}
}
#ifndef iniparser_h
#define iniparser_h

#include <fstream>
#include <string>
#include <unordered_map>
#include <memory>

namespace Ini
{
	using ValueType = std::string;
	struct RegularEntry;
	struct VectorEntry;

	struct Entry
	{
		virtual ~Entry() = default;

		ValueType getValue();
		VectorEntry *getVector();
	};

	using EntryType = std::unique_ptr<Entry>;

	struct RegularEntry : public Entry
	{
		ValueType value;

		RegularEntry(const ValueType &val)
			: value(val)
		{
		}
	};

	struct VectorEntry : public Entry
	{
		std::vector<EntryType> entries;
	};

	ValueType Entry::getValue()
	{
		return static_cast<RegularEntry*>(this)->value;
	}

	VectorEntry *Entry::getVector()
	{
		return static_cast<VectorEntry*>(this);
	}
	
	using EntriesContainer = std::unordered_map<std::string, EntryType>;
	using SectionsContainer = std::unordered_map<std::string, EntriesContainer>;

	struct Model
	{
		EntriesContainer entries;
		SectionsContainer sections;
	};

	struct Result
	{
		enum class Code
		{
			OK,
			Unreadable,
			SyntaxError
		} code;
		std::string details;

		Result(Code c)
			: code(c)
		{
		}

		Result(Code c, const std::string &details)
			: code(c)
			, details(details)
		{
		}
	};

	class Parser
	{
	public:
		inline Result parse(const std::string &source, Model &model)
		{
			bool isSection = false;
			bool isComment = false;
			bool isEscape = false;
			bool isValue = false;
			bool isNewLine = true;
			unsigned int arrayLevel = 0; // 0 - not in array

			std::string entryName;
			std::string buffer;

			EntriesContainer *ec = nullptr;

			for (char c : source)
			{
				if (isComment)
				{
					if (c == '\n')
						isComment = false;
					continue;
				}

				if (isEscape)
				{
					buffer.push_back(escapeChar(c));
					isEscape = false;
				}

				switch (c)
				{
				case '\\':
					isEscape = true;
					break;

				case '[':
					if (isNewLine)
						isSection = true;
					else
					{
						if (arrayLevel > 0)
							buffer.push_back(c);
						++arrayLevel;
					}
					break;

				case ']':
					if (isSection)
					{
						model.sections[buffer] = EntriesContainer();
						ec = &model.sections[buffer];
						isSection = false;
						buffer.clear();
					}
					else if (arrayLevel)
					{
						if (--arrayLevel == 0)
						{
							pushVectorEntry(ec, model, entryName, buffer);
							buffer.clear();
							entryName.erase();
						}
						else
							buffer.push_back(c);
					}
					break;

				case '=':
					trim(buffer);
					entryName = buffer;
					buffer.clear();
					break;

				case ';':
					isComment = true;
					break;

				case '\n':
					if (arrayLevel == 0)
					{
						if (isComment)
						{
							isComment = false;
						}
						else
						{
							trim(buffer);
							if (!buffer.empty())
							{
								pushEntry(ec, model, entryName, buffer);
								buffer.clear();
								entryName.erase();
							}
						}
					}
					else
						buffer.push_back(c);
					break;

				default:
					buffer.push_back(c);
					break;
				}

				isNewLine = c == '\n';
			}

			if (!entryName.empty())
				pushEntry(ec, model, entryName, buffer);

			return Result::Code::OK;
		}

		inline Result readFile(const std::string &path, Model &model)
		{
			std::ifstream str(path, std::ios_base::in | std::ios_base::ate);
			if (str.is_open())
			{
				std::streamoff size = str.tellg();
				str.seekg(0, std::ios_base::beg);
				std::string s(static_cast<size_t>(size), 0x00);
				str.read(&s[0], size);
				str.close();

				return parse(s, model);
			}

			return Result::Code::Unreadable;
		}

	private:
		inline char escapeChar(char c)
		{
			switch (c)
			{
			case 't': return '\t';
			case 'n': return '\n';
			default: return c;
			}
		}
		
		inline void rtrim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			s.erase(s.find_last_not_of(t) + 1);
		}

		inline void ltrim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			s.erase(0, s.find_first_not_of(t));
		}

		inline void trim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			rtrim(s, t);
			ltrim(s, t);
		}

		inline EntryType makeEntry(const std::string &text)
		{
			return EntryType(new RegularEntry(text));
		}

		inline void pushEntry(EntriesContainer *ec, Model &model, const std::string &entryName, const std::string entry)
		{
			if (ec)
				(*ec)[entryName] = makeEntry(entry);
			else
				model.entries[entryName] = makeEntry(entry);
		}

		inline std::unique_ptr<VectorEntry> makeVectorEntry(const std::string &text)
		{
			unsigned int level = 0;
			std::string buffer;

			VectorEntry *v = new VectorEntry();

			for (char c : text)
			{
				switch (c)
				{
				case ',':
					if (level == 0)
					{
						if (!buffer.empty())
						{
							trim(buffer);
							v->entries.push_back(makeEntry(buffer));
							buffer.clear();
						}
					}
					else
						buffer.push_back(c);
					break;

				case '[':
					if (level > 0)
						buffer.push_back(c);
					++level;
					break;

				case ']':
					if (--level == 0)
					{
						trim(buffer);
						v->entries.push_back(makeVectorEntry(buffer));
						buffer.clear();
					}
					else
						buffer.push_back(c);
					break;

				default:
					buffer.push_back(c);
					break;
				}
			}

			trim(buffer);
			if (!buffer.empty())
			{
				if (buffer.back() == ']')
					v->entries.push_back(makeVectorEntry(buffer));
				else
					v->entries.push_back(makeEntry(buffer));
			}

			return std::unique_ptr<VectorEntry>(v);
		}

		inline void pushVectorEntry(EntriesContainer *ec, Model &model, const std::string &entryName, const std::string entry)
		{
			if (ec)
				(*ec)[entryName] = makeVectorEntry(entry);
			else
				model.entries[entryName] = makeVectorEntry(entry);
		}
	};
}

#endif

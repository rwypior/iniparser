#ifndef iniparser_h
#define iniparser_h

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <algorithm>

namespace Ini
{
	struct ScalarEntry;
	struct VectorEntry;

	struct Entry
	{
		Entry() = default;
		virtual ~Entry() = default;

		std::string getValue();
		VectorEntry *getVector();
		const VectorEntry* getVector() const;

		bool empty() const { return this->toString().empty(); }

		virtual bool isVector() const = 0;
		virtual std::string toString() const = 0;
		virtual int toInt(int def = 0) const { return empty() ? def : std::stoi(this->toString()); }
		virtual unsigned int toUInt(unsigned int def = 0) const { return empty() ? def : std::stoul(this->toString()); }
		virtual long toLong(long def = 0) const { return empty() ? def : std::stol(this->toString()); }
		virtual unsigned long toULong(unsigned long def = 0) const { return empty() ? def : std::stoul(this->toString()); }
		virtual float toFloat(float def = 0.0f) const { return empty() ? def : std::stof(this->toString()); }
		virtual double toDouble(double def = 0.0) const { return empty() ? def : std::stod(this->toString()); }
		virtual bool toBool(bool def = false) const { return empty() ? def : (this->toString() == "true" || this->toInt()); }

		virtual Entry& operator=(const std::string &s) { return *this; };
		virtual Entry& operator=(const char *s) { return *this; };
		virtual Entry& operator=(int val) { return *this; };
		virtual Entry& operator=(unsigned int val) { return *this; };
		virtual Entry& operator=(long val) { return *this; };
		virtual Entry& operator=(unsigned long val) { return *this; };
		virtual Entry& operator=(float val) { return *this; };
		virtual Entry& operator=(double val) { return *this; };
		virtual Entry& operator=(bool val) { return *this; };
	};

	struct ScalarEntry : public Entry
	{
		std::string value;

		ScalarEntry() = default;

		ScalarEntry(const std::string&val)
			: value(val)
		{
		}

		ScalarEntry(const char *val)
			: ScalarEntry(std::string(val))
		{
		}

		template<typename T>
		ScalarEntry(const T& val)
			: value(std::to_string(val))
		{
		}

		virtual bool isVector() const override
		{
			return false;
		}

		virtual std::string toString() const override
		{
			return this->value;
		}

		virtual Entry& operator=(const std::string& s) override { this->value = s; return *this; };
		virtual Entry& operator=(const char *s) override { this->value = s; return *this; };
		virtual Entry& operator=(int val) { this->value = std::to_string(val); return *this; };
		virtual Entry& operator=(unsigned int val) { this->value = std::to_string(val); return *this; };
		virtual Entry& operator=(long val) { this->value = std::to_string(val); return *this; };
		virtual Entry& operator=(unsigned long val) { this->value = std::to_string(val); return *this; };
		virtual Entry& operator=(float val) { this->value = std::to_string(val); return *this; };
		virtual Entry& operator=(double val) { this->value = std::to_string(val); return *this; };
		virtual Entry& operator=(bool val) { this->value = std::to_string(val); return *this; };
	};

	struct VectorEntry : public Entry
	{
		using Container = std::vector<std::unique_ptr<Entry>>;
		Container entries;

		VectorEntry() = default;

		VectorEntry(VectorEntry&& b)
			: entries(std::move(b.entries))
		{
		}

		template<typename T>
		VectorEntry(const std::initializer_list<T> &entries)
		{
			for (const auto& entry : entries)
			{
				this->entries.push_back(std::make_unique<ScalarEntry>(entry));
			}
		}

		virtual bool isVector() const override
		{
			return true;
		}

		Container::size_type size() const
		{
			return this->entries.size();
		}

		virtual std::string toString() const override
		{
			std::string res = "[";
			bool first = true;

			for (const std::unique_ptr<Entry>& entry : this->entries)
			{
				if (!first)
					res += ", ";

				res += entry->toString();

				first = false;
			}

			res += "]";

			return res;
		}

		template<typename T>
		void append(const T& entry)
		{
			this->entries.push_back(std::make_unique<ScalarEntry>(entry));
		}

		void append(std::unique_ptr<Entry> &&entry)
		{
			this->entries.push_back(std::move(entry));
		}

		void append(VectorEntry&& entry)
		{
			this->entries.push_back(std::make_unique<VectorEntry>(std::move(entry)));
		}

		Container::iterator begin() { return this->entries.begin(); }
		Container::iterator end() { return this->entries.end(); }
		Container::const_iterator begin() const { return this->entries.begin(); }
		Container::const_iterator end() const { return this->entries.end(); }

		Entry& operator[](Container::size_type idx) { return *this->entries[idx]; }
		const Entry& operator[](Container::size_type idx)const  { return *this->entries[idx]; }
	};

	inline std::string Entry::getValue()
	{
		return static_cast<ScalarEntry*>(this)->value;
	}

	inline VectorEntry *Entry::getVector()
	{
		return static_cast<VectorEntry*>(this);
	}

	inline const VectorEntry* Entry::getVector() const
	{
		return static_cast<const VectorEntry*>(this);
	}
	
	struct EntryContainer
	{
		using Container = std::unordered_map<std::string, std::unique_ptr<Entry>>;
		Container entries;

		template<typename T>
		ScalarEntry& insert(const std::string& name, const T& value)
		{
			this->entries[name] = std::make_unique<ScalarEntry>(value);
			return static_cast<ScalarEntry&>(*this->entries[name]);
		}

		template<typename T>
		VectorEntry& insertVector(const std::string& name, const std::initializer_list<T>& value)
		{
			this->entries[name] = std::make_unique<VectorEntry>(value);
			return static_cast<VectorEntry&>(*this->entries[name]);
		}

		VectorEntry& insertVector(const std::string& name)
		{
			this->entries[name] = std::make_unique<VectorEntry>();
			return static_cast<VectorEntry&>(*this->entries[name]);
		}

		Container::iterator begin() { return this->entries.begin(); }
		Container::iterator end() { return this->entries.end(); }
		Container::const_iterator begin() const { return this->entries.begin(); }
		Container::const_iterator end() const { return this->entries.end(); }

		bool contains(const std::string& idx) const { return this->entries.find(idx) != this->entries.end(); }

		Entry& operator[](const std::string &idx)
		{
			if (this->entries.find(idx) == this->entries.end())
				this->entries[idx] = std::make_unique<ScalarEntry>();
			return *this->entries[idx];
		}

		const Entry& operator[](const std::string& idx) const
		{
			assert(this->contains(idx) && "Index not found");
			return *this->entries.at(idx);
		}
		const Entry& at(const std::string& idx) const { return (*this)[idx]; }
	};

	struct SectionContainer
	{
		using Container = std::unordered_map<std::string, EntryContainer>;
		Container sections;

		Container::iterator begin() { return this->sections.begin(); }
		Container::iterator end() { return this->sections.end(); }
		Container::const_iterator begin() const { return this->sections.begin(); }
		Container::const_iterator end() const { return this->sections.end(); }

		Container::iterator find(const std::string& idx) { return this->sections.find(idx); };
		bool contains(const std::string& idx) const { return this->sections.find(idx) != this->sections.end(); }

		EntryContainer& operator[](const std::string& idx)
		{
			if (this->sections.find(idx) == this->sections.end())
				this->sections[idx] = EntryContainer();
			return this->sections[idx];
		}

		const EntryContainer& operator[](const std::string& idx) const
		{
			assert(this->contains(idx) && "Index not found");
			return this->sections.at(idx);
		}
		const EntryContainer& at(const std::string& idx) const { return (*this)[idx]; }
	};

	using Section = EntryContainer;

	struct Model
	{
		SectionContainer sections;
		EntryContainer entries;

		EntryContainer& appendSection(const std::string& name)
		{
			this->sections[name] = EntryContainer();
			return this->sections[name];
		}

		bool hasSection(const std::string& name)
		{
			return this->sections.find(name) != this->sections.end();
		}
	};

	struct Result
	{
		enum class Code
		{
			OK,
			Unreadable,
			Unwritable,
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
		Result parse(const std::string &source, Model &model)
		{
			bool isSection = false;
			bool isComment = false;
			bool isEscape = false;
			bool isValue = false;
			bool isNewLine = true;
			bool isInQuote = false;
			unsigned int arrayLevel = 0; // 0 - not in array

			std::string entryName;
			std::string buffer;

			EntryContainer*ec = nullptr;

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

				if (isInQuote && c != '"')
				{
					buffer.push_back(c);
					continue;
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
						model.sections[buffer] = EntryContainer();
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
					if (entryName.empty())
					{
						trim(buffer);
						entryName = buffer;
						buffer.clear();
					}
					else
						buffer.push_back(c);
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

				case '"':
					if (isInQuote)
					{
						isInQuote = false;
					}
					else
					{
						if (std::all_of(buffer.begin(), buffer.end(), isspace))
						{
							isInQuote = true;
						}
					}
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

		Result readFile(const std::string &path, Model &model)
		{
			std::ifstream str(path, std::ios_base::in | std::ios_base::ate | std::ios_base::binary);
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

		Result writeFile(const std::string &path, Model& model)
		{
			bool first = true;
			std::ofstream str(path, std::ios_base::out | std::ios_base::binary);
			if (str.is_open())
			{
				for (const auto &entry : model.entries)
				{
					std::string key = entry.first;
					std::string value = entry.second->toString();

					str << key << " = " << value << "\n";

					first = false;
				}

				for (const auto& section : model.sections)
				{
					std::string key = section.first;
					const auto &entries = section.second;

					std::string line = "[" + key + "]" + "\n";
					if (!first)
						line = "\n" + line;

					str << line;

					for (const auto& entry : entries)
					{
						std::string key = entry.first;
						std::string value = entry.second->toString();

						str << key << " = " << value << "\n";
					}

					first = false;
				}

				return Result::Code::OK;
			}

			return Result::Code::Unwritable;
		}

	private:
		char escapeChar(char c)
		{
			switch (c)
			{
			case 't': return '\t';
			case 'n': return '\n';
			default: return c;
			}
		}
		
		void rtrim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			s.erase(s.find_last_not_of(t) + 1);
		}

		void ltrim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			s.erase(0, s.find_first_not_of(t));
		}

		void trim(std::string& s, const char* t = " \t\n\r\f\v")
		{
			rtrim(s, t);
			ltrim(s, t);
		}

		std::unique_ptr<Entry> makeEntry(const std::string &text)
		{
			return std::make_unique<ScalarEntry>(text);
		}

		void pushEntry(EntryContainer*ec, Model &model, const std::string &entryName, const std::string entry)
		{
			if (ec)
				ec->entries[entryName] = makeEntry(entry);
			else
				model.entries.entries[entryName] = makeEntry(entry);
		}

		std::unique_ptr<VectorEntry> makeVectorEntry(const std::string &text)
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

		void pushVectorEntry(EntryContainer*ec, Model &model, const std::string &entryName, const std::string entry)
		{
			if (ec)
				ec->entries[entryName] = makeVectorEntry(entry);
			else
				model.entries.entries[entryName] = makeVectorEntry(entry);
		}
	};
}

#endif

#include "ini.h"

namespace tfc
{
	namespace data
	{
		namespace ini
		{
			INISection::INIItemIterator INISection::begin()
			{
				return items.begin();
			}

			INISection::INIItemIterator INISection::end()
			{
				return items.end();
			}

			bool INISection::hasKey(std::string key)
			{
				return false;
			}

			size_t INISection::length()
			{
				return items.size();
			}

			std::vector<INIItem> INISection::getItems()
			{
				return items;
			}

			std::vector<std::pair<std::string, std::string>> INISection::getEntries()
			{
				std::vector<std::pair<std::string, std::string>> entries;
				for (INISection::INIItemIterator i = this->begin(); i != this->end(); ++i)
				{
					std::pair<std::string, std::string> entry;
					entry.first = i->key;
					entry.second = i->value;
					entries.push_back(entry);
				}
				return entries;
			}

		};
	};
};

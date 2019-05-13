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

			std::string INISection::getName()
			{
				return name;
			}

			void INISection::setName(std::string _name)
			{
				debug()
				name = _name;
			}

			std::string INISection::getComment()
			{
				return comment;
			}

			void INISection::setComment(std::string _comment)
			{
				comment = _comment;
			}

			std::string INISection::getRightComment()
			{
				return rightComment;
			}

			void INISection::setRightComment(std::string _rightComment)
			{
				rightComment = _rightComment;
			}

			std::vector<INIItem> INISection::getItems()
			{
				return items;
			}

			void INISection::pushItem(INIItem item)
			{
				items.push_back(item);
				std::cout << "------ items.push_back() ------ \n" << items.empty() << std::endl;
			}

			INISection::INIItemIterator INISection::eraseItem(INIItemIterator item)
			{
				return items.erase(item);
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

			std::vector<std::string> INISection::getKeys()
			{
				std::cout << this->items.size() << std::endl;
				std::vector<std::string> keys;
				for (INISection::INIItemIterator i = this->begin(); i != this->end(); ++i)
				{
					keys.push_back(i->key);
				}
				return keys;
			}

			std::vector<std::string> INISection::getValues()
			{
				std::vector<std::string> values;
				for (INISection::INIItemIterator i = this->begin(); i != this->end(); ++i)
				{
					values.push_back(i->value);
				}
				return values;
			}

		};
	};
};

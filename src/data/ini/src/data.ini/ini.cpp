#include "ini.h"

namespace tfc
{
	namespace data
	{
		namespace ini
		{
			void INIFile::customBooleans(std::string b0, std::string b1)
			{
				beBooleans.push_back({ b0, b1 });
			}


			ININativeValue INIFile::getValue(std::string section, std::string key)
			{
				INISection sect;
				ININativeValue nativeValue;

				try
				{
					sect = getSection(section);
				}
				catch (INIException& e)
				{
					throw INIException(e.errtype(), e.info());
				}

				for (INISection::INIItemIterator it = sect.begin(); it != sect.end(); ++it) {
					if (it->key == key)
					{
						nativeValue.value = it->value;
						nativeValue.comment = it->comment;
						return nativeValue;
					}
				}

				throw INIException(ERR_NOT_FOUND_KEY, std::string("not found key `") + key + "`");
			}


			void INIFile::setValue(std::string section, std::string key, std::string value, std::string comment)
			{
				INISection sect;
				std::string comt = comment;

				if (comt != "")
				{
					comt = '\n' + comt;
				}

				try
				{
					sect = getSection(section);
				}
				catch (const std::exception&)
				{

					//如果段不存在，新建一个
					try
					{
						sect = INISection();
					}
					catch (const std::exception&)
					{
						throw INIException(ERR_NO_ENOUGH_MEMORY, "no enough memory");
					}
					

					sect.setName(section);
					if (sect.getName() == "")
					{

						// 确保空section始终位于首位
						sectionsCache.insert(sectionsCache.begin(), sect);
					}
					else
					{
						sectionsCache.push_back(sect);
					}
				}
			}


			void INIFile::setValue(std::string section, std::string key, std::string value)
			{
				setValue(section, key, value, "");
			}


			void INIFile::setValue(std::string key, std::string value)
			{
				setValue("", key, value);
			}


			INISection INIFile::updateSection(std::string cleanLine, std::string comment, std::string rightComment)
			{
				INISection newSection;

				// 查找右中括号
				size_t index = cleanLine.find_first_of(']');

				if (index == std::string::npos) {
					throw INIException(ERR_UNMATCHED_BRACKETS, std::string("no matched ] found"));
				}

				int len = index - 1;

				// 若段名为空，继续下一行
				if (len <= 0) {
					throw INIException(ERR_SECTION_EMPTY, std::string("section name is empty"));
				}

				// 取段名
				std::string s(cleanLine, 1, len);
				trim(s);

				// 段名不允许为空

				if (s == "")
				{
					throw INIException(ERR_SECTION_ALREADY_EXISTS, "invalid empty section name");
				}

				//检查段是否已存在
				if (getSection(s).getName() != "") {
					throw INIException(ERR_SECTION_ALREADY_EXISTS, std::string("section ") + s + std::string("already exist"));
				}

				// 填充段名
				newSection.setName(s);

				// 填充段开头的注释
				newSection.setComment(comment);
				newSection.setRightComment(rightComment);
				sectionsCache.push_back(newSection);
				return newSection;
			}


			int INIFile::addEntry(std::string cleanLine, std::string comment, std::string rightComment, INISection currSection)
			{
				INIItem item = parse(cleanLine);

				item.comment = comment;
				item.rightComment = rightComment;

				currSection.pushItem(item);
				return 0;
			}


			INIItem INIFile::parse(std::string line)
			{
				INIItem item;
				auto pair = split(line, "=");
				item.key = pair.first;
				item.value = pair.second;
				return item;
			}


			std::pair<std::string, std::string> INIFile::split(std::string line, std::string sep)
			{
				size_t pos = line.find(sep);
				std::string left, right;
				std::pair<std::string, std::string> pair;

				if (pos != std::string::npos)
				{
					left = std::string(line, 0, pos);
					right = std::string(line, pos + 1);

					trim(left);
					trim(right);

					pair.first = left;
					pair.second = right;
				}
				else
				{
					left = line;
					right = "";

					trim(left);

					pair.first = left;
					pair.second = right;
				}
				return pair;
			}


			bool INIFile::isCommentLine(std::string line)
			{
				return startWith(line, "#") || startWith(line, ";");
			}


			std::string INIFile::trimLeft(std::string line, char c)
			{
				int len = line.length();
				int i = 0;

				while (line[i] == c && line[i] != '\0')
				{
					i++;
				}

				if (i != 0)
				{
					return std::string(line, i, len - i);
				}
				return line;
			}


			std::string INIFile::trimRight(std::string line, char c)
			{
				int i = 0;
				int len = line.length();

				for (i = len - 1; i >= 0; --i)
				{
					if (line[i] != c)
					{
						break;
					}
				}
				return std::string(line, 0, i + 1);
			}


			std::string INIFile::trim(std::string line, char c)
			{
				int len = line.length();
				int i = 0;

				while ((i < len) && isspace(line[i]) && (line[i] != '\0'))
				{
					i++;
				}

				if (i != 0)
				{
					line = std::string(line, i, len - i);
				}

				len = line.length();
				for (i = len - 1; i >= 0; --i)
				{
					if (!isspace(line[i]))
					{
						break;
					}
				}
				return std::string(line, 0, i + 1);
			}


			bool INIFile::startWith(std::string line, std::string prefix)
			{
				if (strncmp(line.c_str(), prefix.c_str(), prefix.size()) == 0)
				{
					return true;
				}
				return false;
			}


			bool INIFile::compareStringIgnoreCase(std::string first, std::string second)
			{
				std::string a = first;
				std::string b = second;

				transform(a.begin(), a.end(), a.begin(), towupper);
				transform(b.begin(), b.end(), b.begin(), towupper);

				return a == b;
			}


			void INIFile::release()
			{
				for (auto i = sectionsCache.begin(); i != sectionsCache.end(); ++i)
				{
					delete (*i);
				}
				sectionsCache.clear();
				path = "";
			}


			INIFile::INIFile()
			{
				beBooleans.push_back({ "true", "false" });
				beBooleans.push_back({ "yes", "no" });
				beBooleans.push_back({ "on", "off" });
				beBooleans.push_back({ "0", "1" });
			}


			INIFile::~INIFile()
			{
				release();
			}


			int INIFile::load(std::string filePath)
			{
				int errorValue;
				std::string line;       // 带注释的行
				std::string cleanLine;  // 去掉注释后的行
				std::string comment;
				std::string rightComment;
				INISection currSection;  // 初始化一个字段指针

				release();

				path = filePath;
				std::ifstream ifs(path);
				if (!ifs.is_open()) {
					return ERR_OPEN_FILE_FAILED;
				}

				//增加默认段，即 无名段""
				currSection.setName("");
				sectionsCache.push_back(currSection);

				// 每次读取一行内容到line
				while (std::getline(ifs, line)) {
					trim(line);

					// step 0，空行处理，如果长度为0，说明是空行，添加到comment，当作是注释的一部分
					if (line.length() <= 0) {
						comment += '\n';
						continue;
					}

					// step 1
					// 如果行首不是注释，查找行尾是否存在注释
					// 如果该行以注释开头，添加到comment，跳过当前循环，continue
					if (isCommentLine(line)) {
						comment += line + '\n';
						continue;
					}

					// 如果行首不是注释，查找行尾是否存在注释，若存在，切割该行，将注释内容添加到rightComment
					auto commentContent = split(line, "#");
					cleanLine = commentContent.first;
					rightComment = commentContent.second;

					// step 2，判断line内容是否为段或键
					//段开头查找 [
					if (cleanLine[0] == '[') {
						try
						{
							updateSection(cleanLine, comment, rightComment);
							errorValue = RET_OK;
						}
						catch (INIException& e)
						{
							errorValue = e.errtype();
						}
					}
					else {

						// 如果该行是键值，添加到section段的items容器
						errorValue = addEntry(cleanLine, comment, rightComment, currSection);
					}

					// comment清零
					comment = "";
					rightComment = "";
				}

				ifs.close();
				return errorValue;
			}


			int INIFile::save(std::string)
			{
				return saveAs(path);
			}


			int INIFile::saveAs(std::string)
			{
				std::string data = "";

				// 载入section数据
				for (INISectionIterator sect = sectionsCache.begin(); sect != sectionsCache.end(); ++sect)
				{
					if (sect->getComment() != "")
					{
						data += sect->getComment();
					}

					if (sect->getName() != "")
					{
						data += std::string("[") + sect->getName() + std::string("]");
						data += '\n';
					}

					if (sect->getRightComment() != "")
					{
						data += " #" + sect->getRightComment();
					}

					// 载入 item 数据
					for (INISection::INIItemIterator item = sect->getItems().begin(); item != sect->getItems().end(); ++item)
					{
						if (item->comment != "")
						{
							data += item->comment;
							if (data[data.length() - 1] != '\n')
							{
								data += '\n';
							}
						}

						data += item->key + "=" + item->value;

						if (item->rightComment != "")
						{
							data += " #" + item->rightComment;
						}

						if (data[data.length() - 1] != '\n')
						{
							data += '\n';
						}
					}
				}

				std::ofstream ofs(path);
				ofs << data;
				ofs.close();
				return 0;
			}


			std::string INIFile::getStringValue(std::string section, std::string key)
			{
				return getValue(section, key).value;
			}


			std::string INIFile::getStringValue(std::string key)
			{
				return getValue("", key).value;
			}


			std::string INIFile::getStringValueOrDefault(std::string section, std::string key, std::string defaultValue)
			{
				std::string s = getStringValue(section, key);
				if (s == "")
				{
					return defaultValue;
				}
				return s;
			}

			std::string INIFile::getStringValueOrDefault(std::string key, std::string defaultValue)
			{
				return getStringValueOrDefault("", key, defaultValue);
			}


			int INIFile::getIntValue(std::string section, std::string key)
			{
				return atoi(getValue(section, key).value.data());
			}


			int INIFile::getIntValue(std::string key)
			{
				return getIntValue("", key);
			}

			int INIFile::getIntValueOrDefault(std::string section, std::string key, int defaultValue)
			{
				return getIntValue(section, key) || defaultValue;
			}

			int INIFile::getIntValueOrDefault(std::string key, int defaultValue)
			{
				return getIntValueOrDefault("", key, defaultValue);
			}


			double INIFile::getDoubleValue(std::string section, std::string key)
			{
				return atof(getValue(section, key).value.data());
			}


			double INIFile::getDoubleValue(std::string key)
			{
				return getDoubleValue("", key);
			}

			double INIFile::getDoubleValueOrDefault(std::string section, std::string key, double defaultValue)
			{
				return getDoubleValue(section, key) || defaultValue;
			}

			double INIFile::getDoubleValueOrDefault(std::string key, double defaultValue)
			{
				return getDoubleValueOrDefault("", key, defaultValue);
			}


			bool INIFile::getBoolValue(std::string section, std::string key)
			{
				std::string value;
				
				getValue(section, key).value;
				for (auto i = beBooleans.begin(); i != beBooleans.end(); ++i)
				{
					if (value == i->first)
					{
						return true;
					}
					else if (value == i->second)
					{
						return false;
					}
				}
				throw INIException(ERR_PARSE_KEY_VALUE_FAILED, value + "is not an expected boolean value");
			}


			bool INIFile::getBoolValue(std::string key)
			{
				return getBoolValue("", key);
			}

			bool INIFile::getBoolValueOrDefault(std::string section, std::string key, bool defaultValue)
			{
				bool value;
				try
				{
					value = getBoolValue(section, key);
				}
				catch (INIException& e)
				{
					return defaultValue;
				}
				return value;
			}

			bool INIFile::getBoolValueOrDefault(std::string key, bool defaultValue)
			{
				return getBoolValueOrDefault("", key, defaultValue);
			}

			std::string INIFile::getComment(std::string section, std::string key)
			{
				INISection sect = getSection(section);

				if (key == "")
				{
					return sect.getComment();
				}

				for (INISection::INIItemIterator it = sect.begin(); it != sect.end(); ++it)
				{
					if (it->key == key)
					{
						return it->comment;
					}
				}

				throw INIException(ERR_NOT_FOUND_KEY, std::string("key `") + key + std::string("` was not found"));
			}

			std::string INIFile::getRightComment(std::string section, std::string key)
			{
				INISection sect = getSection(section);

				if (key == "")
				{
					return sect.getRightComment();
				}

				for (INISection::INIItemIterator it = sect.begin(); it != sect.end(); ++it)
				{
					if (it->key == key)
					{
						return sect.getRightComment();
					}
				}

				throw INIException(ERR_NOT_FOUND_KEY, std::string("key `") + key + std::string("` was not found"));
			}


			std::vector<std::string> INIFile::getKeys(std::string section)
			{
				return getSection(section).getKeys();				
			}


			std::vector<std::string> INIFile::getValues(std::string section)
			{
				return getSection(section).getValues();
			}


			std::vector<std::pair<std::string, std::string>> INIFile::getEntries(std::string _section)
			{
				INISection section = getSection(_section);
				return section.getEntries();
			}


			std::vector<INIItem> INIFile::getItems(std::string _section)
			{
				INISection section = getSection(_section);
				return section.getItems();
			}


			int INIFile::getItemsLength(std::string section)
			{
				return getSection(section).length();
			}


			int INIFile::getItemsLength(INISection section)
			{
				return section.length();
			}


			INISection INIFile::getSection(std::string section)
			{
				for (INISectionIterator i = sectionsCache.begin(); i != sectionsCache.end(); ++i)
				{
					if (i->getName() == section)
					{
						return *i;
					}
				}
				throw INIException(ERR_NOT_FOUND_SECTION, std::string("section `") + section + std::string("` was not found"));
			}


			std::vector<INISection> INIFile::getSections()
			{
				std::vector<INISection> sections;
				for (INISectionIterator i = sectionsCache.begin(); i != sectionsCache.end(); ++i)
				{
					sections.push_back(*i);
				}
				return sections;
			}


			std::vector<INISection> INIFile::getSections(std::string section, ...)
			{
				std::vector<INISection> sections;
				for (INISectionIterator i = sectionsCache.begin(); i != sectionsCache.end(); ++i)
				{
					sections.push_back(*i);
				}
				return sections;
			}


			std::vector<std::string> INIFile::getSectionNames()
			{
				std::vector<std::string> sectionNames;
				for (INISectionIterator i = sectionsCache.begin(); i != sectionsCache.end(); ++i)
				{
					sectionNames.push_back(i->getName());
				}
				return sectionNames;
			}


			bool INIFile::hasSection(std::string section)
			{
				try
				{
					getSection(section);
				}
				catch (INIException&)
				{
					return false;
				}
				return true;
			}


			bool INIFile::hasKey(std::string section, std::string key)
			{
				return getSection(section).hasKey(key);
			}


			bool INIFile::hasKey(std::string key)
			{
				return hasKey("", key);
			}


			void INIFile::setStringValue(const std::string section, const std::string key, const std::string value)
			{
			}


			void INIFile::setIntValue(const std::string section, const std::string key, int value)
			{
			}


			void INIFile::setDoubleValue(const std::string section, const std::string key, double value)
			{
			}


			void INIFile::setBoolValue(const std::string section, const std::string key, bool value)
			{
			}


			void INIFile::setComment(std::string section, std::string key, std::string comment)
			{
			}


			int INIFile::setRightComment(std::string section, const std::string key, std::string rightComment)
			{
				return 0;
			}


		};
	};
};

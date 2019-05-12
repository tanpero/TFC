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


			INISection INIFile::updateSection(std::string cleanLine, std::string comment, std::string rightComment)
			{
				INISection newSection;

				// ������������
				size_t index = cleanLine.find_first_of(']');

				if (index == std::string::npos) {
					throw INIException(ERR_UNMATCHED_BRACKETS, std::string("no matched ] found"));
				}

				int len = index - 1;

				// ������Ϊ�գ�������һ��
				if (len <= 0) {
					throw INIException(ERR_SECTION_EMPTY, std::string("section name is empty"));
				}

				// ȡ����
				std::string s(cleanLine, 1, len);
				trim(s);

				// ����������Ϊ��

				if (s == "")
				{
					throw INIException(ERR_SECTION_ALREADY_EXISTS, "invalid empty section name");
				}

				//�����Ƿ��Ѵ���
				if (getSection(s).name != "") {
					throw INIException(ERR_SECTION_ALREADY_EXISTS, std::string("section ") + s + std::string("already exist"));
				}

				// ������
				newSection.name = s;

				// ���ο�ͷ��ע��
				newSection.comment = comment;
				newSection.rightComment = rightComment;
				sectionsCache.push_back(newSection);
				return newSection;
			}


			int INIFile::addEntry(std::string cleanLine, std::string comment, std::string rightComment, INISection currSection)
			{
				INIItem item = parse(cleanLine);

				item.comment = comment;
				item.rightComment = rightComment;

				currSection.items.push_back(item);
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
				std::string line;       // ��ע�͵���
				std::string cleanLine;  // ȥ��ע�ͺ����
				std::string comment;
				std::string rightComment;
				INISection currSection;  // ��ʼ��һ���ֶ�ָ��

				release();

				path = filePath;
				std::ifstream ifs(path);
				if (!ifs.is_open()) {
					return ERR_OPEN_FILE_FAILED;
				}

				//����Ĭ�϶Σ��� ������""
				currSection.name = "";
				sectionsCache.push_back(currSection);

				// ÿ�ζ�ȡһ�����ݵ�line
				while (std::getline(ifs, line)) {
					trim(line);

					// step 0�����д����������Ϊ0��˵���ǿ��У���ӵ�comment��������ע�͵�һ����
					if (line.length() <= 0) {
						comment += '\n';
						continue;
					}

					// step 1
					// ������ײ���ע�ͣ�������β�Ƿ����ע��
					// ���������ע�Ϳ�ͷ����ӵ�comment��������ǰѭ����continue
					if (isCommentLine(line)) {
						comment += line + '\n';
						continue;
					}

					// ������ײ���ע�ͣ�������β�Ƿ����ע�ͣ������ڣ��и���У���ע��������ӵ�rightComment
					auto commentContent = split(line, "#");
					cleanLine = commentContent.first;
					rightComment = commentContent.second;

					// step 2���ж�line�����Ƿ�Ϊ�λ��
					//�ο�ͷ���� [
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

						// ��������Ǽ�ֵ����ӵ�section�ε�items����
						errorValue = addEntry(cleanLine, comment, rightComment, currSection);
					}

					// comment����
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

				// ����section����
				for (INISectionIterator sect = sectionsCache.begin(); sect != sectionsCache.end(); ++sect)
				{
					if (sect->comment != "")
					{
						data += sect->comment;
					}

					if (sect->name != "")
					{
						data += std::string("[") + sect->name + std::string("]");
						data += '\n';
					}

					if (sect->rightComment != "")
					{
						data += " #" + sect->rightComment;
					}

					// ���� item ����
					for (INISection::INIItemIterator item = sect->items.begin(); item != sect->items.end(); ++item)
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


			int INIFile::getIntValue(std::string section, std::string key)
			{
				return atoi(getValue(section, key).value.data());
			}


			int INIFile::getIntValue(std::string key)
			{
				return getIntValue("", key);
			}


			double INIFile::getDoubleValue(std::string section, std::string key)
			{
				return atof(getValue(section, key).value.data());
			}


			double INIFile::getDoubleValue(std::string key)
			{
				return getDoubleValue("", key);
			}


			bool INIFile::getBoolValue(std::string section, std::string key)
			{
				std::string value = getValue(section, key).value;
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


			std::vector<std::string> INIFile::getKeys(std::string section)
			{
				return std::vector<std::string>();
			}


			std::vector<std::string> INIFile::getValues(std::string section)
			{
				return std::vector<std::string>();
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
					if (i->name == section)
					{
						return *i;
					}
				}
				throw INIException(ERR_NOT_FOUND_SECTION, std::string("cannot find such section"));
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
				return std::vector<INISection>();
			}


			std::vector<std::string> INIFile::getSectionNames()
			{
				std::vector<std::string> sectionNames;
				for (INISectionIterator i = sectionsCache.begin(); i != sectionsCache.end(); ++i)
				{
					sectionNames.push_back(i->name);
				}
				return sectionNames;
			}


			bool INIFile::hasSection(std::string section)
			{
				return false;
			}


			bool INIFile::hasKey(std::string section, std::string key)
			{
				return false;
			}


			bool INIFile::hasKey(std::string key)
			{
				return false;
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

#include "ini.h"

namespace tfc
{
	namespace data
	{
		namespace ini
		{

			std::string INIFile::getValue(std::string section, std::string key)
			{
				INISection sect;

				try
				{
					sect = getSection(section);
				}
				catch (INIException& e)
				{
					throw INIException(e.errtype(), e.info());
				}

				for (INISection::INIItem_it it = sect->begin(); it != sect->end(); ++it) {
					if (it->key == key)
					{
						*value = it->value;
						*comment = it->comment;
						return RET_OK;
					}
				}

				errMsg = std::string("not find the key ") + key;
				return ERR_NOT_FOUND_KEY;
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

		};
	};
};

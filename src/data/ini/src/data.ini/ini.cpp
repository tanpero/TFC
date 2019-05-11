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


		};
	};
};

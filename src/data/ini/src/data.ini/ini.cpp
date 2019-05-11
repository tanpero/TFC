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


		};
	};
};

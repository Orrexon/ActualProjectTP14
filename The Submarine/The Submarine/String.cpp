#include "String.h"

namespace String
{
	void SplitStr2Vec(const char* pszPath, std::vector<std::string>& vecString)
	{
		char * pch;

		pch = strtok(const_cast < char*> (pszPath), STR_TOKEN);
		while (pch != NULL)
		{
			vecString.push_back(pch);
			pch = strtok(NULL, STR_TOKEN);
		}
	}

	std::string GetRelativePath(const char* pszPath1, const char* pszPath2)
	{
		std::vector<std::string> vecPath1, vecPath2;
		vecPath1.clear();
		vecPath2.clear();
		SplitStr2Vec(pszPath1, vecPath1);
		SplitStr2Vec(pszPath2, vecPath2);
		size_t iSize = (vecPath1.size() < vecPath2.size()) ? vecPath1.size() : vecPath2.size();
		unsigned int iSameSize(0);
		for (unsigned int i = 0; i<iSize; ++i)
		{
			if (vecPath1[i] != vecPath2[i])
			{
				iSameSize = i;
				break;
			}
		}

		std::string m_strRelativePath = "";
		for (unsigned int i = 0; i< (vecPath1.size() - iSameSize); ++i)
			m_strRelativePath += const_cast<char *> (LAST_FOLDER);

		for (unsigned int i = iSameSize; i<vecPath2.size(); ++i)
		{
			m_strRelativePath += vecPath2[i];
			if (i < (vecPath2.size() - 1))
				m_strRelativePath += const_cast<char *> (FOLDER_SEP);
		}

		return m_strRelativePath;
	}
}
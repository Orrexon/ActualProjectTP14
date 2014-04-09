#pragma once

#include <vector>
#include <string>

namespace String
{
	#ifdef _WIN32                                                                              
		#define STR_TOKEN "\\"                                                                 
		#define LAST_FOLDER "..\\"                                                             
		#define FOLDER_SEP "\\"                                                                
		#define LINE_BREAK "\r\n"                                                              
	#else                                                                                      
		#define STR_TOKEN "/"                                                                  
		#define LAST_FOLDER "../"                                                              
		#define FOLDER_SEP "/"                                                                 
		#define LINE_BREAK "/n"                                                               
	#endif // _WIN32                                                                           

	void SplitStr2Vec(const char* pszPath, std::vector<std::string>& vecString);

	std::string GetRelativePath(const char* pszPath1, const char* pszPath2);
}
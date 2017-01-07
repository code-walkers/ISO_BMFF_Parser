#ifndef _IMF_PARSER_H_
#define _IMF_PARSER_H_

#include <vector>
#include "IMFParseError.h"
#include "IMFStream.h"
#include "IMFBoxes.h"
#include "Utils.h"



class IMFParser
{
public:
	IMFParser(std::string address, bool bUrl = false)
		:m_pStream(NULL)			
	{
		try {
			m_pStream = new IMFStream(address, bUrl);
		} catch (int e) {
			//DEBUG_PRINT("Failed to create parser errcode %d\n", e);
			throw ERR_IMFPARSER_INIT_FAIL;
		}
	}

	~IMFParser()
	{
		for (size_t i = 0; i < m_parsedBoxes.size(); i++) {
			delete(m_parsedBoxes[i]);
		}
		m_parsedBoxes.clear();
		delete m_pStream;
	}

	std::vector<Box *>& parse();

private: 
	IMFStream *m_pStream;
	std::vector<Box *> m_parsedBoxes;

	Box * _parseBoxFType(int boxSize);
	Box * _parseBoxMovieHdr(int boxSize);
	Box * _parseTrakHdr(int boxSize);

};


#endif
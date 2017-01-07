
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "Utils.h"
#include "IMFParser.h"


/*Note: The parser's calling of IMFStream::releaseBuf() (an explicit call to release memory)
 *is not required in Languages like JAVA or even using C++11's shared_ptr or unique_ptr.
*/
std::vector<Box *>& IMFParser::parse()
{
	char *pbuff;
	int heirarchy = 0;
	bool firstBox = true;
	while (!m_pStream->eof() && m_pStream->canRead()) {
		//Box size
		try {
			int r = m_pStream->readBytes(pbuff, 4);
			int boxSize = buff4ToInt(pbuff);
			boxSize -= 4;
			m_pStream->releaseBuf(pbuff);

			//Box type
			m_pStream->readBytes(pbuff, 4);
			int boxType = buff4ToInt(pbuff);
			boxSize -= 4;
			m_pStream->releaseBuf(pbuff);

			if (firstBox && boxType != BOX_FTYP) {
				break;
			}

			firstBox = false;

			switch (boxType)
			{
				case BOX_FTYP:
				{
					Box *pBoxFtyp = _parseBoxFType(boxSize);
					pBoxFtyp->setHeirarchy(0);
					m_parsedBoxes.push_back(pBoxFtyp);
					break;
				}
				case BOX_CONT_MOOV:
				{
					Box *pBoxMoov = new MovieBox(BOX_CONT_MOOV, boxSize);
					pBoxMoov->setHeirarchy(0);
					m_parsedBoxes.push_back(pBoxMoov);
					break;
				}
				case BOX_MVHD:
				{
					heirarchy++;
					Box *pBoxMvHdr = _parseBoxMovieHdr(boxSize);
					pBoxMvHdr->setHeirarchy(1);
					m_parsedBoxes.push_back(pBoxMvHdr);
					break;
				}
				case BOX_TRAK:
				{
					Box *pBoxTrak = new TrackBox(BOX_TRAK, boxSize);
					pBoxTrak->setHeirarchy(1);
					m_parsedBoxes.push_back(pBoxTrak);
					break;
				}
				case BOX_TKHD:
				{
					Box *pBoxTrakHdr = _parseTrakHdr(boxSize);
					pBoxTrakHdr->setHeirarchy(2);
					m_parsedBoxes.push_back(pBoxTrakHdr);
					break;
				}
				case BOX_MDAT:
				{
					Box *pBoxMdat = new MediaDataBox(BOX_MDAT, boxSize);
					pBoxMdat->setHeirarchy(1);
					m_parsedBoxes.push_back(pBoxMdat);

					char *pbuff;
					m_pStream->readBytes(pbuff, boxSize);
					m_pStream->releaseBuf(pbuff);
					break;
				}
				case BOX_FREE:
				{
					Box *pBoxFree = new FreeSpaceBox(BOX_FREE, boxSize);
					pBoxFree->setHeirarchy(1);
					m_parsedBoxes.push_back(pBoxFree);

					char *pbuff;
					m_pStream->readBytes(pbuff, boxSize);
					m_pStream->releaseBuf(pbuff);
					break;
				}
				case BOX_CONT_PDIN:
				case BOX_CONT_META:
				case BOX_CONT_MECO:
				case BOX_CONT_STYP:
				case BOX_CONT_SIDX:
				case BOX_CONT_SSIX:
				case BOX_CONT_PRFT:
				case BOX_MOOF:
				case BOX_MFRA:
				case BOX_SKIP:
				case BOX_TREF:
				case BOX_TRGR:
				case BOX_EDTS:
				case BOX_MDIA:
				case BOX_MVEX:
				default:
				{
					char *pbuff;
					m_pStream->readBytes(pbuff, boxSize);
					m_pStream->releaseBuf(pbuff);
					break;
				}
			}
		}
		catch (int e) {
			if (ERR_STREAM_END == e) {
				
			}
			else {
				wxMessageBox(wxString::Format(wxT("Parsing halted due to unexpected error %d"),e));
			}
		}
	}
	return m_parsedBoxes;
}





Box * IMFParser::_parseBoxFType(int boxSize)
{
	char *pbuff;
	int bytesRem = boxSize;
	try
	{
		//Major brand
		m_pStream->readBytes(pbuff, 4);
		bytesRem -= 4;
		int major_brand = buff4ToInt(pbuff);
		m_pStream->releaseBuf(pbuff);

		//Minor version
		m_pStream->readBytes(pbuff, 4);
		bytesRem -= 4;
		int minor_version = buff4ToInt(pbuff);
		m_pStream->releaseBuf(pbuff);

		//Compatibe brands
		std::vector<int> compatibleBrands;
		while (bytesRem) {
			m_pStream->readBytes(pbuff, 4);
			bytesRem -= 4;
			compatibleBrands.push_back(buff4ToInt(pbuff));
			m_pStream->releaseBuf(pbuff);
		}

		return new FileTypeBox(BOX_FTYP, boxSize, major_brand, minor_version, compatibleBrands);

	}
	catch (int e) {
		throw ERROR_PARSE_BOX_FTYPE;
	}

	return NULL;
}




Box * IMFParser::_parseTrakHdr(int boxSize)
{
	char *pbuff;
	try {
		//Version and Flags
		m_pStream->readBytes(pbuff, 4);
		int versionAndFlags = buff4ToInt(pbuff);
		m_pStream->releaseBuf(pbuff);

		//version
		char version = (versionAndFlags >> 24) & 0xFF;
		//falgs
		int flags = versionAndFlags & 0x00FFFFFF;

		long long int creation_time;
		long long int modification_time;
		int track_id;
		int reserved;
		long long int duration;

		if (version) {
			//creation time
			m_pStream->readBytes(pbuff, 8);
			creation_time = buff8ToInt64(pbuff);
			m_pStream->releaseBuf(pbuff);
			//modification time
			m_pStream->readBytes(pbuff, 8);
			modification_time = buff8ToInt64(pbuff);
			m_pStream->releaseBuf(pbuff);
			//track id
			m_pStream->readBytes(pbuff, 4);
			track_id = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);

			//reserved
			m_pStream->readBytes(pbuff, 4);
			reserved = 0;
			m_pStream->releaseBuf(pbuff);
			//duration
			m_pStream->readBytes(pbuff, 8);
			duration = buff8ToInt64(pbuff);
			m_pStream->releaseBuf(pbuff);
		}
		else {
			//creation time
			m_pStream->readBytes(pbuff, 4);
			creation_time = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
			//modification time
			m_pStream->readBytes(pbuff, 4);
			modification_time = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
			//track id
			m_pStream->readBytes(pbuff, 4);
			track_id = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);

			//reserved
			m_pStream->readBytes(pbuff, 4);
			reserved = 0;
			m_pStream->releaseBuf(pbuff);
			//duration
			m_pStream->readBytes(pbuff, 4);
			duration = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
		}

		//reserved2 hardcoded to 0
		m_pStream->readBytes(pbuff, 4);
		m_pStream->releaseBuf(pbuff);

		m_pStream->readBytes(pbuff, 4);
		m_pStream->releaseBuf(pbuff);

		//layer
		short layer;
		m_pStream->readBytes(pbuff, 2);
		layer = buff2ToShort(pbuff);
		m_pStream->releaseBuf(pbuff);

		//alternate_group
		short alternate_group;
		m_pStream->readBytes(pbuff, 2);
		alternate_group = buff2ToShort(pbuff);
		m_pStream->releaseBuf(pbuff);

		//volume
		short volume;
		m_pStream->readBytes(pbuff, 2);
		volume = buff2ToShort(pbuff);
		m_pStream->releaseBuf(pbuff);

		//reserved3
		short reserved3;
		m_pStream->readBytes(pbuff, 2);
		reserved3 = buff2ToShort(pbuff);
		m_pStream->releaseBuf(pbuff);

		//matrix hardcoded
		for (size_t i = 0; i < 9; i++) {
			m_pStream->readBytes(pbuff, 4);
			m_pStream->releaseBuf(pbuff);
		}
		//width
		int width;
		m_pStream->readBytes(pbuff, 4);
		width = buff4ToInt(pbuff);
		m_pStream->releaseBuf(pbuff);
		//height
		int height;
		m_pStream->readBytes(pbuff, 4);
		height = buff4ToInt(pbuff);;
		m_pStream->releaseBuf(pbuff);

		Box * pBox = new TrackHeaderBox(BOX_TKHD, boxSize, (int)version, flags,creation_time, modification_time, track_id, duration, layer, width,height);
		return pBox;
	}
	catch (int e) {
		throw ERROR_PARSE_TRAK_HDR;
	}
	return NULL;
}


Box * IMFParser::_parseBoxMovieHdr(int boxSize)
{
	char *pbuff;
	long long int bytesRem = boxSize;

	try {
		m_pStream->readBytes(pbuff, 4);
		int versionAndFlags = buff4ToInt(pbuff);
		m_pStream->releaseBuf(pbuff);

		//version
		char version = (versionAndFlags >> 24) & 0xFF;
		//falgs
		int flags = versionAndFlags & 0x00FFFFFF;

		long long int creation_time;
		long long int modification_time;
		int time_scale;
		long long int duration;
		if (version) {
			//creation time
			m_pStream->readBytes(pbuff, 8);
			creation_time = buff8ToInt64(pbuff);
			m_pStream->releaseBuf(pbuff);
			//modification time
			m_pStream->readBytes(pbuff, 8);
			modification_time = buff8ToInt64(pbuff);
			m_pStream->releaseBuf(pbuff);
			//timescale
			m_pStream->readBytes(pbuff, 4);
			time_scale = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
			//duration
			m_pStream->readBytes(pbuff, 8);
			duration = buff8ToInt64(pbuff);
			m_pStream->releaseBuf(pbuff);
		}
		else {
			//creation time
			m_pStream->readBytes(pbuff, 4);
			creation_time = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
			//modification time
			m_pStream->readBytes(pbuff, 4);
			modification_time = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
			//timescale
			m_pStream->readBytes(pbuff, 4);
			time_scale = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
			//duration
			m_pStream->readBytes(pbuff, 4);
			duration = buff4ToInt(pbuff);
			m_pStream->releaseBuf(pbuff);
		}

		//rate
		int rate;
		m_pStream->readBytes(pbuff, 4);
		rate = buff4ToInt(pbuff);
		m_pStream->releaseBuf(pbuff);

		//volume
		short volume;
		m_pStream->readBytes(pbuff, 2);
		volume = buff2ToShort(pbuff);
		m_pStream->releaseBuf(pbuff);

		//reserved
		short reserved;
		m_pStream->readBytes(pbuff, 2);
		reserved = buff2ToShort(pbuff);
		m_pStream->releaseBuf(pbuff);

		//reserved2 hardcoded to 0
		m_pStream->readBytes(pbuff, 4);
		m_pStream->releaseBuf(pbuff);

		m_pStream->readBytes(pbuff, 4);
		m_pStream->releaseBuf(pbuff);

		//matrix hardcoded
		for (size_t i = 0; i < 9; i++) {
			m_pStream->readBytes(pbuff, 4);
			m_pStream->releaseBuf(pbuff);
		}

		//predefined always 0
		for (size_t i = 0; i < 6; i++) {
			m_pStream->readBytes(pbuff, 4);
			m_pStream->releaseBuf(pbuff);
		}

		int next_track_id;
		m_pStream->readBytes(pbuff, 4);
		next_track_id = buff4ToInt(pbuff);
		m_pStream->releaseBuf(pbuff);

		Box * pBox = new MovieHeaderBox(BOX_MVHD, boxSize, (int)version, creation_time, modification_time, time_scale, duration, next_track_id);
		return pBox;

	}
	catch (int e) {
		throw ERROR_PARSE_MOVIE_HDR;
	}



	return NULL;
}






#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "Utils.h"
#include "IMFParser.h"


std::string boxTypeToString(int boxType)
{
	if ((boxType == BOX_FTYP)) {
		return string("ftyp");
	}
	else if ((boxType == BOX_CONT_MOOV)) {
		return string("moov");
	}
	else if ((boxType == BOX_MVHD)) {
		return string("mvhd");
	}
	else if ((boxType == BOX_TRAK)) {
		return string("trak");
	}
	else if ((boxType == BOX_TKHD)) {
		return string("tkhd");
	} else if((boxType == BOX_MDAT)) {
		return string("mdat");
	}
	else if ((boxType == BOX_FREE)) {
		return string("free");
	} else {
		return string("unkw");
	}
}




void FullBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
}



void TrackHeaderBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
	indent(out);
	DEBUG_PRINT(out, "version: 0x%x\r\n", m_version);
	indent(out);
	DEBUG_PRINT(out, "flags: 0x%x\r\n", m_flags24);
	indent(out);
	DEBUG_PRINT(out, "creationTime: 0x%x\r\n", m_creationTime);
	indent(out);
	DEBUG_PRINT(out, "modificationTime: 0x%x\r\n", m_modificationTime);
	indent(out);
	DEBUG_PRINT(out, "trackId: 0x%x\r\n", m_trackId);
	indent(out);
	DEBUG_PRINT(out, "duration: 0x%x\r\n", m_duration);
	indent(out);
	DEBUG_PRINT(out, "layer: 0x%x\r\n", m_layer);
	indent(out);
	DEBUG_PRINT(out, "alternate_group: 0x%x\r\n", m_alternate_group);
	indent(out);
	DEBUG_PRINT(out, "volume: 0x%x\r\n", m_volume);
	indent(out);
	DEBUG_PRINT(out, "matrix: {0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x}\r\n",
		m_matrix[0],
		m_matrix[1],
		m_matrix[2],
		m_matrix[3],
		m_matrix[4],
		m_matrix[5],
		m_matrix[6],
		m_matrix[7],
		m_matrix[8]
		);
	indent(out);
	DEBUG_PRINT(out, "width: 0x%x\r\n", m_width);
	indent(out);
	DEBUG_PRINT(out, "height: 0x%x\r\n", m_height);
	indent(out);
	DEBUG_PRINT(out, "\r\n");
}


TrackHeaderBox::TrackHeaderBox(int boxType,
	long long int size,
	char version,
	int flags,
	long long int creationTime,
	long long int modificationTime,
	int trackId,
	long long int duration,
	short layer,
	int width,
	int height)
	:FullBox(boxType, size, version, flags)
{
	if (boxType != 0x746B6864) {  //0x746B6864 => 'tkhd'
								  //log_error
		throw ERR_INV_BOX_TYP;
	}
	m_version = version;
	m_creationTime = creationTime;
	m_modificationTime = modificationTime;
	m_trackId = trackId;
	m_reserved = 0;
	m_duration = duration;
	m_reserved2[0] = m_reserved2[1] = 0;
	m_layer = layer;
	m_alternate_group = 0;
	m_volume = 0; // not sure how to check track is audio yet
	m_reserved3 = 0;
	const int matrix[9] = { 0x00010000, 0, 0, 0, 0x00010000, 0, 0, 0, 0x40000000 };
	memcpy(m_matrix, matrix, 9 * 4);
	m_width = width;
	m_height = height;
}

void MovieHeaderBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
	indent(out);
	DEBUG_PRINT(out, "version: 0x%x\r\n", m_version);
	indent(out);
	DEBUG_PRINT(out, "flags: 0x%x\r\n", m_flags24);
	indent(out);
	DEBUG_PRINT(out, "creationTime: 0x%x\r\n", m_creationTime);
	indent(out);
	DEBUG_PRINT(out, "modificationTime: 0x%x\r\n", m_modificationTime);
	indent(out);
	DEBUG_PRINT(out, "timeSale: 0x%x\r\n", m_timescale);
	indent(out);
	DEBUG_PRINT(out, "duration: 0x%x\r\n", m_duration);
	indent(out);
	DEBUG_PRINT(out, "rate: 0x%x\r\n", m_rate);
	indent(out);
	DEBUG_PRINT(out, "volume: 0x%x\r\n", m_volume);
	indent(out);
	DEBUG_PRINT(out, "matrix: {0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x}\r\n",
		m_matrix[0],
		m_matrix[1],
		m_matrix[2],
		m_matrix[3],
		m_matrix[4],
		m_matrix[5],
		m_matrix[6],
		m_matrix[7],
		m_matrix[8]
	);
	indent(out);
	DEBUG_PRINT(out, "nextTrackId: 0x%x\r\n", m_nextTrackId);
	indent(out);
	DEBUG_PRINT(out,"\r\n");
}

MovieHeaderBox::MovieHeaderBox(int boxType,
	long long int size,
	int version,
	long long int creationTime,
	long long int modificationTime,
	int timescale,
	long long int duration,
	int nextTackId)
	:FullBox(boxType, size, version, 0)            // 0x6d766864 => 'mvhd'
{
	if (boxType != 0x6d766864) {
		//log_error
		throw ERR_INV_BOX_TYP;
	}
	m_version = version;
	m_creationTime = creationTime;
	m_modificationTime = modificationTime;
	m_timescale = timescale;
	m_duration = duration;
	m_rate = 0x00010000;
	m_volume = 0x0100;
	m_reserved = 0;
	m_reserved2[0] = m_reserved2[1] = 0;
	static const int matrix[9] = { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };
	memcpy(m_matrix, matrix, 9 * 4);
	for (size_t i = 0; i < 6; i++) {
		m_preDefined[i] = 0;
	}
	m_nextTrackId = nextTackId;
}


void ProgressiveDownloadInfoBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());

}



void FileTypeBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
	indent(out);
	DEBUG_PRINT(out, "major_brand: 0x%x\r\n", m_major_brand);
	indent(out);
	DEBUG_PRINT(out, "minor_version: 0x%x\r\n", m_minor_version);
	indent(out);
	DEBUG_PRINT(out, "\r\n");
}

FileTypeBox::FileTypeBox(int boxType, long long int size, int major_brand, int minor_ver, std::vector<int> compat_brands) :
	Box(boxType, size)  //'ftyp' => 0x66747970
{
	if (boxType != 0x66747970) {
		//log_error
		throw ERR_INV_BOX_TYP;
	}
	m_major_brand = major_brand;
	m_minor_version = minor_ver;
	m_vect_compatible_brands = compat_brands;
}



void MediaDataBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
	indent(out);
	DEBUG_PRINT(out, "\r\n");
}



void FreeSpaceBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
	indent(out);
	DEBUG_PRINT(out, "\r\n");
}



void MovieBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
}




void TrackBox::printBoxContent(wxFileOutputStream *out)
{
	indent(out);
	DEBUG_PRINT(out, "size: %d\r\n", m_size);
	indent(out);
	DEBUG_PRINT(out, "type: %s\r\n", boxTypeToString(m_boxType).c_str());
}
#ifndef _BOX_H_
#define _BOX_H_
#include <list>
#include <vector>
#include <string>
using namespace std;


#define ERR_INV_BOX_TYP (0x100)



/******Constants****************************************************************/

#define BOX_FTYP      (0x66747970)
#define BOX_CONT_PDIN (0x7064696e)
#define BOX_CONT_MOOV (0x6d6f6f76)
#define BOX_CONT_META (0x6d657461)
#define BOX_CONT_MECO (0x6d65636f)
#define BOX_CONT_STYP (0x73746f70)
#define BOX_CONT_SIDX (0x73696478)
#define BOX_CONT_SSIX (0x73736978)
#define BOX_CONT_PRFT (0x70726674)

#define BOX_MVHD      (0x6D766864)
#define BOX_TRAK      (0x7472616B)
#define BOX_MOOF	  (0x6d6f6f66)
#define BOX_MFRA      (0x6d667261)
#define BOX_MDAT      (0x6d646174)
#define BOX_FREE      (0x66726565)
#define BOX_SKIP      (0x736b6970)

#define BOX_TKHD      (0x746b6864)
#define BOX_TREF      (0x74726566)
#define BOX_TRGR      (0x74726772)
#define BOX_EDTS      (0x65647473)
#define BOX_MDIA      (0x6d646961) 
#define BOX_MVEX      (0x6d766578)

/*****************************************************************************/

std::string boxTypeToString(int boxType);

/*****************************************************************************/

/*! Box
*/
class Box
{
public:
	Box(int boxType, long long int size)
	{
		m_boxType = boxType;
		m_size = size;
		m_heirarchy = 0;
	}
	virtual ~Box() 	{}

	virtual void indent(wxFileOutputStream *out)
	{
		for (size_t i = 0; i < m_heirarchy; i++) {
			DEBUG_PRINT(out,"        ");
		}
	}
	virtual void printBoxContent(wxFileOutputStream *out){}
	
	virtual string getBoxType()
	{
		return  boxTypeToString(m_boxType);
	}
	
	void setHeirarchy(int heirarchy) {
		m_heirarchy = heirarchy;
	}
protected:
	long long int m_size; //can accomodate 64-bits
	int m_boxType; //Extended type not implemented
	int m_heirarchy; //Position/layer num in BMFF box types heirarchy
};

/*! FullBox
*/
class FullBox : public Box
{
public:
	FullBox(int boxType, long long int size, char version, int flags) :
		Box(boxType, size)
	{
		m_version = version;
		m_flags24 = flags;
	}
	virtual ~FullBox() {}
	virtual void printBoxContent(wxFileOutputStream *out);

protected:
	char m_version;
	int  m_flags24;
};

/*! TrackHeaderBox
*/
class TrackHeaderBox : public FullBox
{
public:
	TrackHeaderBox(int boxType,
		long long int size,
		char version,
		int flags,
		long long int creationTime,
		long long int modificationTime,
		int trackId,
		long long int duration,
		short layer,
		int width,
		int height);

	virtual ~TrackHeaderBox() {}
	virtual void printBoxContent(wxFileOutputStream *out);

protected:			
	int m_version;
	long long int m_creationTime;
	long long int m_modificationTime;
	int m_trackId;
	int m_reserved;
	long long int m_duration;
	int m_reserved2[2];
	short m_layer;
	short m_alternate_group;
	short m_volume;
	short m_reserved3;
	int m_matrix[9];
	int m_width;
	int m_height;
};

/*! MovieHeaderBox
*/
class MovieHeaderBox : public FullBox
{
public:
	MovieHeaderBox(int boxType,
		long long int size,
		int version,
		long long int creationTime,
		long long int modificationTime,
		int timescale,
		long long int duration,
		int nextTackId);

	virtual ~MovieHeaderBox() {}

	virtual void printBoxContent(wxFileOutputStream *out);

protected:	
	int m_version;
	long long int m_creationTime;
	long long int m_modificationTime;
	int m_timescale;
	long long int m_duration;
	int m_rate;
	short m_volume;
	short m_reserved;
	int m_reserved2[2];
	int m_matrix[9];
	int m_preDefined[6];
	int m_nextTrackId;

};

/*! ProgressiveDownloadInfoBox
*/
class ProgressiveDownloadInfoBox : public FullBox
{
public:
	ProgressiveDownloadInfoBox(int boxType, long long int size) :
		FullBox(boxType, size, 0 /*fixed*/, 0 /*fixed*/)   //0x7064696e => 'pdin'
	{
		if (boxType != 0x7064696e) {
			//log_error
			throw ERR_INV_BOX_TYP;
		}
	}
	
	typedef struct _DLRate
	{
		int m_rate;
		int m_initialDelay;
	}DLRate;

	virtual ~ProgressiveDownloadInfoBox() {}

	virtual void printBoxContent(wxFileOutputStream *out);


protected:
	std::vector<DLRate> m_downloadRateInfo;
};


/*! FileTypeBox
*/
class FileTypeBox : public Box
{
public:
	FileTypeBox(int boxType, long long int size, int major_brand, int minor_ver, std::vector<int> compat_brands);
	virtual ~FileTypeBox() {};
	virtual void printBoxContent(wxFileOutputStream *out);
protected:
	int m_major_brand;
	int m_minor_version;
	std::vector<int> m_vect_compatible_brands;
};

/*! MediaDataBox
*/
class MediaDataBox : public Box
{
public:
	MediaDataBox(int boxType, long long int size) :    //Skipping actual data since out of scope for this parser
		Box(boxType, size)  // 'mdat' => 0x6D646174
	{
		if (boxType != 0x6D646174) {
			//log_error
			throw ERR_INV_BOX_TYP;
		}
	}
	virtual ~MediaDataBox() {}
	virtual void printBoxContent(wxFileOutputStream *out);
protected:

};

/*! FreeSpaceBox
*/
class FreeSpaceBox : public Box
{
public:
	FreeSpaceBox(int boxType, long long int size) :
		Box(boxType, size)                        //0x66726565 => 'free' , 0x736B6970 => 'skip'
	{
		if (boxType != 0x66726565 && boxType != 0x736B6970) {
			//log_error
			throw ERR_INV_BOX_TYP;
		}
	}
	virtual ~FreeSpaceBox() {}
	virtual void printBoxContent(wxFileOutputStream *out);
protected:

};

/*! MovieBox
*/
class MovieBox : public Box
{
public:
	MovieBox(int boxType, long long int size) :
		Box(boxType, size)                     //0x6d6f6f76 => 'moov'
	{
		if (boxType != 0x6d6f6f76 ) {
			//log_error
			throw ERR_INV_BOX_TYP;
		}
	}
	virtual ~MovieBox() {}
	virtual void printBoxContent(wxFileOutputStream *out);
protected:

};


/*! TrackBox
*/
class TrackBox : public Box
{
public:
	TrackBox(int boxType, long long int size) :
		Box(boxType, size)
	{
		if (boxType != 0x7472616b) {          //0x7472616b =>' trac'
			//log_error
			throw ERR_INV_BOX_TYP;
		}
	}
	virtual ~TrackBox() {}
	virtual void printBoxContent(wxFileOutputStream *out);
protected:

};

#endif


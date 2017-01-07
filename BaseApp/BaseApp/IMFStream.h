#ifndef _PARSER_H_
#define _PARSER_H_
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/wfStream.h>
#include <wx/sstream.h>
#include <wx/protocol/http.h>

#define ERR_CANNOT_OPEN_SOURCE_STREAM (0x1000)
#define ERR_STREAM_END                (0x1001)


using namespace std;


class IMFStream
{
public:
	IMFStream(std::string address, bool bUrl = false)
	{
		if (bUrl) {
			//TODO: Http connection 
			//Ideally HTTP connection, reading the HTTP stream should be done in a worker thread,
			//so that application does not block waiting for live stream.
#if defined(ENABLE_HTTP_SUPPORT)
			wxHTTP http;
			http.SetHeader(_T("Content-type"), "application/octet-stream");
			http.SetTimeout(10);

			if (http.Connect(server, 80)) {				
				m_pInputStream = http.GetInputStream(wxT("/files/ProRes_option9.mov"));
				if (m_pInputStream == NULL) {
					throw ERR_CANNOT_OPEN_SOURCE_STREAM
				}
			}
#endif
		} else {
			wxFileInputStream *pFileStream = new wxFileInputStream(wxString(address));
			if (!pFileStream->IsOk()) {
				throw ERR_CANNOT_OPEN_SOURCE_STREAM;
			}
			m_pInputStream = static_cast<wxInputStream *> (pFileStream);
		}
	}

	~IMFStream()
	{
		wxDELETE(m_pInputStream);
	}


	/*Returns actual number of bytes read*/
	size_t readBytes(char *&pBuff, long long int lenBytes)
	{
		pBuff = new char[lenBytes];

		if (m_pInputStream->CanRead()) {
			m_pInputStream->Read(pBuff, lenBytes);

			if (m_pInputStream->LastRead() < lenBytes) {
				throw ERR_STREAM_END;
			}
			return m_pInputStream->LastRead();
		}
		else {
			return 0;
		}
	}

	bool canRead()
	{
		return m_pInputStream->CanRead();
	}
	bool eof()
	{
		return m_pInputStream->Eof();
	}


	/*TODO: Alternaltely use boost shared_ptr or C++ 11 shared_ptr to automatically release buffer when 
	  not references any more*/
	void releaseBuf(char *pBuff) {
		delete pBuff;
	}

private:
	wxInputStream *m_pInputStream;

};


#endif

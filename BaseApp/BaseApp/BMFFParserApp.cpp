// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/textctrl.h>
#include <wx/filename.h>
#include "Utils.h"
#include "BoxTree.h"
#include "IMFParser.h"
#include "IMFBoxes.h"

enum
{
	ID_OpenFile = 1
};



class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};



class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	wxTextCtrl *pMainTextBox;
private:
	void OnOpenFile(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_OpenFile, MyFrame::OnOpenFile)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);


bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame("ISO base media file format parser", wxPoint(50, 50), wxSize(450, 340));

	frame->Show(true);
	return true;
}




MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_OpenFile, "&Browse\tCtrl-H",
		"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("ISO_IEC 14496-12 IMF Parser");

	pMainTextBox = new wxTextCtrl(this, wxID_ANY);
}


void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}


void MyFrame::OnAbout(wxCommandEvent& event)
{

}


void MyFrame::OnOpenFile(wxCommandEvent& event)
{
	wxString filename;
	wxFileDialog *openDialog = new wxFileDialog(this, _T("Choose a media file to open"));
	char displayText[100 * 1024];

	if (openDialog->ShowModal() == wxID_OK) {
		filename = openDialog->GetPath();

		if (!filename.empty()) {
			IMFParser *pParser = new IMFParser(filename.ToStdString()); // std::string("D:\\ProRes_option9.mov"));

			/*Parse the ISO file*/
			std::vector<Box *> boxes = pParser->parse();

			wxFileOutputStream *pOutput = new wxFileOutputStream(_T("output.txt"));
			if (boxes.empty()) {
				wxMessageBox(_T("Please verify the file format is ISO/IEC Base Media File Format"));
				goto bail;
			}
			
			/*Print the parsed file details*/
			for (size_t i = 0; i < boxes.size(); i++) {
				boxes[i]->printBoxContent(pOutput);
			}

			pMainTextBox->AppendText(wxT("Parsing completed. Detail were written to output.txt"));
bail:
			delete(pParser);
			/*Close the output file*/
			pOutput->Close();
#if 0 /*Unable to format on UI*/
			wxFileInputStream *output = new wxFileInputStream(_T("output.txt"));
			output->ReadAll(displayText, 100 * 1024);
			wxString displayTextwx = wxString(string((char *)displayText));
			pMainTextBox->AppendText(displayTextwx);
#endif
			
			


		} 
	}

}

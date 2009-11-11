#ifndef __FBFRAMESEARCH_H__
#define __FBFRAMESEARCH_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wxsqlite3.h>
#include "FbFrameBase.h"

class FbFrameSearch : public FbFrameBase
{
public:
	static void Execute(wxAuiMDIParentFrame * parent, const wxString &title);
	FbFrameSearch(wxAuiMDIParentFrame * parent, const wxString & title = wxEmptyString);
protected:
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
	virtual void CreateControls();
	virtual void UpdateBooklist();
private:
	void CreateBookInfo();
	void FillBooks(wxSQLite3ResultSet & result, const wxString &caption);
private:
	wxTextCtrl * m_textTitle;
	wxTextCtrl * m_textAuthor;
	wxString m_title;
private:
	void OnFoundNothing(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMESEARCH_H__

#include "FbFrameSeqn.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbClientData.h"
#include "FbExportDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"
#include "FbParams.h"
#include "FbSequenDlg.h"
#include "FbMasterTypes.h"

IMPLEMENT_CLASS(FbFrameSeqn, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameSeqn, FbFrameBase)
	EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameSeqn::OnColClick)
	EVT_TEXT_ENTER(ID_MASTER_FIND, FbFrameSeqn::OnFindEnter )
	EVT_BUTTON(ID_MASTER_FIND, FbFrameSeqn::OnFindEnter )
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameSeqn::OnContextMenu)
	EVT_MENU(ID_MASTER_APPEND, FbFrameSeqn::OnMasterAppend)
	EVT_MENU(ID_MASTER_MODIFY, FbFrameSeqn::OnMasterModify)
	EVT_MENU(ID_MASTER_DELETE, FbFrameSeqn::OnMasterDelete)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameSeqn::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameSeqn::OnArray)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameSeqn::OnBooksCount)
END_EVENT_TABLE()

FbFrameSeqn::FbFrameSeqn(wxAuiNotebook * parent, bool select)
	: FbFrameBase(parent, ID_FRAME_SEQN, GetTitle(), select),
		m_FindText(NULL), m_FindInfo(NULL), m_SequenceCode(0)
{
	wxPanel * panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_MasterList = new FbMasterViewCtrl;
	m_MasterList->Create(panel, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	m_MasterList->SetSortedColumn(1);
	CreateColumns();

	CreateBooksPanel(this);

	m_FindText = new FbSearchCombo( panel, ID_MASTER_FIND, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 200,-1 ) );

	sizer->Add( m_FindText, 0, wxEXPAND, 0 );
	sizer->Add( m_MasterList, 1, wxTOP|wxEXPAND, 2 );
	panel->SetSizer( sizer );
	panel->Layout();
	sizer->Fit( panel );

	SplitVertically(panel, m_BooksPanel);

	CreateControls(select);

	FindSequence(wxEmptyString);
}

void FbFrameSeqn::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Ser."), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
}

void FbFrameSeqn::CreateMasterThread()
{
	m_MasterList->AssignModel(NULL);
	if (m_MasterThread) {
		m_MasterThread->Close();
		m_MasterThread->Wait();
		wxDELETE(m_MasterThread);
	}
	m_MasterThread = new FbSeqnListThread(this, m_info, m_MasterList->GetSortedColumn());
	m_MasterThread->Execute();
}

void FbFrameSeqn::FindSequence(const wxString &text)
{
	m_info = text;
	CreateMasterThread();
}

void FbFrameSeqn::OpenSequence(const int sequence, const int book)
{
/*
	m_SequenceText = wxEmptyString;
	m_SequenceCode = sequence;
	(new MasterThread(m_MasterList, m_SequenceCode, m_MasterList->GetSortedColumn()))->Execute();
*/
}

void FbFrameSeqn::OnColClick(wxListEvent& event)
{
	CreateMasterThread();
}

void FbFrameSeqn::OnBooksCount(FbCountEvent& event)
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	FbMasterSeqnInfo * info = wxDynamicCast(&event.GetInfo(), FbMasterSeqnInfo);
	if (model && info) {
		model->SetCount(info->GetId(), event.GetCount());
		m_MasterList->Refresh();
	}

	event.Skip();
}

void FbFrameSeqn::OnFindEnter(wxCommandEvent& event)
{
	FindSequence(m_FindText->GetValue());
}

FbFrameSeqn::MasterMenu::MasterMenu(int id)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_DELETE,  _("Delete"));
}

void FbFrameSeqn::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	if (point.x == -1 && point.y == -1) {
		wxSize size = m_MasterList->GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbFrameSeqn::ShowContextMenu(const wxPoint& pos, wxTreeItemId)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbSeqnListData * data = wxDynamicCast(&item, FbSeqnListData);
	int id = data ? data->GetCode() : 0;
	MasterMenu menu(id);
	m_MasterList->PopupMenu(&menu, pos.x, pos.y);
}

void FbFrameSeqn::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbSequenDlg::Append(newname);
	if (id == 0) return;

	m_MasterList->Append(new FbSeqnListData(id));
}

void FbFrameSeqn::OnMasterModify(wxCommandEvent& event)
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model == NULL) return;

	FbModelItem item = m_MasterList->GetCurrent();
	FbSeqnListData * current = wxDynamicCast(&item, FbSeqnListData);
	if (current == NULL) return;

	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbSequenDlg::Modify(old_id, newname);
	if (new_id == 0) return;

	if (new_id != old_id) model->Delete(new_id);
	m_MasterList->Replace(new FbSeqnListData(new_id));
}

void FbFrameSeqn::OnMasterDelete(wxCommandEvent& event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbModelItem item = model->GetCurrent();
	FbSeqnListData * current = wxDynamicCast(&item, FbSeqnListData);
	if (current == NULL) return;

	int id = current->GetCode();

	wxString msg = wxString::Format(_("Delete series \"%s\"?"), current->GetValue(*model).c_str());
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		FbCommonDatabase database;
		FbAutoCommit commit(database);
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM s WHERE sid=%d"), id));
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM bs WHERE sid=%d"), id));
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM fts_s WHERE docid=%d"), id));
		m_MasterList->Delete();
	}
}

FbFrameSeqn::MenuBar::MenuBar()
{
	Append(new MenuFile,   _("&File"));
	Append(new MenuEdit,   _("&Edit"));
	Append(new MenuLib,    _("&Library"));
	Append(new MenuFrame,  _("&Catalog"));
	Append(new MenuMaster, _("&Series"));
	Append(new MenuBook,   _("&Books"));
	Append(new MenuWindow, _("&Window"));
	Append(new MenuHelp,   _("&?"));
}

FbFrameSeqn::MenuMaster::MenuMaster()
{
	Append(ID_MASTER_APPEND,  _("Append"));
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_DELETE,  _("Delete"));
}

wxMenuBar * FbFrameSeqn::CreateMenuBar()
{
	return new MenuBar;
}

void FbFrameSeqn::OnModel( FbArrayEvent& event )
{
	FbSeqnListModel * model = new FbSeqnListModel(event.GetArray());
	m_MasterList->AssignModel(model);
}

void FbFrameSeqn::OnArray( FbArrayEvent& event )
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model) model->Append(event.GetArray());
	m_MasterList->Refresh();
}
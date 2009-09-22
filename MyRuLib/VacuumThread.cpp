#include "VacuumThread.h"
#include "MyRuLibApp.h"

void VacuumThread::Execute()
{
    VacuumThread * thread = new VacuumThread();
    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }
    thread->Run();
}

void * VacuumThread::Entry()
{
    wxLogInfo(wxT("Start SQLite VACUUM"));
    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    wxGetApp().GetDatabase().ExecuteUpdate(wxT("VACUUM"));
    wxLogInfo(wxT("Finish SQLite VACUUM"));
    return NULL;
}
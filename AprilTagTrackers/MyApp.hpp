#pragma once

#include "Config.hpp"
#include "Connection.hpp"
#include "GUI.hpp"
#include "Localization.hpp"
#include "Tracker.hpp"
#include "utils/Env.hpp"
#include "utils/Log.hpp"

#include <wx/app.h>

class MyLogFrame : public wxFrame
{
public:
    wxTextCtrl* logText = nullptr;
    MyLogFrame()
        : wxFrame(NULL, wxID_ANY, _("Test"))
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

      logText  = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxSize(600, 300), wxTE_MULTILINE | wxTE_READONLY);

        sizer->Add(logText, 1, wxEXPAND, 5);
        SetSizer(sizer);

         wxLog::SetActiveTarget(new wxLogTextCtrl(logText));
    }
};

class MyApp : public wxApp
{
    utils::EnvVars envVars{};
    utils::LogFileHandler logFileHandler{};

    std::unique_ptr<Tracker> tracker;
    std::unique_ptr<GUI> gui;
    bool attached_console_ = false;
    UserConfig userConfig;
    CalibrationConfig calibConfig;
    ArucoConfig arucoConfig;
    Localization lc;
    MyLogFrame* LogFrame = nullptr;
    
    

public:
    void attachConsole();
    bool ConnectToSlimeVr = false;
    HANDLE SlimeVrPipeHandle = INVALID_HANDLE_VALUE;

    //int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow);
    void print(char* buf) {
        LogFrame->logText->SetLabelText(buf);
    };
    int OnExit() override;
    bool OnInit() override;

#ifdef ATT_DEBUG
    void OnFatalException() override;
    void OnUnhandledException() override;
    bool OnExceptionInMainLoop() override;
#endif
};

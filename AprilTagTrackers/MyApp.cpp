#include "MyApp.hpp"

#include "utils/Assert.hpp"
#include "utils/Log.hpp"
#include "pipes.h"
#include <opencv2/core/utils/logger.hpp>

wxIMPLEMENT_APP(MyApp); // NOLINT

int MyApp::OnExit()
{
    tracker->Stop();

    if (envVars.IsRedirectConsoleToFile())
    {
        logFileHandler.CloseAndTimestampFile();
    }
    return 0;
}

void MyApp::attachConsole()
{
    if (attached_console_ ) {
        return;
    }
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        attached_console_ = true;
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif
}



bool MyApp::OnInit()
{

    LogFrame = new MyLogFrame();
    LogFrame->Show();

    wxLogWarning("warning");
    wxLogMessage("message");
    wxLogError("error");
    LogFrame->logText->SetLabelText("dwsadwadjk wiojk");

   // char* pipe_path = (char*)malloc(500);
  //  sprintf(pipe_path, "%s%d", "\\\\.\\pipe\\tparser_main_pipe_id_", 0);
    if (ConnectToSlimeVr) {
        wchar_t pipe_path[500];
        swprintf(pipe_path, L"%s%d", L"\\\\.\\pipe\\slimevr_april_pipe_", 0);
        LPCWSTR P = L"\\\\.\\pipe\\slimevr_april_pipe_0";
        SlimeVrPipeHandle = create_pipe(P, true, true, 0, 0);
        char buf[1] = { 99 };
        DWORD nbWritten = 0;
        if (SlimeVrPipeHandle != INVALID_HANDLE_VALUE) {
            pipe_write(SlimeVrPipeHandle, buf, 1, nbWritten);
            buf[0] = 0;
            pipe_read(SlimeVrPipeHandle, buf, 1, nbWritten);
            if (buf[0] == 99)
                wxLogMessage("Connection to Slime VR succesful");

        }
    }

    // OnAssertFailure(const wxChar* file, int line, const wxChar* func, const wxChar* cond, const wxChar* msg);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_INFO);

    if (envVars.IsRedirectConsoleToFile())
    {
        logFileHandler.RedirectConsoleToFile();
    }

    ATT_LOG_INFO("Starting AprilTagTrackers");

    userConfig.Load();

    // The next two lines were added as a quick fix. The two options should be handeled differently from other parameters, so as a quick fix, they are reset on every launch of ATT.
    // since disable openvr api isnt part of the parameters, it isnt loaded properly. This ensures it is globaly disabled on every launch.
    userConfig.disableOpenVrApi = false;

    calibConfig.Load();
    arucoConfig.Load();
    lc.LoadLang(userConfig.langCode);

    tracker = std::make_unique<Tracker>(userConfig, calibConfig, arucoConfig, lc);
    gui = std::make_unique<GUI>(tracker, lc, userConfig);
    gui->MainApp = this;

    return true;
}

#ifdef ATT_DEBUG

#    define ATT_FATAL_EXCEPTION(p_throwExpr, p_context)            \
        do                                                         \
        {                                                          \
            try                                                    \
            {                                                      \
                (p_throwExpr);                                     \
            }                                                      \
            catch (const std::exception& exc)                      \
            {                                                      \
                ATT_LOG_ERROR(p_context, ": ", exc.what());        \
                ATT_ABORT();                                       \
            }                                                      \
            catch (...)                                            \
            {                                                      \
                ATT_LOG_ERROR(p_context, ": malformed exception"); \
                ATT_ABORT();                                       \
            }                                                      \
            ATT_LOG_ERROR(p_context, ": expected exception");      \
            ATT_ABORT();                                           \
        } while (false)

void MyApp::OnFatalException()
{
    ATT_FATAL_EXCEPTION(RethrowStoredException(), "wxApp::OnFatalException");
}
void MyApp::OnUnhandledException()
{
    ATT_FATAL_EXCEPTION(RethrowStoredException(), "wxApp::OnUnhandledException");
}
bool MyApp::OnExceptionInMainLoop()
{
    ATT_FATAL_EXCEPTION(RethrowStoredException(), "wxApp::OnExceptionInMainLoop");
    return true;
}

// cv::ErrorCallback
static int OpenCVErrorHandler(int status, const char* funcName, const char* errMsg, const char* fileName, int line, void*)
{
    ATT_LOG_ERROR_AT(fileName, line, "OpenCV Error(", status, "): ", errMsg, "\nin  ", funcName);
    ATT_ABORT();
    return 0;
}

// wxAssertHandler_t
static void wxWidgetsAssertHandler(const wxString& file, int line, const wxString& func, const wxString& cond, const wxString& msg)
{
    ATT_LOG_ERROR_AT(file.c_str().AsChar(), line, "wxWidgets Error: ", msg,
                     "\nassertion failure  ( ", cond, " )  in  ", func);
    ATT_ABORT();
}

static inline const bool overrideErrorHandlers = []
{
    cv::redirectError(&OpenCVErrorHandler);
    wxSetAssertHandler(&wxWidgetsAssertHandler);
    return true;
}();

#endif

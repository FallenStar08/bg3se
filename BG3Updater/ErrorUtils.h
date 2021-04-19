#pragma once

#include <memory>
#include <optional>
#include <vector>

#if defined(_DEBUG)
#define HAS_DEBUG_LOGGING
#endif

enum class GameState : uint32_t
{
	Unknown = 0,
	Init,
	InitMenu,
	InitNetwork,
	InitConnection,
	Idle,
	LoadMenu,
	Menu,
	Exit,
	SwapLevel,
	LoadLevel = 10,
	LoadModule,
	LoadSession,
	LoadGMCampaign,
	UnloadLevel,
	UnloadModule,
	UnloadSession,
	Paused,
	PrepareRunning,
	Running,
	Disconnect = 20,
	Join,
	StartLoading,
	StopLoading,
	StartServer,
	Movie,
	Installation,
	GameMasterPause,
	ModReceiving,
	Lobby = 30,
	BuildStory
};

struct EoCClientState
{
	uint8_t Unkn[16];
	GameState State;
};

struct eclEoCClient
{
	uint8_t Unkn[0x60];
	EoCClientState ** State;
};

struct THREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
};


void DebugMsg(char const *);

template <typename... Args>
void Debug(char const * fmt, Args... args)
{
	char buf[1024];
	int length = sprintf_s(buf, 1024 - 1, fmt, args...);
	buf[length++] = 0;
	DebugMsg(buf);
}

#if defined(HAS_DEBUG_LOGGING)
#define DEBUG(msg, ...) Debug(__FUNCTION__ "(): " msg, __VA_ARGS__)
#else
#define DEBUG(msg, ...)
#endif

class ErrorUtils
{
public:
	ErrorUtils();

	void ShowError(wchar_t const * msg) const;
	std::optional<GameState> GetState() const;

	void SuspendClientThread() const;
	void ResumeClientThread() const;

private:
	typedef void(*EoCClient__HandleError)(void * self, std::wstring const& message, bool exitGame, std::wstring const& a4);

	struct ThreadInfo
	{
		DWORD ThreadId;
		std::string Name;
	};

	HMODULE eocApp_{ NULL };
	uint8_t const * moduleStart_{ nullptr };
	std::size_t moduleSize_{ 0 };
	std::vector<ThreadInfo> threads_;

	eclEoCClient ** EoCClient{ nullptr };
	EoCClient__HandleError EoCClientHandleError{ nullptr };

	bool FindModule();
	void FindErrorFuncs();
	bool CanShowError() const;

	bool ShowErrorDialog(wchar_t const * msg) const;
	void ClientHandleError(wchar_t const * msg, bool exitGame) const;

	ThreadInfo const * FindClientThread() const;
	static LONG NTAPI ThreadNameCaptureFilter(_EXCEPTION_POINTERS *ExceptionInfo);
};

extern std::unique_ptr<ErrorUtils> gErrorUtils;

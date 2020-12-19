#pragma once
#include "Console.h"
#if _WIN32
#include <mfapi.h>
#include <mfidl.h>
#endif //_WIN32

// Class Prototype

class DevicesManager {
protected:
	// Variables


public:
	// Variables

	// Constructors
	DevicesManager();
	// Functions
	void InitializeDevicesManager();
	void ShowDevices();
};
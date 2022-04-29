#pragma once
int SerialBegin(int BaudRate, int Comport) {
	DCB SerialParams;
	COMMTIMEOUTS SerialTimeouts;

	CloseHandle(connectedPort);
	connectedPort = CreateFileA(
		("\\\\.\\COM" + std::to_string(Comport)).c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (connectedPort == INVALID_HANDLE_VALUE) { return -4; } // no port

	SerialParams.DCBlength = sizeof(SerialParams);
	if (!GetCommState(connectedPort, &SerialParams)) { return -3; } // GetState error

	SerialParams.BaudRate = BaudRate;
	SerialParams.ByteSize = 8;
	SerialParams.StopBits = ONESTOPBIT;
	SerialParams.Parity = NOPARITY;
	if (!SetCommState(connectedPort, &SerialParams)) { return -2;  } // SetState error

	SerialTimeouts.ReadIntervalTimeout = 1;
	SerialTimeouts.ReadTotalTimeoutConstant = 1;
	SerialTimeouts.ReadTotalTimeoutMultiplier = 1;
	SerialTimeouts.WriteTotalTimeoutConstant = 1;
	SerialTimeouts.WriteTotalTimeoutMultiplier = 1;
	if (!SetCommTimeouts(connectedPort, &SerialTimeouts)) { return -1; }  //SetTimeouts error

	return 0;  // OK
}

void ConnectRequest(void) {
	if (isConnected) {
		CloseHandle(connectedPort);
		SetWindowStatus("Disconnected");
		isConnected = false;
		return;
	}

	switch (SerialBegin(targetBoudRate, selectedPort))
	{
	case -4: SetWindowStatus("Device not connected!"); break;
	case -3: SetWindowStatus("GetState error!"); break;
	case -2: SetWindowStatus("SetState error!"); break;
	case -1: SetWindowStatus("SetTimeouts!"); break;
	case  0: 
		SetWindowStatus("Connected to: COM " + std::to_string(selectedPort));
		isConnected = true;
		return;
	}

	CloseHandle(connectedPort);
}

void SerialRead(void) {
	if (!isConnected) { return; }
	if (!SetCommMask(connectedPort, EV_RXCHAR)) { ConnectRequest(); return; }
	DWORD BytesIterated;

	if (ReadFile(connectedPort, Buffer, 11, &BytesIterated, NULL)) {
		SetWindowTextA(hEditControl, Buffer);
	}
}

void SerialWrite(char* buffer, int lenth) {
	if (!isConnected) { return; }
	DWORD BytesIterated;
	WriteFile(connectedPort, buffer, lenth, &BytesIterated, NULL);
}
#pragma once

#pragma pack(1)

enum VisionStatus
{
	VisionStatusReady = 0,
	VisionStatusInspecting = 1,
	VisionStatusInspectFinish = 2,
	VisionStatusStop = 3,
	VisionStatusError = 4
};

struct VisionStatusPacket
{
	short Type;		// VisionStatus
	short Length;	// Packet Length
	union
	{
		struct _Inspecting
		{
			short SlotNumber;
			short SlotCount;
			char RFID[17];	// RFID: 최대 16개 문자 + NULL
			long MagazineID[2];	// 0: 2D, 1: 3D
			short RecipeNumber;
			char RecipeName[128];
		} Inspecting;
		struct _Error
		{
			char Message[256];
		} Error;
		struct _InspectFinish
		{
			short SlotNumber;
			short SlotCount;
			char RFID[17];	// RFID: 최대 16개 문자 + NULL
			long MagazineID[2];	// 0: 2D, 1: 3D
			short RecipeNumber;
			char RecipeName[128];
			char Barcode[42]; // Barcode: 최대 41개 문자 + NULL
			long StripID[2];	// 0: 2D, 1: 3D
		} InspectFinish;
		char Dummy[256];
	} Body;
};

//const int VisionStatusPacketHeaderSize = ((int)&((VisionStatusPacket *)0)->Body);
const size_t VisionStatusPacketHeaderSize = (size_t)(uintptr_t)&((VisionStatusPacket *)0)->Body;

#pragma pack()
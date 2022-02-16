#include <iostream>
#include <Windows.h>

#include "Memory.h"
#include "Structs.h"
#include "Drawing.h"
#include "Offsets.h"

using namespace std;

int ScreenWidth = 1920;
int ScreenHeight = 1080;

bool Aimbot = false;

int main() {

	HDC hdc = GetDC(FindWindowA(NULL, "Apex Legends"));

	process_id = get_process_id("r5apex.exe");
	base_address = get_module_base_address("r5apex.exe");

	cout << "Process id found at " << process_id << endl;
	cout << "Process Base Address found at 0x" << hex << base_address << endl;

	while (true) {

		Matrix ViewMatrix = Read<Matrix>(base_address + Offsets::ViewMatrix);
		uintptr_t LocalPlayer = Read<uintptr_t>(base_address + Offsets::LocalPlayer);

		for (int Entity = 0; Entity < 100; Entity++) {
			uintptr_t Base = Read<uintptr_t>(base_address + Offsets::EntityList + (Entity << 5));

			Vector3 LocalOrigin = Read<Vector3>(LocalPlayer + Offsets::Origin);
			Vector3 Origin = Read<Vector3>(Base + Offsets::Origin);

			float Distance = (Vector3::Distance(LocalOrigin, Origin)) / 36.5f;

			char Dist[100];
			snprintf(Dist, sizeof(Dist), "%f", Distance);

			DrawCrosshair(hdc, ScreenWidth, ScreenHeight, 60);//Draw CrossHasir
			if ((int)Distance <= 500) {
				int OwnTeam = Read<int>(LocalPlayer + Offsets::TeamId);
				int Team = Read<int>(Base + Offsets::TeamId);
				float Health = Read<int>(Base + Offsets::Health);
				float MaxHealth = Read<int>(Base + Offsets::MaxHealth);
				int Knocked = Read<int>(Base + Offsets::Knocked);
				int Dead = Read<int>(Base + Offsets::Dead);

				Vector3 HeadPos;
				HeadPos.x = Origin.x;
				HeadPos.y = Origin.y;
				HeadPos.z = Origin.z + 75.f;

				Vector3 OriginPos = WorldToScreen(Origin, ViewMatrix, ScreenWidth, ScreenHeight);
				Vector3 HeadLocation = WorldToScreen(HeadPos, ViewMatrix, ScreenWidth, ScreenHeight);

				float height = HeadLocation.y - OriginPos.y;
				float width = height / 2.4f;

				//Draw Esp
				if (OriginPos.z >= 0.01f && Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0) {
					if (Knocked > 0) {
						DrawKnockedString(hdc, OriginPos.x, OriginPos.y + 30, "Knocked");//Draw a string when enemy is knocked
						DrawKnockedString(hdc, OriginPos.x, OriginPos.y + 10, Dist);//Draw Ditance of enemy
						DrawKnockedRectangle(hdc, OriginPos.x - (width / 2), OriginPos.y, width, height, 1);//Draw a Rectangular box over enemy
						DrawKnockedLine(hdc, ScreenWidth / 2, ScreenHeight / 8, HeadLocation.x, HeadLocation.y);//Draw a line from screen top to enemy head
					} else {
						DrawNotSeenString(hdc, OriginPos.x, OriginPos.y + 10, Dist);//Draw Ditance of enemy
						DrawNotSeenRectangle(hdc, OriginPos.x - (width / 2), OriginPos.y, width, height, 1);//Draw a Rectangular box over enemy
						DrawNotSeenLine(hdc, ScreenWidth / 2, ScreenHeight / 8, HeadLocation.x, HeadLocation.y);//Draw a line from screen top to enemy head
					}
				}
			}
		}

		if (GetAsyncKeyState(VK_DELETE)) {
			break;
		}
	}
}

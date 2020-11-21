// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "dx_overlay.h"
#include "dx_renderer.h"
#include <vector>
#include <math.h>
#include <stdio.h>
#include "vector.h"


typedef struct
{
	float x, y, z, w;
} vec4d_f;

vec4d_f clipCoords;
vec4d_f NDC;

char text[999];

float matrix[16];
DWORD pid;
HANDLE ghandle;
HWND ghwnd;


namespace offsets
{

	DWORD basepoint_camera = 0x0EE74A4;
	DWORD view_angle_x =  0x3C;
	DWORD view_angle_y = view_angle_x + 0x4;

	DWORD num_enemy = 0x0F7CD3C;
	DWORD basepoint_localplayer = 0x0F5E384;

	DWORD off_name = 0x14;
	DWORD off_health = 0x340;
	DWORD off_max_health = 0x344;
	DWORD off_pos_x = 0x394;
	DWORD off_pos_y = off_pos_x + 0x4;
	DWORD off_pos_z = off_pos_y + 0x4;
	
}
using namespace std;



template <class T>
T Read(DWORD address)
{
	T VALUE;
	ReadProcessMemory(ghandle, (LPVOID)(address), &VALUE, sizeof(T), 0);
	return VALUE;
}

std::wstring readpChar(DWORD address) {
	try {
		if (address != 0) {
			const size_t namesize = 200;
			char x[namesize];
			ReadProcessMemory(ghandle, (LPCVOID)address, &x, namesize, NULL);
			std::wstring tmpname = std::wstring(&x[0], &x[namesize]);
			wchar_t* czech = wcstok(&tmpname[0], L"\0");
			if (czech != nullptr) return czech;
		}
	}
	catch (const std::exception& exc) {}
	return std::wstring(L"\0");
}

bool WorldToScreenGl(Vec3 pos, Vec3& screen, float matrix[16], int windowWidth, int windowHeight)
{
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
	{
		//std::cout << "clipCoords.w <: 0.1f" << clipCoords.w << "\n";
		return false;
	}

	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

	return true;
}




#include <codecvt>

std::wstring stringToWstring(const std::string& t_str)
{

	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.from_bytes(t_str);
}

void GetAimAngles(Vec3 vLocalPosition, Vec3 vTargetPosition, float* pflYaw, float* pflPitch) 
{
	Vec3 vDelta = (vTargetPosition - vLocalPosition);
	vDelta.Normalize();
	*pflYaw = -atan2(vDelta.x, vDelta.y) / 3.141592653589793F * 180;
	*pflPitch = atan2(vDelta.z, sqrt(vDelta.x * vDelta.x + vDelta.y * vDelta.y)) / 3.141592653589793F * 180;
}

int main()
{
	SetConsoleTitle("LEET:.... ");
	forceinline::dx_overlay overlay(L"SDL_app", L"Cube 2: Sauerbraten", true /*Read comments regarding this*/);
	if (!overlay.is_initialized())
		return -1;

	MSG m;
	ZeroMemory(&m, sizeof(m));

	forceinline::dx_renderer renderer = overlay.create_renderer();

	ghwnd = FindWindowA("SDL_app", 0);
	GetWindowThreadProcessId(ghwnd,&pid);
	ghandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	

	while (m.message != WM_QUIT)
	{
		if (PeekMessage(&m, overlay.get_overlay_wnd(), NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&m);
			DispatchMessage(&m);
		}

		renderer.begin_rendering();


	
		DWORD localplayer = Read<DWORD>(offsets::basepoint_localplayer);
		DWORD camera_base = Read<DWORD>(offsets::basepoint_camera);

		localplayer = Read<DWORD>(localplayer);
		localplayer = Read<DWORD>(localplayer + 0x110);


		int health = Read<int>(localplayer + offsets::off_health);
		Vec3 local_pos = Read<Vec3>(localplayer + offsets::off_pos_x);


		Vec3 local_pos_head = Read<Vec3>(0x00F77E70);
		
		Vec3 disvechead = local_pos_head - local_pos;
	
		//float local_viewangle_x = Read<float>(camera_base + offsets::view_angle_x);
		//float local_viewangle_y = Read<float>(camera_base + offsets::view_angle_y);



		
		ReadProcessMemory(ghandle, LPVOID(0xF77AF0), &matrix, sizeof(matrix), 0);
	
		float flClosestDist = INFINITY;
		DWORD pAimTarget;
		DWORD entitylist = Read<DWORD>(localplayer + 0x458);
		int numenemy = Read<int>(offsets::num_enemy);
		Vec3 location_enemy;
		
		for (int i = 0; i < numenemy; i++)
		{
			DWORD ent = Read<DWORD>(entitylist + (i * 4));
			if (!ent)
				continue;
			int ent_health = Read<int>(ent + offsets::off_health);
	
			if (ent_health <= 0)
				continue;
			location_enemy = Read<Vec3>(ent + offsets::off_pos_x);

			Vec3 location_enemy_head = location_enemy+ disvechead;
			Vec3 location_enemy_out;
			wstring nameplayer = readpChar(ent + offsets::off_name);
			
			if (WorldToScreenGl(location_enemy, location_enemy_out,matrix,800,600))
			{
				
				sprintf(text,"%d", ent_health);
				
				renderer.draw_text(L"name :"+nameplayer +L"\n"+L"health :"+ stringToWstring(text), location_enemy_out.x, location_enemy_out.y, 0xFFFFFFFF, false);
			}

			float flDist = location_enemy_head.dist(local_pos_head);
			if (flDist < flClosestDist)
			{
				flClosestDist = flDist;
				pAimTarget = ent;
				
			}

		
		}


	
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) 
		{
			if (pAimTarget)
			{
				

			
			Vec3 location_enemyx = Read<Vec3>(pAimTarget + offsets::off_pos_x);
			Vec3 vAimPos = location_enemyx + disvechead;
	
			float x, y;
			GetAimAngles(local_pos_head, vAimPos, &x, &y);
				
			
				WriteProcessMemory(ghandle, (LPVOID)(camera_base + offsets::view_angle_x), &x, sizeof(x), 0);
				WriteProcessMemory(ghandle, (LPVOID)(camera_base + offsets::view_angle_y), &y, sizeof(y), 0);
		
			}
		}



		renderer.end_rendering();

	}



	return 0;
}


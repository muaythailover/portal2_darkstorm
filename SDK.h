#pragma once

#include <windows.h>
#include <math.h>
#include <xstring>
#include "Vector.h"
#include "getvfunc.h"
#include "dt_recv2.h"
#include "CBaseAPI.h"
#include "WeaponList.h"
#include "CGlobalVars.h"
#include "VMTHooks.h"

using namespace std;

typedef void* ( __cdecl* CreateInterface_t )( const char*, int* );
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

#define VMTManager toolkit::VMTManager
#define VMTBaseManager toolkit::VMTBaseManager

#define WIN32_LEAN_AND_MEAN
#pragma optimize("gsy",on)
#pragma warning( disable : 4244 ) //Possible loss of data

typedef float matrix3x4[3][4];

#define me gInts.Engine->GetLocalPlayer()
#define GetBaseEntity gInts.EntList->GetClientEntity
#define MASK_AIMBOT 0x200400B
#define	FL_ONGROUND (1<<0)
#define FL_DUCKING (1<<1)
#define CONTENTS_HITBOX 0x40000000
#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define square( x ) ( x * x )
#define RADPI 57.295779513082f
#define SQUARE( a ) a*a
#define BLU_TEAM 3
#define RED_TEAM 2

typedef struct player_info_s
{
	char			name[32];
	int				userID;
	char			guid[33];
	unsigned long	friendsID;
	char			friendsName[32];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[4];
	unsigned char	filesDownloaded;
} player_info_t;

class ClientClass
{
private:
	BYTE _chPadding[8];
public:
	char* chName;
	RecvTable* Table;
	ClientClass* pNextClass;
	int iClassID;
};

class CHLClient
{
public:
	ClientClass* GetAllClasses( void )
	{
		typedef ClientClass* ( __thiscall* OriginalFn )( PVOID ); //Anything inside a VTable is a __thiscall unless it completly disregards the thisptr. You can also call them as __stdcalls, but you won't have access to the __thisptr.
		return getvfunc<OriginalFn>( this, 8 )( this ); //Return the pointer to the head CClientClass.
	}
};

class CGlobals
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxclients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};

class CUserCmd
{
public:
	virtual ~CUserCmd() {}; //Destructor 0 - 4
	int command_number; //4 - 8
	int tick_count; //8 - C
	Vector viewangles; //C - 18
	float forwardmove; //24 - 28
	float sidemove; //28 - 2C
	float upmove; //2C - 30
	int	buttons; //30 - 34
	byte impulse; //34 - 35 
	int weaponselect; //35 - 39
	int weaponsubtype; //39 - 3D
	int random_seed; //3D - 41
	short mousedx; //41 - 43
	short mousedy; //43 - 45
	bool hasbeenpredicted; //45 - 46
	Vector headangles; //46 - 52
	Vector headoffset; //52 - 5E
};

class IGameResources
{
public:
	virtual ~IGameResources() {};

	virtual const char* GetTeamName(int iTeamIndex) = 0;
	virtual int GetTeamScore(int iTeamIndex) = 0;
	virtual const DWORD& GetTeamColor(int iTeamIndex) = 0;
	virtual bool IsConnected(int iEntityIndex) = 0;
	virtual bool IsAlive(int iEntityIndex) = 0;
	virtual bool IsFakePlayer(int iEntityIndex) = 0;
	virtual bool IsLocalPlayer(int iEntityIndex) = 0;
	virtual const char* GetPlayerName(int iEntityIndex) = 0;
	virtual int GetPlayerScore(int iEntityIndex) = 0;
	virtual int GetPing(int iEntityIndex) = 0;
	virtual int GetDeaths(int iEntityIndex) = 0;
	virtual int GetFrags(int iEntityIndex) = 0;
	virtual int GetTeam(int iEntityIndex) = 0;
	virtual int GetHealth(int iEntityIndex) = 0;
};

class CBaseEntity
{
public:
	Vector& GetAbsOrigin( )
	{
		typedef Vector& ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>(this, 10)(this);
	}
	Vector& GetAbsAngles( )
	{
		typedef Vector& ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>(this, 11)(this);
	}
	void GetWorldSpaceCenter( Vector& vWorldSpaceCenter)
	{
		Vector vMin, vMax;
		this->GetRenderBounds( vMin, vMax );
		vWorldSpaceCenter = this->GetAbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2;
	}
	DWORD* GetModel( )
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef DWORD* ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( pRenderable, 8 )( pRenderable );
	}
	bool SetupBones( matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef bool ( __thiscall* OriginalFn )( PVOID, matrix3x4*, int, int, float );
		return getvfunc<OriginalFn>( pRenderable, 13 )( pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime );
	}
	int GetSkin()
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef int (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 37)(pRenderable);
	}
	ClientClass* GetClientClass( )
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef ClientClass* ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( pNetworkable, 2 )( pNetworkable );
	}
	bool IsDormant( )
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( pNetworkable, 9 )( pNetworkable );
	}
	int GetIndex( )
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef int ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( pNetworkable, 10 )( pNetworkable );
	}
	const char* GetPlayerName();
private:
	void GetRenderBounds( Vector& mins, Vector& maxs )
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef void ( __thiscall* OriginalFn )( PVOID, Vector& , Vector& );
		getvfunc<OriginalFn>( pRenderable, 17 )( pRenderable, mins, maxs );
	}
};

class EngineClient
{
public:
	void GetScreenSize( int& width, int& height )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int& , int& );
		return getvfunc<OriginalFn>( this, 5 )( this, width, height );
	}
	bool GetPlayerInfo( int ent_num, player_info_t *pinfo )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID, int, player_info_t * );
		return getvfunc<OriginalFn>(this, 8)(this, ent_num, pinfo );
	}
	bool Con_IsVisible( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 11 )( this );
	}
	int GetLocalPlayer( void )
	{
		typedef int ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 12 )( this );
	}
	float Time( void )
	{
		typedef float ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 14 )( this );
	}
	void GetViewAngles( Vector& va )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, Vector& va );
		return getvfunc<OriginalFn>( this, 18 )( this, va );
	}
	void SetViewAngles( Vector& va )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, Vector& va );
		return getvfunc<OriginalFn>( this, 19 )( this, va );
	}
	int GetMaxClients( void )
	{
		typedef int ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 20 )( this );
	}
	bool IsInGame( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 25 )( this );
	}
	bool IsConnected( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 26 )( this );
	}
	bool IsDrawingLoadingImage( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 27 )( this );
	}
	const matrix3x4& WorldToScreenMatrix( void )
	{
		typedef const matrix3x4& ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>(this, 37)(this);
	}
	bool IsTakingScreenshot( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 88 )( this );
	}
	DWORD* GetNetChannelInfo( void )
	{
		typedef DWORD* ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 74 )( this );
	}
	void ClientCmd_Unrestricted( const char* chCommandString )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, const char * );
		return getvfunc<OriginalFn>( this, 108 )( this, chCommandString );
	}
};

class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel)
	{
		typedef const char* ( __thiscall* OriginalFn )( PVOID, unsigned int );
		return getvfunc<OriginalFn>( this, 36 )( this, vguiPanel );
	}
};

class ISurface
{
public:
	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int, int, int );
		getvfunc<OriginalFn>( this, 14 )( this, r, g, b, a );
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int, int, int );
		getvfunc<OriginalFn>( this, 15 )( this, x0, y0, x1, y1 );
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int, int, int );
		getvfunc<OriginalFn>( this, 17 )( this, x0, y0, x1, y1 );
	}
	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long);
		getvfunc<OriginalFn>( this, 22 )( this, font );
	}
	void DrawSetTextColor(int r, int g, int b, int a )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int, int, int );
		getvfunc<OriginalFn>( this, 24 )( this, r, g, b, a );
	}
	void DrawSetTextPos(int x, int y )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int );
		getvfunc<OriginalFn>( this, 25 )( this, x, y );
	}
	void DrawPrintText(const wchar_t *text, int textLen )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, const wchar_t *, int, int );
		return getvfunc<OriginalFn>( this, 27 )( this, text, textLen, 0 );
	}
	unsigned long CreateFont( )
	{
		typedef unsigned long ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>(this, 69)(this);
	}
	void SetFontGlyphSet (unsigned long font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		getvfunc<OriginalFn>(this, 70)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
};

class CModelInfo
{
public:
	const char *GetModelName(DWORD* model)
	{
		typedef const char* (__thiscall* OriginalFn)(PVOID, DWORD*);
		return getvfunc<OriginalFn>(this, 3)(this, model);
	}
};

class CEntList
{
public:
	CBaseEntity* GetClientEntity( int entnum )
	{
		typedef CBaseEntity* ( __thiscall* OriginalFn )( PVOID, int );
		return getvfunc<OriginalFn>( this, 3 )( this, entnum );
	}
	CBaseEntity* GetClientEntityFromHandle( int hEnt )
	{
		typedef CBaseEntity* ( __thiscall* OriginalFn )( PVOID, int );
		return getvfunc<OriginalFn>( this, 4 )( this, hEnt );
	}
	int GetHighestEntityIndex(void)
	{
		typedef int ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 6 )( this );
	}
};

enum playercontrols
{
	IN_ATTACK = (1 << 0),
	IN_JUMP	= (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_ATTACK2 = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_ALT1 = (1 << 14),
	IN_ALT2 = (1 << 15),
	IN_SCORE = (1 << 16),	// Used by client.dll for when scoreboard is held down
	IN_SPEED = (1 << 17),	// Player is holding the speed key
	IN_WALK = (1 << 18),	// Player holding walk key
	IN_ZOOM	= (1 << 19),	// Zoom key for HUD zoom
	IN_WEAPON1 = (1 << 20),	// weapon defines these bits
	IN_WEAPON2 = (1 << 21),	// weapon defines these bits
	IN_BULLRUSH = (1 << 22),
};

enum source_lifestates
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

class CInput
{
public:
	CUserCmd* GetUserCmd( int seq )
	{
		typedef CUserCmd* ( __thiscall* OriginalFn )( PVOID, int );
		return getvfunc<OriginalFn>( this, 8 )( this, seq );
	}
};

#define XASSERT( x ) if( !x ) MessageBoxW( 0, L#x, 0, 0 );

class CInterfaces
{
public:
	CInput* Input;
	CEntList* EntList;
	EngineClient* Engine;
	IPanel* Panels;
	ISurface* Surface;
	CHLClient* Client;
	CModelInfo* ModelInfo;
	PDWORD ClientMode;
};

extern CInterfaces gInts;
extern CPlayerVariables gPlayerVars;
extern COffsets gOffsets;
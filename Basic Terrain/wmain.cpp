#include "Game.h"


int WINAPI wWinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int iShow
)
{
	if ( !GAME->Initialize( hInstance, false ) )
	{
		return 1;
	}
	GAME->Run( );
	GAME->Shutdown( );
	return 0;
}
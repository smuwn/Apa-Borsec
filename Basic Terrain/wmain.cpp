#include "Game.h"


int WINAPI wWinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int iShow
)
{
	if ( !GAME->Initialize( hInstance ) )
	{
		return 1;
	}
	GAME->Run( );
	GAME->Shutdown( );
}
#include "Game.h"


int WINAPI wWinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int iShow
)
{
	srand( ( unsigned int ) time( NULL ) );
	if ( !GAME->Initialize( hInstance, 
#if DEBUG || _DEBUG
		false 
#else
		true
#endif
		) )
	{
		return 1;
	}
	GAME->Run( );
	GAME->Shutdown( );
	return 0;
}
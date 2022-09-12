#pragma once


namespace DawnState
{
	enum DawnState
	{
		NoState,
		MainMenu,
		ChooseClass,
		LoadingScreen,
		OptionsMenu,
		InGame
	};

	DawnState getDawnState();
	void setDawnState(DawnState state);
}

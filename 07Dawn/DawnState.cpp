#include "DawnState.h"

namespace DawnState
{
	static DawnState state;

	DawnState getDawnState()
	{
		return state;
	}

	void setDawnState(DawnState state)
	{
		state = state;
	}
}

#pragma once
#include <string>
#include <NsGui/FrameworkElement.h>
#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>

#include "GUI/level-intro-bindings.hpp"

class LevelIntroGrid : public Noesis::Grid {

public:
	LevelIntroGrid();

	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onStartLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

	Noesis::Ptr<LevelIntroBindings> m_bindings;

private:
	NS_DECLARE_REFLECTION(LevelIntroGrid, Grid);
};

class LevelIntro {

public:

	static LevelIntro& Get();
	void init();

	std::string decimalToRoman(int num);
	Noesis::Ptr<LevelIntroGrid> m_grid;

private:
	LevelIntro() = default;

	static LevelIntro s_instance;
};
#pragma once

#include <NsGui/IntegrationAPI.h>
#include <NsGui/Grid.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <Event/EventEmitter.h>

#include "Progression.h"

class LevelIntroBindings : public NoesisApp::NotifyPropertyChangedBase {
public:
	LevelIntroBindings();

	const char* getTitle() const;
	void setTitle(const char* value);

	const char* getImagePath() const;
	void setImagePath(const char* value);

	const char* getText() const;
	void setText(const char* value);

private:
	NS_DECLARE_REFLECTION(LevelIntroBindings, NotifyPropertyChangedBase)

		NsString m_title;
	NsString m_text;
	NsString m_imagePath;
};

class LevelIntroGrid : public Noesis::Grid {

public:

	LevelIntroGrid();

	void InitializeComponent();

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
	void init(EventEmitter& emitter, Progression& progression);

	std::string decimalToRoman(int num);
	Noesis::Ptr<LevelIntroGrid> m_grid;

private:
	LevelIntro() = default;

	static LevelIntro s_instance;
};
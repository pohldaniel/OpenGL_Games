#include <iostream>

#include <NsCore/Nullable.h>
#include <NsGui/Button.h>
#include <NsGui/ResourceKeyString.h>
#include <NsGui/PropertyPath.h>
#include <NsGui/Animation.h>
#include <NsGui/FreezableCollection.h>
#include <NsGui/ResourceDictionary.h>

#include "TitleScreen.h"
#include "EventListener.h"

NS_IMPLEMENT_REFLECTION(TitleScreenGrid) {
	NsMeta<Noesis::TypeId>("TitleScreenGrid");
}

TitleScreenGrid::TitleScreenGrid(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

//https://github.com/bitdesign/OgreBindings/blob/master/Samples/TicTacToe/src/TicTacToeLogic.cpp
void TitleScreenGrid::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "title-screen.xaml");

	Noesis::Ptr<Noesis::ResourceKeyString> key = Noesis::ResourceKeyString::Create("Reveal");
	m_storyBoard = this->FindResource<Noesis::Storyboard>(key.GetPtr());

	/*Noesis::TimelineCollection* animations = m_storyBoard->GetChildren();
	Noesis::Timeline* timeline = animations->Get(0);
	std::cout << "Name: " << m_storyBoard->GetTargetName(timeline) << std::endl;
	for (int i = 0; i < animations->Count(); i++) {
		std::cout << "Name: " << m_storyBoard->GetTargetName(animations->Get(i)) << std::endl;
	}

	Noesis::Timeline* timeline = animations->Get(0);
	Noesis::Ptr<Noesis::DoubleAnimation> anim = *static_cast<Noesis::DoubleAnimation*>(animations->Get(0));



	Noesis::Ptr<Noesis::DoubleAnimation> anim = *new Noesis::DoubleAnimation();
	anim->SetFrom(1.0f);
	anim->SetTo(0.0f);
	anim->SetDuration(Noesis::TimeSpan(0, 0, 0, 0, 500));

	Noesis::Storyboard::SetTarget(anim.GetPtr(), m_storyBoard);
	Noesis::Ptr<Noesis::PropertyPath> path = *new Noesis::PropertyPath("Opacity");
	Noesis::Storyboard::SetTargetProperty(anim.GetPtr(), path.GetPtr());

	Noesis::Ptr<Noesis::Storyboard> pStoryboard = *new Noesis::Storyboard();
	pStoryboard->GetChildren()->Add(anim.GetPtr());*/
}

bool TitleScreenGrid::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onStartGame);
	NS_CONNECT_EVENT(Noesis::Button, Click, onQuitGame);
	return false;
}

void TitleScreenGrid::onStartGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameStateNew>(States::LEVELINTRO);
}

void TitleScreenGrid::onQuitGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ApplicationExit>();
}

TitleScreen TitleScreen::s_instance;

TitleScreen& TitleScreen::Get() {
	return s_instance;
}

void TitleScreen::init(EventEmitter& emitter, Progression& progression) {
	m_grid = *new TitleScreenGrid(emitter);
}

void TitleScreen::clearAnimations() {
	m_grid->m_storyBoard->GetChildren()->Clear();
}
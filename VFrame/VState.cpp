#include "VState.h"
#include "VGlobal.h"

using std::vector;

VState::VState() : VGroup()
{
	
}

VState::~VState()
{
	Cleanup();
}

void VState::Initialise()
{
	Cameras.emplace_back();
	Cameras[0] = new VCamera();
	DefaultCamera = Cameras[0];
	TimeManager = new VTimeManager();
}

void VState::Cleanup()
{
	if (exists)
	{
		Destroy();

		for (unsigned int i = 0; i < Cameras.size(); i++)
		{
			delete Cameras[i];
			Cameras[i] = nullptr;
		}

		Cameras.clear();
		Cameras.shrink_to_fit();

		delete TimeManager;

		VLog("State cleanup successful");
	}
}

void VState::Update(float dt)
{
	VSUPERCLASS::Update(dt);
	TimeManager->Update(dt);
}

void VState::OpenSubState(VSubState* subState)
{
	if (VGlobal::p()->Async->ActiveAsyncFunctions())
	{
		VLogError("Cannot modify substate while async functions are active.");
		return;
	}

	if (SubState) //If substate is already active, close it and reset the substate.
	{
		CloseSubState();
		ResetSubState();
	}

	SubState = subState;
	openSubState = true;

	VLog("Substate open.");
}

void VState::CloseSubState()
{
	if (VGlobal::p()->Async->ActiveAsyncFunctions())
	{
		VLogError("Cannot modify substate while async functions are active.");
		return;
	}

	if (SubState)
	{
		if (SubState->OnClose)
			SubState->OnClose();

		active = true;
		closeSubstate = true;

		VLog("Substate closed.");
	}
}

void VState::ResetSubState()
{
	if (openSubState)
	{ 
		if (SubState)
		{
			SubState->ParentState = this;
			SubState->Initialise();
		}
		openSubState = false;
	}

	if (closeSubstate)
	{
		SubState->Cleanup();

		delete SubState;
		SubState = nullptr;

		closeSubstate = false;
	}
}

void VSubState::Cleanup()
{
	if (exists)
	{
		Destroy();
		VLog("Substate cleanup successful.");

		vertices.clear();
		ParentState = nullptr;

		delete TimeManager;
		TimeManager = nullptr;
	}
}

void VSubState::SetFillColour(const sf::Color& colour)
{
	vertices[0].color = colour;
	vertices[1].color = colour;
	vertices[2].color = colour;
	vertices[3].color = colour;
}


/**
* @param colour The background colour of the substate.
*/

VSubState::VSubState(sf::Color colour) : VGroup()
{
	vertices.setPrimitiveType(sf::Quads);
	vertices.resize(4);
	SetFillColour(colour);

	TimeManager = new VTimeManager();
}

VSubState::~VSubState()
{
	Cleanup();
}

void VSubState::Close()
{
	if (ParentState)
		ParentState->CloseSubState();
}

void VSubState::Update(float dt)
{
	VSUPERCLASS::Update(dt);
	TimeManager->Update(dt);
}

void VSubState::Draw(sf::RenderTarget &RenderTarget)
{
	sf::View defaultView = RenderTarget.getDefaultView();
	sf::View currentView = RenderTarget.getView();

	RenderTarget.setView(defaultView);
	vertices[0].position = sf::Vector2f(0, 0);
	vertices[1].position = sf::Vector2f(defaultView.getSize().x, 0);
	vertices[2].position = defaultView.getSize();
	vertices[3].position = sf::Vector2f(0, defaultView.getSize().y);
	RenderTarget.draw(vertices);

	if (UseParentCamera)
		RenderTarget.setView(currentView);

	VSUPERCLASS::Draw(RenderTarget);
	RenderTarget.setView(currentView);
}

VState* VStateManager::CurrentState()
{
	return states.back();
}

void VStateManager::ChangeState(VState* state)
{
	if (states.size())
	{
		states.back()->Cleanup();
		delete states.back();
		states.pop_back();
	}

	state->Initialise();
	states.push_back(state);
}

void VStateManager::PushState(VState* state)
{
	if (states.size())
	{
		states.back()->Pause();
	}

	state->Initialise();
	states.push_back(state);
}

void VStateManager::PopState()
{
	if (states.size())
	{
		states.back()->Cleanup();
		delete states.back();
		states.pop_back();
	}

	if (!states.empty())
	{
		states.back()->Resume();
	}
}

void VStateManager::Clear()
{
	for (unsigned int i = 0; i < states.size(); i++)
	{
		states[i]->Cleanup();
		delete states[i];
		states[i] = nullptr;
	}
	
	states.clear();
	states.shrink_to_fit();

	VBase::VLog("States cleared");
}
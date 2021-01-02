//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_FlowFields.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EEularianPath.h"

using namespace Elite;

//Destructor
App_FlowFields::~App_FlowFields()
{
	SAFE_DELETE(m_pGridGraph);
	for (BaseAgent* pAgent : m_pAgents)
	{
		SAFE_DELETE(pAgent);
	}
}

//Functions
void App_FlowFields::Start()
{
	//Initialization of your application. If you want access to the physics world you will need to store it yourself.
	//----------- CAMERA ------------
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(80.f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(0, 0));
	DEBUGRENDERER2D->GetActiveCamera()->SetMoveLocked(false);
	DEBUGRENDERER2D->GetActiveCamera()->SetZoomLocked(false);

	//initialize agents
	for (int i = 0; i < m_AgentAmount; i++)
	{
		m_pAgents.push_back(new BaseAgent{});
		m_pAgents.at(i)->SetPosition(Elite::Vector2{ randomFloat(0.f,m_TrimWorldSize),randomFloat(0.f,m_TrimWorldSize) });
	}

	//initialize graph
	m_pGridGraph = new Elite::InfluenceMap<Elite::GridGraph< Elite::InfluenceNode, Elite::GraphConnection>>{false};
	m_pGridGraph->InitializeGrid(14, 14, 10, false, true);
}

void App_FlowFields::Update(float deltaTime)
{
	for (BaseAgent* pAgent : m_pAgents)
	{
		pAgent->TrimToWorld(Elite::Vector2{ 0.f,0.f }, Elite::Vector2{m_TrimWorldSize,m_TrimWorldSize });
		pAgent->Update(deltaTime);
	}


	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
		AddInfluenceOnMouseClick(InputMouseButton::eLeft, 100);

	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eRight))
		AddInfluenceOnMouseClick(InputMouseButton::eRight, -100);

	//------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 150;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Flow Fields");
		ImGui::Spacing();
		ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
	

}

void App_FlowFields::Render(float deltaTime) const
{
	m_pGridGraph->SetNodeColorsBasedOnInfluence();
	m_GraphRenderer.RenderGraph(m_pGridGraph, true, false, false, true);
	for (BaseAgent* pAgent: m_pAgents)
	{
		pAgent->Render(deltaTime);
	}
}

void App_FlowFields::AddInfluenceOnMouseClick(Elite::InputMouseButton mouseBtn, float inf)
{
	auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, mouseBtn);
	auto mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Vector2{ (float)mouseData.X, (float)mouseData.Y });

	if (m_pGridGraph->IsNodeValid(m_pGridGraph->GetNodeAtWorldPos(mousePos)->GetIndex()))
	{
		if (m_pGridGraph->GetNodeAtWorldPos(mousePos)->GetInfluence() < 0.f)
		{
			m_pGridGraph->SetInfluenceAtPosition(mousePos, 0.f);
		}
		else
		{
			m_pGridGraph->SetInfluenceAtPosition(mousePos, inf);
		}
	}
}
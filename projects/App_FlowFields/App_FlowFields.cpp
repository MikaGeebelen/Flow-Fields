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
	SAFE_DELETE(m_pBFS);
	for (BaseAgent* pAgent : m_pAgents)
	{
		SAFE_DELETE(pAgent);
	}
	for (Vector2* vec: m_Directions)
	{
		SAFE_DELETE(vec);
	}
	for (NavigationColliderElement* collider: m_Colliders)
	{
		SAFE_DELETE(collider);
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
	m_TrimWorldSize = float(m_ColsRows * m_CellSize)-1.f;
	for (int i = 0; i < m_AgentAmount; i++)
	{
		m_pAgents.push_back(new BaseAgent{});
		m_pAgents.at(i)->SetPosition(Elite::Vector2{ randomFloat(1.f,m_TrimWorldSize),randomFloat(1.f,m_TrimWorldSize) });
	}

	//initialize graph
	m_pGridGraph = new Elite::GridGraph< Elite::InfluenceNode, Elite::GraphConnection>{ false };
	m_pGridGraph->InitializeGrid(m_ColsRows, m_ColsRows, m_CellSize, false, true);
	m_pBFS = new Elite::BFS< Elite::InfluenceNode, Elite::GraphConnection>{m_pGridGraph};
	for (int i = 0; i < m_ColsRows * m_ColsRows; i++)
	{
		m_Directions.push_back(new Vector2{});
	}
}

void App_FlowFields::Update(float deltaTime)
{
	for (BaseAgent* pAgent : m_pAgents)
	{
		pAgent->TrimToWorld(Vector2{ 1.f,1.f }, Vector2{m_TrimWorldSize,m_TrimWorldSize });
		const float agentSpeed{10};
		pAgent->SetLinearVelocity(*m_Directions.at(m_pGridGraph->GetNodeAtWorldPos(pAgent->GetPosition())->GetIndex())* agentSpeed);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), *m_Directions.at(m_pGridGraph->GetNodeAtWorldPos(pAgent->GetPosition())->GetIndex()), agentSpeed, { 0.f,1.f,0.f });
		pAgent->Update(deltaTime);
	}


	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
		SetTargetOnMouseClick(InputMouseButton::eLeft);

	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))
		AddWallOnMouseClick(InputMouseButton::eMiddle);

	//------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 200;
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
		ImGui::Text("Flow Fields");
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("CONTROLS");
		ImGui::Text("left click = set target");
		ImGui::Spacing();
		ImGui::Text("middle mouse = make wall");
		ImGui::Spacing();

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

		ImGui::Text("Render");
		ImGui::Spacing();
		ImGui::Checkbox("VectorField", &m_DrawVectorField);
		ImGui::Spacing();
		ImGui::Checkbox("Grid", &m_DrawGrid);

		ImGui::Spacing();
		ImGui::Separator();
		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

void App_FlowFields::Render(float deltaTime) const
{
	if (m_DrawGrid)
	{
		m_GraphRenderer.RenderGraph(m_pGridGraph, true, false, false, true);
	}
	for (BaseAgent* pAgent : m_pAgents)
	{
		pAgent->Render(deltaTime);
	}
	for (NavigationColliderElement* collider : m_Colliders)
	{
		collider->RenderElement();
	}
	if (m_DrawVectorField)
	{
		for (int c = 0; c < m_ColsRows; c++)
		{
			for (int r = 0; r < m_ColsRows; r++)
			{
				DEBUGRENDERER2D->DrawDirection(m_pGridGraph->GetNodeWorldPos(c, r), *m_Directions.at(m_pGridGraph->GetNode(c, r)->GetIndex()), 3.f, { 1.f,0.f,0.f });
			}
		}
	}
}

void App_FlowFields::AddWallOnMouseClick(Elite::InputMouseButton mouseBtn)
{
	auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, mouseBtn);
	auto mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Vector2{ (float)mouseData.X, (float)mouseData.Y });

	if (m_pGridGraph->IsNodeValid(m_pGridGraph->GetNodeAtWorldPos(mousePos)->GetIndex()))
	{
		if (m_pGridGraph->GetNodeAtWorldPos(mousePos)->GetInfluence() < 0.f)
		{
			InfluenceNode* temp = m_pGridGraph->GetNodeAtWorldPos(mousePos);
			temp->SetInfluence(0.f);//set wall
			for (NavigationColliderElement* collider : m_Colliders)
			{
				if (Elite::DistanceSquared(collider->GetPosition(),mousePos)<float(m_CellSize/2 * m_CellSize/2))
				{
					m_Colliders.erase(std::remove(m_Colliders.begin(), m_Colliders.end(), collider));
					SAFE_DELETE(collider);
				}
			}
			temp = nullptr;	
		}
		else
		{
			InfluenceNode* temp = m_pGridGraph->GetNodeAtWorldPos(mousePos);
			temp->SetInfluence(-100.f);//set wall
			m_Colliders.push_back(new NavigationColliderElement{ m_pGridGraph->GetNodeWorldPos(temp),float(m_CellSize),float(m_CellSize) });
			temp = nullptr;
		}

		m_pBFS->CreateHeatMap(m_pGridGraph->GetNodeAtWorldPos(CurrentTarget));
		//set the new vectors after heatmap update
		SetDirections();
		SetNodeColorsBasedOnHeatMap();
	}
}

void App_FlowFields::SetTargetOnMouseClick(Elite::InputMouseButton mouseBtn)
{
	auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, mouseBtn);
	auto mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Vector2{ (float)mouseData.X, (float)mouseData.Y });

	if (m_pGridGraph->IsNodeValid(m_pGridGraph->GetNodeAtWorldPos(mousePos)->GetIndex()))
	{
		m_pBFS->CreateHeatMap(m_pGridGraph->GetNodeAtWorldPos(mousePos));
		CurrentTarget = mousePos;
		//set the new vectors after heatmap update
		SetDirections();
		SetNodeColorsBasedOnHeatMap();
	}
}

void App_FlowFields::SetNodeColorsBasedOnHeatMap()
{
	const Elite::Color neutralColor{0.2f,0.2f,0.2f};
	const Elite::Color positiceColor{0.f,1.f,1.f};

	float MaxInfluance{};
	for (InfluenceNode* pNode : m_pGridGraph->GetAllActiveNodes())
	{
		if (pNode->GetInfluence() > MaxInfluance)
		{
			MaxInfluance = pNode->GetInfluence();
		}
	}

	for (InfluenceNode* pNode : m_pGridGraph->GetAllActiveNodes())
	{
		float influence = pNode->GetInfluence();
		float relativeInfluence = abs(influence) / MaxInfluance;

		Color nodeColor = Elite::Color{
		Lerp(neutralColor.r, positiceColor.r, relativeInfluence),
		Lerp(neutralColor.g, positiceColor.g, relativeInfluence),
		Lerp(neutralColor.b, positiceColor.b, relativeInfluence) };

		pNode->SetColor(nodeColor);
	}
}

void App_FlowFields::SetDirections() 
{

	for (int c = 0; c < m_ColsRows; c++)
	{
		for (int r = 0; r < m_ColsRows; r++)
		{
			float LowestInfluance{FLT_MAX};
			int LowestInfluanceIndex{-1};
			int originalIndex{ m_pGridGraph->GetNode(c, r)->GetIndex() };

			for (Elite::GraphConnection* con : m_pGridGraph->GetConnections(m_pGridGraph->GetNode(c, r)->GetIndex()))
			{
				if (LowestInfluance > m_pGridGraph->GetNode(con->GetTo())->GetInfluence() && !(m_pGridGraph->GetNode(con->GetTo())->GetInfluence() < 0.f))
				{
					LowestInfluance = m_pGridGraph->GetNode(con->GetTo())->GetInfluence();
					LowestInfluanceIndex = con->GetTo();
				}
			}

			if (m_pGridGraph->IsNodeValid(LowestInfluanceIndex))
			{
				*m_Directions.at(originalIndex) = (m_pGridGraph->GetNodeWorldPos(LowestInfluanceIndex) - m_pGridGraph->GetNodeWorldPos(originalIndex)).GetNormalized();
			}
			if (m_pGridGraph->GetNode(c, r)->GetInfluence() < 0.1f)
			{
				*m_Directions.at(originalIndex) = Vector2{ 0,0 };
			}
		}
	}
}

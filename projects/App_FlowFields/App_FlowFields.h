#ifndef INFLUENCE_MAP_APPLICATION_H
#define INFLUENCE_MAP_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"

#include "framework\EliteAI\EliteGraphs\EGraphNodeTypes.h"
#include "framework\EliteAI\EliteGraphs\EGraphConnectionTypes.h"
#include "framework\EliteAI\EliteGraphs\EGraph2D.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphRenderer.h"

#include "projects/Shared/BaseAgent.h"
#include "projects/Shared/NavigationColliderElement.h"
#include "framework/EliteAI/EliteGraphs/EInfluenceMap.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EBFS.h"

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_FlowFields final : public IApp
{
public:
	//Constructor & Destructor
	App_FlowFields() = default;
	virtual ~App_FlowFields() final;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//agents
	const int m_AgentAmount{ 50 };
	std::vector<BaseAgent*> m_pAgents;

	//world
	float m_TrimWorldSize{ };
	const int m_CellSize{10};
	const int m_ColsRows{25};
	Elite::Vector2 CurrentTarget{};

	std::vector<NavigationColliderElement*> m_Colliders;
	//graph
	Elite::GridGraph< Elite::InfluenceNode, Elite::GraphConnection>* m_pGridGraph;
	Elite::BFS< Elite::InfluenceNode, Elite::GraphConnection>* m_pBFS;//used to generate heatmap
	Elite::EGraphRenderer m_GraphRenderer;

	std::vector<Elite::Vector2*> m_Directions;
	//rendering
	bool m_DrawVectorField;
	bool m_DrawGrid;
	//C++ make the class non-copyable
	App_FlowFields(const App_FlowFields&) = delete;
	App_FlowFields& operator=(const App_FlowFields&) = delete;

	void AddWallOnMouseClick(Elite::InputMouseButton mouseBtn);
	void SetTargetOnMouseClick(Elite::InputMouseButton mouseBtn);
	void SetNodeColorsBasedOnHeatMap();
	void SetDirections();
};
#endif
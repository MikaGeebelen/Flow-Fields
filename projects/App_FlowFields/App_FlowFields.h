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
#include "framework/EliteAI/EliteGraphs/EInfluenceMap.h"
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
	const int m_AgentAmount{ 40 };
	std::vector<BaseAgent*> m_pAgents;

	//world
	const float m_TrimWorldSize{ 140.f };

	//graph
	Elite::InfluenceMap<Elite::GridGraph< Elite::InfluenceNode, Elite::GraphConnection>>* m_pGridGraph;
	Elite::EGraphRenderer m_GraphRenderer;

	//C++ make the class non-copyable
	App_FlowFields(const App_FlowFields&) = delete;
	App_FlowFields& operator=(const App_FlowFields&) = delete;

	void AddInfluenceOnMouseClick(Elite::InputMouseButton mouseBtn, float inf);
};
#endif
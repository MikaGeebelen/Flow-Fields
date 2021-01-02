#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon; 
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}
	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	for (Line* line :m_pNavMeshPolygon->GetLines())
	{
		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(line->index).size()>1)
		{
			Vector2 lineMiddle = (line->p1 + line->p2) / 2;
			AddNode((new NavGraphNode(GetNextFreeNodeIndex(), line->index, lineMiddle)));
		}
	}
	//2. Create connections now that every node is created
	for (Triangle* triangle :m_pNavMeshPolygon->GetTriangles())
	{
		std::vector<NavGraphNode*> tempSave;
		for (int i :triangle->metaData.IndexLines)
		{
			for (NavGraphNode* node: m_Nodes)
			{
				if (i == node->GetLineIndex())
				{
					tempSave.push_back(node);
				}
			}
		}
		if (tempSave.size() == 2)
		{
			AddConnection(new GraphConnection2D(tempSave[0]->GetIndex(), tempSave[1]->GetIndex()));
		}
		if (tempSave.size() == 3)
		{
			for (size_t i = 0; i < tempSave.size(); i++)
			{
				if (i != 2)
				{
					AddConnection(new GraphConnection2D(tempSave[i]->GetIndex(), tempSave[i+1]->GetIndex()));
				}
				else
				{
					AddConnection(new GraphConnection2D(tempSave[i]->GetIndex(), tempSave[0]->GetIndex()));
				}
			}
		}
	}
	SetConnectionCostsToDistance();
}


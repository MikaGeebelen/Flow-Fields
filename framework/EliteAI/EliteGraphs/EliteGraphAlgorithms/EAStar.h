#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		vector<T_NodeType*> path;
		vector<NodeRecord> openList;
		vector<NodeRecord> closedList;
		NodeRecord currentRecord;

		NodeRecord startRecord = NodeRecord{ pStartNode,nullptr,0,GetHeuristicCost(pStartNode,pGoalNode) };
	
		openList.push_back(startRecord);
		while (!openList.empty())
		{
			float gCost{};
			float tempMax{FLT_MAX};
			bool check{true};

			for (NodeRecord node: openList)
			{
				if (node.estimatedTotalCost <= tempMax)
				{
					currentRecord = node;
					tempMax = currentRecord.estimatedTotalCost;
				}
			}

			if (currentRecord.pNode == pGoalNode)
			{
				break;
			}

			gCost = currentRecord.costSoFar;
			for (auto con : m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex()))
			{
				check = true;
				T_NodeType* nextNode = m_pGraph->GetNode(con->GetTo());
				NodeRecord tempNode = NodeRecord{ nextNode, con, con->GetCost() + gCost, con->GetCost() + gCost + GetHeuristicCost(nextNode, pGoalNode) };

				for (size_t i{}; i < closedList.size(); i++)
				{
					if (closedList[i].pNode == tempNode.pNode)
					{
						if (closedList[i].costSoFar <= tempNode.costSoFar)
						{
							check = false;
						}
						else
						{
							std::vector<NodeRecord>::iterator it = closedList.begin();
							closedList.erase(it + i);
						}
					}
				}
				for (size_t i{}; i < openList.size(); i++)
				{
					if (openList[i].pNode == tempNode.pNode)
					{
						if (openList[i].costSoFar <= tempNode.costSoFar)
						{
							check = false;
						}
						else
						{
							std::vector<NodeRecord>::iterator it = openList.begin();
							openList.erase(it + i);
						}
					}
				}

				if (check)
				{
					openList.push_back(tempNode);
				}
			}

			for (size_t i{}; i < openList.size(); i++)
			{
				if (openList[i] == currentRecord)
				{
					std::vector<NodeRecord>::iterator it = openList.begin();
					openList.erase(it + i);
					closedList.push_back(currentRecord);
				}
			}

		}

		path.push_back(currentRecord.pNode);
		while (currentRecord.pNode != pStartNode)
		{
			for (NodeRecord node : closedList)
			{
				if (currentRecord.pConnection != nullptr)
				{
					if (m_pGraph->GetNode(currentRecord.pConnection->GetFrom()) == node.pNode)
					{
						path.push_back(node.pNode);
						currentRecord = node;
					}
				}
			}
		}

		std::reverse(path.begin(), path.end());
		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}
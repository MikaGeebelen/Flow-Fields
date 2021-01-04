#pragma once

namespace Elite 
{
	template <class T_NodeType, class T_ConnectionType>
	class BFS
	{
	public:
		BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);
		void CreateHeatMap(InfluenceNode* pStartNode);
	private:
		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template <class T_NodeType, class T_ConnectionType>
	BFS<T_NodeType, T_ConnectionType>::BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> BFS<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode)
	{
		std::queue<T_NodeType*> openList;
		std::map<T_NodeType*, T_NodeType*> closedList;

		openList.push(pStartNode);

		while (!openList.empty())
		{
			T_NodeType* currentNode = openList.front();
			openList.pop();

			if (currentNode == pDestinationNode)
			{
				break;
			}

			for (auto con: m_pGraph->GetNodeConnections(currentNode->GetIndex()))
			{
				T_NodeType* nextNode = m_pGraph->GetNode(con->GetTo());
				if (closedList.find(nextNode)==closedList.end())
				{
					openList.push(nextNode);
					closedList[nextNode] = currentNode;
				}
			}
		}

		vector<T_NodeType*> path;
		T_NodeType* currentNode = pDestinationNode;
		while (currentNode != pStartNode)
		{
			path.push_back(currentNode);
			currentNode = closedList[currentNode];
		}
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());
		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	void BFS<T_NodeType, T_ConnectionType>::CreateHeatMap(InfluenceNode* pDestinationNode)
	{
		//HeatMap only works on influanceMaps
		//using the influance parameter to set the distance of each square to the target node
		std::list<InfluenceNode*> openList;//nodes to search 
		std::list<InfluenceNode*> closedList;//searched nodes

		openList.push_back(pDestinationNode);

		pDestinationNode->SetInfluence(0);

		
		while (!openList.empty())//keep running till all possible nodes are done
		{
			InfluenceNode* currentNode = openList.front();
			openList.pop_front();
			
			for (auto con : m_pGraph->GetNodeConnections(currentNode->GetIndex()))
			{
				InfluenceNode* nextNode = m_pGraph->GetNode(con->GetTo());
				if (std::find(closedList.begin(), closedList.end(), nextNode) == closedList.end() && std::find(openList.begin(), openList.end(), nextNode) == openList.end())//check if node is searched
				{
					if (!(nextNode->GetInfluence() < 0.f))
					{
						openList.push_back(nextNode);
						nextNode->SetInfluence(currentNode->GetInfluence() + 1);
					}

					if (std::find(closedList.begin(), closedList.end(), currentNode) == closedList.end())//check if node is searched
					{
						closedList.push_back(currentNode);
					}
				}
			}
		}
	}
}


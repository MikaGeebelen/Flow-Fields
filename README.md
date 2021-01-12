# Flow fields

## Implementation
The way I implemented the flow fields was by following three steps.
1. generate a heatmap
1. use the heatmap to generate a vectorfield
1. pathfinder uses vectorfield to move

### generate heatmap
To generate a heatmap you use a pathfinding algorithm to go over all the nodes in the world that the pathfinder can traverse.
You start at the destination and give that node the value of 0 for each consequent node you traverse you give it the value of the node previous node +1.
When you traversed all the nodes and you give each node a colour based on their value.


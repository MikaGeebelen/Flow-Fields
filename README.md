# Flow fields

## Implementation
The way I implemented the flow fields was by following three steps.
1. generate a heatmap
1. use the heatmap to generate a vectorfield
1. pathfinder uses vectorfield to move

### generate heatmap
To generate a heatmap you use a pathfinding algorithm to go over all the nodes in the world that the pathfinder can traverse.
You start at the destination and give that node the value of 0.
From that node you let the pathfinding algorythm traverse all other nodes till there are no nodes left. 
Each traversed node gets the value of the previos node +1.

You can visualise a heatmap by coloring the nodes based on the value you assigned them and it should result in something like this:


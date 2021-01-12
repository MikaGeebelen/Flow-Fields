# Flow fields

## Implementation
The way I implemented the flow fields was by following three steps.
1. generate a heatmap
1. use the heatmap to generate a vectorfield
1. pathfinder uses vectorfield to move

### generate heatmap
To generate a heatmap you use a pathfinding algorithm to go over all the nodes in the world that the pathfinder can traverse.

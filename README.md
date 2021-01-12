# Flow fields

## Implementation
The way I implemented the flow fields was by following three steps.
1. generate a heatmap
1. use the heatmap to generate a vector field
1. pathfinder uses vectorfield to move

### Generate heatmap
To generate a heatmap you use a pathfinding algorithm to go over all the nodes in the world that the pathfinder can traverse.
You start at the destination and give that node the value of 0.
From that node you let the pathfinding algorythm traverse all other nodes till there are no nodes left. 
Each traversed node gets the value of the previos node +1.

The algorythm I used to traverse the nodes was a breath first search.
Since we don't have to prioritise any nodes we can just start from the center and go outwards.

You can visualise a heatmap by coloring the nodes based on the value you assigned them. 
It should result in something like this:
 - yellow: pathfinders
 - The closer to the target less blue.

![](Images/Heatmap.PNG)

### Generate vector field 
Now that we have a heatmap we can use it to generate a vector field.
We want to know the direction for all nodes that the pathfinder can find themselves in to reach the target.
You can do this by creating a vector for each node that points to the neighboring node with the lowest value.

You can visualise this by drawing the generated vector in each node.
It should result in something like this:
 - yellow: pathfinders
 - red: the vector for each node
 - They all point in the general direction of the target.
 
 ![](Images/VectorField.PNG)

### Movement
Now that we have a vector field our pathfinders just need to use it.
Every frame you check for each pathfinder in what node the reside and give them a direction based on the vector field. 

By drawing all the pathfinders their direction you can see if they properly use the vector field.
It should result in something like this:
 - yellow: pathfinders
 - green: pathfinder direction
 - The use the same direction as the node they walk on.
 
![](Images/PathfindersUseVectorField.PNG)


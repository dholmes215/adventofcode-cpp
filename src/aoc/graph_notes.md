
# Graph Concepts

### Implicit Graph
[Wikipedia](https://en.wikipedia.org/wiki/Implicit_graph): an implicit graph representation (or more simply implicit graph) is a graph whose vertices or edges are not represented as explicit objects in a computer's memory, but rather are determined algorithmically from some other input, for example a computable function.

_The Boost Graph Library_ describes an implicit graph for the knight's jumps in the Knight's Tour problem.  That implicit graph type knows the total number of vertices, even if they aren't explicitly enumerated, which is suitable for its specific definition of the backtracking algorithm.

### Backtracking Algorithm

_The Boost Graph Library_ describes a "Backtracking Graph Search" where "the search is completed once all of the vertices have been visited, or when all possible paths have been exhausted".  It knows the number vertices in advance, remembers which vertices it's visited on its current path, and does not define any successful result other than finding a path that touches all vertices exactly once.

[Wikipedia](https://en.wikipedia.org/wiki/Backtracking) defines a generic backtracking algorithm which does not itself consider whether vertices have already been visited, but could reject such vertices if its generic `reject(P,c)` parameter does so.  It searches _all_ non-rejected candidate solutions but can optionally be made to stop after one or other conditions.

### Warnsdorff's Rule/Heuristic
TODO
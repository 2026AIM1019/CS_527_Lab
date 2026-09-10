Sure. Let's understand **Minimum Spanning Tree (MST), Kruskal's algorithm, and Prim's algorithm from the very basics**, in the same easy, exam/viva-oriented way.

The most important thing is to understand **what problem both algorithms are solving** before learning their steps.

---

# 1. What is a Minimum Spanning Tree?

Suppose we have some cities:

```text
A ---- B
|    / |
|   /  |
C ---- D
```

Suppose the edges represent roads, and each road has a construction cost.

For example:

```text
A --5-- B
A --2-- C
B --3-- C
B --4-- D
C --6-- D
```

We want to connect **all cities together**.

But we don't necessarily want to build every road.

We want:

> Connect every vertex using the minimum possible total cost.

That is exactly the **Minimum Spanning Tree problem**.

---

# 2. Why is it called a "Spanning Tree"?

Let's break the name into two words.

## Spanning

"Spanning" means:

> It covers/reaches **all vertices** of the graph.

If the graph has:

```text
A B C D E
```

the spanning tree must contain:

```text
A, B, C, D, E
```

All vertices must be connected.

---

## Tree

A tree is a graph that:

* is connected
* has no cycle

For `V` vertices, a tree always has:

```text
V - 1 edges
```

So if we have:

```text
V = 5
```

a spanning tree must have:

```text
5 - 1 = 4 edges
```

---

# 3. What makes it "Minimum"?

There can be many different spanning trees for the same graph.

For example:

```text
        5
   A -------- B
   |          |
  2|          |3
   |          |
   C -------- D
        4
```

There can be multiple ways to connect all four vertices.

The **Minimum Spanning Tree** is the one whose:

```text
sum of edge weights
```

is the smallest.

---

# 4. Example of MST

Consider:

```text
A --1-- B
|      /|
4    2  |5
|  /    |
C --3---D
```

Let's say the edges are:

```text
A-B = 1
A-C = 4
B-C = 2
B-D = 5
C-D = 3
```

We need:

```text
V = 4
```

Therefore MST must contain:

```text
V - 1 = 3 edges
```

The cheapest edges are:

```text
A-B = 1
B-C = 2
C-D = 3
```

Take them:

```text
A --1-- B --2-- C --3-- D
```

Total cost:

```text
1 + 2 + 3 = 6
```

This connects all vertices and has no cycle.

Therefore:

```text
MST weight = 6
```

---

# 5. The three important properties of MST

For an MST:

### Property 1 — All vertices must be connected

```text
A ---- B ---- C ---- D
```

Good.

But:

```text
A ---- B       C ---- D
```

is not an MST because the graph is disconnected.

---

### Property 2 — No cycle

For example:

```text
A ---- B
|      |
|      |
C ---- D
```

contains a cycle:

```text
A → B → D → C → A
```

This cannot be the final spanning tree.

---

### Property 3 — Exactly V-1 edges

If:

```text
V = 6
```

then MST has:

```text
5 edges
```

Always.

---

# 6. Kruskal vs Prim

Both algorithms solve:

> Find a Minimum Spanning Tree.

But they approach the problem differently.

### Kruskal

Thinks:

> "Let me look at the cheapest edges globally."

It focuses on **edges**.

```text
Sort edges
     ↓
Cheapest edge
     ↓
Next cheapest edge
     ↓
...
```

---

### Prim

Thinks:

> "Let me start with one vertex and grow my tree."

It focuses on **growing one connected tree**.

```text
Start with vertex 0
        ↓
Find cheapest edge leaving tree
        ↓
Add new vertex
        ↓
Find cheapest edge leaving tree
        ↓
...
```

This is the main difference you should remember.

---

# 7. Kruskal's Algorithm

Kruskal is a **greedy algorithm**.

Greedy means:

> At every step, make the best-looking choice available right now.

For Kruskal:

> The best choice is the currently cheapest edge that does not create a cycle.

---

# 8. Kruskal's basic idea

Suppose edges are:

```text
A-B = 1
B-C = 2
C-D = 3
A-C = 4
B-D = 5
```

First sort:

```text
1 → 2 → 3 → 4 → 5
```

Then process them.

```text
A-B (1) → take
B-C (2) → take
C-D (3) → take
```

We already have:

```text
V - 1 = 3
```

edges, so stop.

---

# 9. Why can't Kruskal simply take every cheapest edge?

Because of cycles.

Consider:

```text
A-B = 1
B-C = 2
A-C = 3
```

Kruskal:

### First:

```text
A-B = 1
```

Take it.

```text
A ---- B
```

### Second:

```text
B-C = 2
```

Take it.

```text
A ---- B ---- C
```

### Third:

```text
A-C = 3
```

If we take it:

```text
A ---- B
 \     /
  \   /
    C
```

we create a cycle.

Therefore:

```text
A-C
```

must be rejected.

---

# 10. How does Kruskal detect cycles?

This is where **DSU / Union-Find** comes in.

DSU means:

> Disjoint Set Union.

It maintains groups/components of vertices.

Initially:

```text
A    B    C    D
```

Every vertex is its own separate component.

---

# 11. DSU visualization

Initially:

```text
{A} {B} {C} {D}
```

Take:

```text
A-B
```

Now:

```text
{A,B} {C} {D}
```

Take:

```text
B-C
```

Now:

```text
{A,B,C} {D}
```

Take:

```text
C-D
```

Now:

```text
{A,B,C,D}
```

All vertices are connected.

---

# 12. The key Kruskal question

For every edge `(u,v)`, ask:

> Are `u` and `v` already in the same component?

If **NO**:

```text
Take edge
```

If **YES**:

```text
Reject edge
```

Why?

Because if they're already connected, adding another edge between them creates a cycle.

---

# 13. DSU has two important operations

You will usually see:

```text
find()
union()
```

## `find(x)`

Answers:

> Which component does vertex `x` belong to?

For example:

```text
{A,B,C}
{D}
```

Then:

```text
find(A) → component 1
find(B) → component 1
find(C) → component 1
find(D) → component 2
```

---

## `union(a,b)`

Combines two different components.

For example:

```text
{A,B} {C,D}
```

After:

```text
union(B,C)
```

we get:

```text
{A,B,C,D}
```

---

# 14. Kruskal algorithm step-by-step

Suppose:

```text
A-B = 1
A-C = 4
B-C = 2
B-D = 5
C-D = 3
```

### Step 1: Sort

```text
A-B = 1
B-C = 2
C-D = 3
A-C = 4
B-D = 5
```

---

### Step 2: Initially

```text
{A} {B} {C} {D}
```

MST:

```text
empty
```

---

### Step 3: A-B = 1

Check:

```text
find(A) != find(B)
```

Yes.

Take it.

```text
{A,B} {C} {D}
```

MST:

```text
A-B
```

---

### Step 4: B-C = 2

Check:

```text
find(B) != find(C)
```

Yes.

Take it.

```text
{A,B,C} {D}
```

MST:

```text
A-B
B-C
```

---

### Step 5: C-D = 3

Check:

```text
find(C) != find(D)
```

Yes.

Take it.

```text
{A,B,C,D}
```

MST:

```text
A-B
B-C
C-D
```

We now have:

```text
V - 1 = 4 - 1 = 3 edges
```

Stop.

Total:

```text
1 + 2 + 3 = 6
```

---

# 15. Kruskal's algorithm pseudocode

```text
Kruskal(G):

    sort all edges by increasing weight

    create DSU

    MST = empty

    for each edge (u, v, weight):

        if find(u) != find(v):

            add edge to MST

            union(u, v)

        if MST has V-1 edges:
            break

    return MST
```

That's the entire algorithm.

---

# 16. Why Kruskal works

The important greedy idea is:

> Always take the cheapest edge that connects two currently separate components.

We never take an edge that creates a cycle.

Eventually, all components merge into one connected component.

And because we're always choosing the cheapest safe edge, the resulting tree has minimum total weight.

---

# 17. Prim's Algorithm

Now let's understand Prim.

Prim is also greedy.

But its thinking is different.

Kruskal:

```text
Look at cheapest edge anywhere.
```

Prim:

```text
Look at cheapest edge that connects
my current tree to a new vertex.
```

---

# 18. Prim starts from a vertex

Your assignment recommends:

```text
Start from vertex 0
```

Suppose:

```text
A = vertex 0
```

Initially:

```text
Tree = {A}
```

---

# 19. Find cheapest edge leaving the tree

Suppose:

```text
A-B = 1
A-C = 4
```

The cheapest edge from A is:

```text
A-B = 1
```

Take it.

Now:

```text
Tree = {A,B}
```

---

# 20. Now expand again

From the current tree:

```text
A ---- B
```

Look at all edges that go from:

```text
Tree → Outside
```

Suppose:

```text
A-C = 4
B-C = 2
B-D = 5
```

The cheapest is:

```text
B-C = 2
```

Take it.

Now:

```text
Tree = {A,B,C}
```

---

# 21. Continue

Remaining outside vertex:

```text
D
```

Edges connecting the tree to D:

```text
C-D = 3
B-D = 5
```

Choose:

```text
C-D = 3
```

Now:

```text
Tree = {A,B,C,D}
```

Done.

MST:

```text
A-B = 1
B-C = 2
C-D = 3
```

Total:

```text
6
```

---

# 22. Prim's algorithm with a priority queue

The assignment says:

> A min-priority queue is recommended for an efficient implementation.

Why?

Because every time we need:

> "Give me the cheapest edge available."

A **min-priority queue** is perfect for this.

It keeps the smallest-weight edge at the top.

---

# 23. What does Prim store?

Typically we maintain:

```text
visited[]
```

This tells us whether a vertex is already part of the MST.

For example:

```text
visited[A] = true
visited[B] = true
visited[C] = false
visited[D] = false
```

means:

```text
A, B → already in tree
C, D → outside tree
```

---

# 24. Prim algorithm step-by-step

Suppose:

```text
A-B = 1
A-C = 4
B-C = 2
B-D = 5
C-D = 3
```

Start:

```text
visited = {}
```

Start from A:

```text
visited = {A}
```

Put A's edges into min-heap:

```text
(A-B, 1)
(A-C, 4)
```

Minimum:

```text
(A-B, 1)
```

Take it.

Now:

```text
visited = {A,B}
```

Add B's edges:

```text
(B-C, 2)
(B-D, 5)
```

Heap contains relevant edges such as:

```text
(B-C, 2)
(A-C, 4)
(B-D, 5)
```

Take:

```text
B-C = 2
```

Now:

```text
visited = {A,B,C}
```

Add C's edge:

```text
C-D = 3
```

Minimum edge leading outside:

```text
C-D = 3
```

Take it.

Now:

```text
visited = {A,B,C,D}
```

Done.

---

# 25. Prim's algorithm pseudocode

```text
Prim(G):

    choose starting vertex 0

    visited[0] = true

    put all edges from vertex 0 into min-heap

    MST = empty

    while MST has fewer than V-1 edges:

        take minimum-weight edge from heap

        let edge be (u, v)

        if v is already visited:
            continue

        add edge to MST

        visited[v] = true

        add all edges from v to unvisited vertices into heap

    return MST
```

The exact implementation may push edges in a slightly different way, but this is the core idea.

---

# 26. Kruskal vs Prim — the most important difference

This is probably the **single most important thing for your viva**.

| Kruskal                                  | Prim                                    |
| ---------------------------------------- | --------------------------------------- |
| Works by considering edges globally      | Grows one tree                          |
| Sorts all edges                          | Uses min-priority queue                 |
| Uses DSU/Union-Find                      | Uses visited array                      |
| Starts with separate components          | Starts with one vertex                  |
| Can temporarily have multiple components | Maintains one growing connected tree    |
| Adds cheapest safe edge                  | Adds cheapest edge leaving current tree |

Remember:

```text
Kruskal → cheapest edge globally
Prim    → cheapest edge from current tree
```

---

# 27. Visual difference

### Kruskal

Think:

```text
All edges
   |
   ↓
Sort by weight
   |
   ↓
Cheapest
   |
   ↓
Safe?
 /   \
Yes   No
 |     |
Take  Reject
```

It doesn't initially care about one particular starting vertex.

---

### Prim

Think:

```text
Start at vertex 0
       |
       ↓
Current tree
       |
       ↓
Edges leaving tree
       |
       ↓
Cheapest one
       |
       ↓
Add new vertex
       |
       ↓
Repeat
```

---

# 28. Can Kruskal and Prim produce different MSTs?

**Yes.**

Suppose the graph has multiple edges with the same weights.

There can be multiple valid MSTs.

For example:

```text
A --1-- B
|       |
1       1
|       |
C --1-- D
```

Several different spanning trees can have:

```text
total weight = 3
```

Kruskal might choose one set of edges.

Prim might choose another.

That's completely okay.

The important requirement is:

```text
MST weight from Kruskal
=
MST weight from Prim
```

If both produce:

```text
Total weight = 3
```

then both have successfully found an MST, even if their edge sets differ.

---

# 29. Why exactly V-1 edges?

This is worth understanding because your assignment explicitly mentions it.

Suppose we have:

```text
V = 4
```

Start with four separate vertices:

```text
A    B    C    D
```

Every time we safely add an edge connecting two separate components, the number of components decreases by 1.

```text
4 components
    ↓ add edge
3 components
    ↓ add edge
2 components
    ↓ add edge
1 component
```

We needed:

```text
4 - 1 = 3
```

edges.

Therefore:

```text
MST always has V-1 edges.
```

---

# 30. Why can't MST have V edges?

Because if a connected graph with `V` vertices has `V` edges, it must contain a cycle.

For example:

```text
A ---- B
|      |
|      |
D ---- C
```

4 vertices and 4 edges.

There is a cycle.

A tree cannot have that.

Therefore MST has exactly:

```text
V - 1
```

edges.

---

# 31. What happens if we add a very cheap edge that creates a cycle?

This is an important greedy concept.

Suppose:

```text
A-B = 1
B-C = 2
A-C = 3
```

After:

```text
A-B
B-C
```

we already have:

```text
A ---- B ---- C
```

Now `A-C = 3` is cheap, but adding it produces:

```text
A ---- B
 \      |
  \     |
   -----C
```

cycle.

Therefore it is rejected.

This is why **cheapest edge doesn't automatically mean take it**.

The actual Kruskal rule is:

> Take the cheapest edge that does not create a cycle.

---

# 32. Real-life example: Internet cables

Suppose IIT campuses/buildings need to be connected:

```text
Building A
Building B
Building C
Building D
Building E
```

Possible cable connections have different costs.

You want:

```text
Every building connected
+
No unnecessary cycles
+
Minimum total cable cost
```

That's an MST problem.

Kruskal:

```text
Find cheapest cable
→ connect two separate groups
→ repeat
```

Prim:

```text
Start at Building A
→ find cheapest cable to a new building
→ expand network
→ repeat
```

---

# 33. Other real-life applications

### 1. Electricity networks

Connect all cities/houses while minimizing wire cost.

### 2. Road construction

Connect all required cities using minimum construction cost.

### 3. Water pipelines

Connect all locations while minimizing total pipeline length/cost.

### 4. Computer networks

Design a low-cost network backbone.

### 5. Clustering

MST can be used as a basis for grouping similar data points.

### 6. Image segmentation

Pixels can be represented as graph vertices, with edge weights representing similarity/difference. MST-based methods can help separate regions.

---

# 34. Time complexity

This is also important for your assignment.

## Kruskal

The main expensive operation is sorting edges.

If there are:

```text
V vertices
E edges
```

then sorting takes approximately:

```text
O(E log E)
```

DSU operations are very efficient.

So overall:

```text
Kruskal ≈ O(E log E)
```

---

# 35. Prim

With an adjacency list and binary min-priority queue:

```text
O(E log V)
```

approximately.

So:

```text
Kruskal → O(E log E)
Prim    → O(E log V)
```

In many practical situations these are quite similar because `E` is related to `V`.

---

# 36. One very important point: graph must be connected

Your MST definition says:

> The graph must be connected.

Why?

Suppose:

```text
A ---- B

C ---- D
```

There is no way to create a single spanning tree containing all four vertices because the two components aren't connected.

So there is **no MST for the entire graph**.

You could calculate a minimum spanning forest, but that is a different problem.

---

# 37. The complete example: both algorithms

Let's use:

```text
A-B = 1
A-C = 4
B-C = 2
B-D = 5
C-D = 3
```

### Kruskal

Sort:

```text
1: A-B
2: B-C
3: C-D
4: A-C
5: B-D
```

Take:

```text
A-B
B-C
C-D
```

Result:

```text
MST weight = 1 + 2 + 3 = 6
```

---

### Prim

Start:

```text
A
```

Cheapest outgoing:

```text
A-B = 1
```

Tree:

```text
A-B
```

Cheapest edge to outside:

```text
B-C = 2
```

Tree:

```text
A-B-C
```

Cheapest edge to outside:

```text
C-D = 3
```

Tree:

```text
A-B-C-D
```

Result:

```text
MST weight = 1 + 2 + 3 = 6
```

Therefore:

```text
Kruskal MST weight = 6
Prim MST weight     = 6
```

They agree.

---

# 38. A simple mental model

If you forget everything else, remember this:

### Kruskal

Imagine all roads lying on a table.

```text
ALL ROADS
   ↓
Arrange cheapest → expensive
   ↓
Pick cheapest
   ↓
Would it create a cycle?
   ↓
No → take it
Yes → reject it
```

---

### Prim

Imagine you're building a network starting from one city.

```text
START CITY
    ↓
CURRENT NETWORK
    ↓
Look at roads leaving network
    ↓
Pick cheapest road to a new city
    ↓
Add city
    ↓
Repeat
```

---

# 39. One-line definitions for exam

### MST

> A Minimum Spanning Tree is a spanning tree of a connected, weighted, undirected graph that connects all vertices using exactly V−1 edges with minimum possible total edge weight and no cycles.

### Kruskal

> Kruskal's algorithm sorts all edges in increasing order of weight and repeatedly adds the cheapest edge whose endpoints belong to different components, using DSU to avoid cycles.

### Prim

> Prim's algorithm starts from a chosen vertex and repeatedly adds the minimum-weight edge connecting a vertex inside the current tree to a vertex outside it, typically using a min-priority queue.

---

# 40. Final comparison you should memorize

```text
                    MST
                     |
          ┌──────────┴──────────┐
          ↓                     ↓
      KRUSKAL                  PRIM
          |                     |
   Sort all edges          Start vertex 0
          |                     |
     Cheapest edge         Grow one tree
          |                     |
      DSU check             Min-heap
          |                     |
   Cycle? Reject            Cheapest edge
          |                     |
   Different component      New vertex?
          |                     |
       Add edge                Add edge
          |                     |
       V-1 edges              V-1 edges
          |                     |
          └──────────┬──────────┘
                     ↓
                Minimum
              total weight
```

**The key distinction:**
**Kruskal builds the MST by joining components using the cheapest edges globally. Prim builds the MST by growing one connected tree using the cheapest edge at its boundary.** Both are greedy algorithms, and when implemented correctly, they produce the same minimum total MST weight.


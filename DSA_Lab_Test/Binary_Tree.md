Absolutely. **Binary Trees** are one of the most important topics for DSA/coding rounds. I’ll teach this from the ground up, including **creation, insertion, deletion, update, searching, all traversals, height, leaf nodes, level order, recursion, and the most expected coding-round questions with solutions**.

# 🌳 Binary Trees — Complete C++ DSA Tutorial

---

# 1. What is a Binary Tree?

A **Binary Tree** is a tree data structure where every node can have **at most two children**:

* Left child
* Right child

For example:

```text
             10
            /  \
           20   30
          / \     \
         40  50    60
```

Here:

* `10` → root
* `20`, `30` → children of 10
* `40`, `50` → children of 20
* `60` → right child of 30

A node can have:

```text
0 children
1 child
2 children
```

but **never more than 2 children**.

---

# 2. Important Terminology

Consider:

```text
             10
            /  \
           20   30
          / \
         40  50
```

### Root

The topmost node.

```text
10
```

### Parent

A node that has children.

For example:

```text
20
```

is the parent of `40` and `50`.

### Child

`40` and `50` are children of `20`.

### Leaf Node

A node with **no children**.

Here:

```text
40, 50, 30
```

are leaf nodes.

### Edge

Connection between two nodes.

```text
10 --- 20
```

is one edge.

### Subtree

The tree rooted at a particular node.

For example, subtree of `20`:

```text
       20
      /  \
     40   50
```

---

# 3. How Do We Represent a Binary Tree in C++?

This is extremely important.

We create a `Node`.

```cpp
struct Node {
    int data;
    Node* left;
    Node* right;
};
```

Each node contains:

```text
data
left pointer
right pointer
```

For example:

```text
       10
      /  \
    20    30
```

Internally:

```text
Node 10
 data = 10
 left = address of Node 20
 right = address of Node 30
```

---

# 4. Creating a Node

We can write:

```cpp
Node* newNode(int value) {
    Node* node = new Node();

    node->data = value;
    node->left = nullptr;
    node->right = nullptr;

    return node;
}
```

Or more conveniently:

```cpp
struct Node {
    int data;
    Node* left;
    Node* right;

    Node(int value) {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};
```

Now:

```cpp
Node* root = new Node(10);
```

creates:

```text
       10
      /  \
   NULL  NULL
```

---

# 5. Creating a Complete Binary Tree Manually

Suppose we want:

```text
             10
            /  \
           20   30
          / \   /
         40 50 60
```

Code:

```cpp
#include <bits/stdc++.h>
using namespace std;

struct Node {
    int data;
    Node* left;
    Node* right;

    Node(int value) {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};

int main() {

    Node* root = new Node(10);

    root->left = new Node(20);
    root->right = new Node(30);

    root->left->left = new Node(40);
    root->left->right = new Node(50);

    root->right->left = new Node(60);

    return 0;
}
```

The important part is:

```cpp
root->left = new Node(20);
```

means:

> Create node 20 and make it the left child of root.

Similarly:

```cpp
root->right = new Node(30);
```

means:

> Create node 30 and make it the right child of root.

---

# 6. Why Do We Use `->`?

You will see this everywhere in trees.

Suppose:

```cpp
Node* root;
```

`root` is a pointer.

Therefore:

```cpp
root->data
```

means:

> Access the `data` inside the node pointed to by root.

Similarly:

```cpp
root->left
```

means:

> Access the left child pointer.

And:

```cpp
root->right
```

means:

> Access the right child pointer.

---

# 7. The Most Important Concept: Recursion

Binary trees are naturally recursive.

Why?

Because every subtree is itself a binary tree.

For example:

```text
             10
            /  \
           20   30
          / \
         40 50
```

The left subtree:

```text
       20
      /  \
     40   50
```

is itself a binary tree.

And the right subtree:

```text
30
```

is also a binary tree.

Therefore, most binary-tree problems are solved using:

```text
Solve left subtree
Solve right subtree
Combine the answers
```

This idea is **extremely important for coding rounds**.

---

# 8. Tree Traversals

The most important binary-tree topic is traversal.

There are four major traversals you should know:

1. Preorder
2. Inorder
3. Postorder
4. Level Order

Consider:

```text
             10
            /  \
           20   30
          / \   /
         40 50 60
```

---

# 9. Preorder Traversal

Rule:

```text
ROOT → LEFT → RIGHT
```

For our tree:

```text
             10
            /  \
           20   30
          / \   /
         40 50 60
```

Visit:

```text
10
20
40
50
30
60
```

So:

```text
10 20 40 50 30 60
```

### Code

```cpp
void preorder(Node* root) {

    if (root == nullptr)
        return;

    cout << root->data << " ";

    preorder(root->left);

    preorder(root->right);
}
```

Remember:

```text
PRE = Root first
```

---

# 10. Inorder Traversal

Rule:

```text
LEFT → ROOT → RIGHT
```

For our tree:

```text
             10
            /  \
           20   30
          / \   /
         40 50 60
```

Result:

```text
40 20 50 10 60 30
```

### Code

```cpp
void inorder(Node* root) {

    if (root == nullptr)
        return;

    inorder(root->left);

    cout << root->data << " ";

    inorder(root->right);
}
```

Remember:

```text
IN = Root in the middle
```

---

# 11. Postorder Traversal

Rule:

```text
LEFT → RIGHT → ROOT
```

Result:

```text
40 50 20 60 30 10
```

### Code

```cpp
void postorder(Node* root) {

    if (root == nullptr)
        return;

    postorder(root->left);

    postorder(root->right);

    cout << root->data << " ";
}
```

Remember:

```text
POST = Root last
```

---

# 12. Easy Way to Remember Traversals

| Traversal   | Order               |
| ----------- | ------------------- |
| Preorder    | Root → Left → Right |
| Inorder     | Left → Root → Right |
| Postorder   | Left → Right → Root |
| Level Order | Level by level      |

Shortcut:

```text
PRE   = ROOT first
IN    = ROOT middle
POST  = ROOT last
```

---

# 13. Level Order Traversal

This is basically **BFS for a tree**.

We use a queue.

Tree:

```text
             10
            /  \
           20   30
          / \   /
         40 50 60
```

Output:

```text
10
20 30
40 50 60
```

Or:

```text
10 20 30 40 50 60
```

### Code

```cpp
void levelOrder(Node* root) {

    if (root == nullptr)
        return;

    queue<Node*> q;

    q.push(root);

    while (!q.empty()) {

        Node* current = q.front();
        q.pop();

        cout << current->data << " ";

        if (current->left != nullptr)
            q.push(current->left);

        if (current->right != nullptr)
            q.push(current->right);
    }
}
```

This is one of the most important tree patterns:

```text
Tree + level by level
        ↓
      Queue
```

---

# 14. Search in a Binary Tree

Unlike a Binary Search Tree, a normal binary tree has **no ordering rule**.

So we may have to visit every node.

```cpp
bool search(Node* root, int key) {

    if (root == nullptr)
        return false;

    if (root->data == key)
        return true;

    return search(root->left, key) ||
           search(root->right, key);
}
```

Usage:

```cpp
if (search(root, 50))
    cout << "Found";
else
    cout << "Not Found";
```

### Complexity

Worst case:

```text
O(N)
```

because we may visit every node.

---

# 15. Count Number of Nodes

Suppose:

```text
       10
      /  \
     20   30
    / \
   40 50
```

There are 5 nodes.

### Logic

```text
Number of nodes =
1
+ nodes in left subtree
+ nodes in right subtree
```

### Code

```cpp
int countNodes(Node* root) {

    if (root == nullptr)
        return 0;

    return 1
        + countNodes(root->left)
        + countNodes(root->right);
}
```

---

# 16. Count Leaf Nodes

Leaf means:

```text
left == nullptr
right == nullptr
```

Code:

```cpp
int countLeaves(Node* root) {

    if (root == nullptr)
        return 0;

    if (root->left == nullptr &&
        root->right == nullptr)
        return 1;

    return countLeaves(root->left)
         + countLeaves(root->right);
}
```

---

# 17. Find Height of Binary Tree

This is **very frequently asked**.

Suppose:

```text
        10
       /
      20
     /
    30
   /
  40
```

Height is generally measured as the number of edges on the longest root-to-leaf path.

But some coding platforms define height as number of nodes.

You need to check the convention.

### Height in number of nodes

```cpp
int height(Node* root) {

    if (root == nullptr)
        return 0;

    int leftHeight = height(root->left);
    int rightHeight = height(root->right);

    return 1 + max(leftHeight, rightHeight);
}
```

For:

```text
10
```

height = 1.

For:

```text
10
 |
20
 |
30
```

height = 3 nodes.

---

# 18. Height in Number of Edges

If your course defines an empty tree as `-1`:

```cpp
int height(Node* root) {

    if (root == nullptr)
        return -1;

    return 1 + max(
        height(root->left),
        height(root->right)
    );
}
```

Then:

```text
    10
   /
  20
 /
30
```

has height:

```text
2
```

because there are two edges.

---

# 19. Find Maximum Value

```cpp
int findMax(Node* root) {

    if (root == nullptr)
        return INT_MIN;

    int leftMax = findMax(root->left);
    int rightMax = findMax(root->right);

    return max(root->data,
               max(leftMax, rightMax));
}
```

---

# 20. Find Minimum Value

```cpp
int findMin(Node* root) {

    if (root == nullptr)
        return INT_MAX;

    int leftMin = findMin(root->left);
    int rightMin = findMin(root->right);

    return min(root->data,
               min(leftMin, rightMin));
}
```

---

# 21. Sum of All Nodes

```cpp
int sumNodes(Node* root) {

    if (root == nullptr)
        return 0;

    return root->data
         + sumNodes(root->left)
         + sumNodes(root->right);
}
```

---

# 22. Update a Node

There is no special "update" operation in a binary tree.

If you already have the node/pointer:

```cpp
root->data = 100;
```

But usually we need to **find the node first**.

For example:

```cpp
bool update(Node* root, int oldValue, int newValue) {

    if (root == nullptr)
        return false;

    if (root->data == oldValue) {
        root->data = newValue;
        return true;
    }

    if (update(root->left, oldValue, newValue))
        return true;

    return update(root->right, oldValue, newValue);
}
```

Usage:

```cpp
update(root, 50, 500);
```

Changes:

```text
50 → 500
```

---

# 23. Inserting into a Binary Tree

Here we need to be careful.

A **normal binary tree does not have a fixed insertion rule**.

Unlike a BST:

```text
smaller → left
larger → right
```

doesn't exist in an ordinary binary tree.

One common approach is **level-order insertion**.

Example:

```text
             10
            /  \
           20   30
          / \
         40 50
```

Insert `60`.

We find the first available position:

```text
             10
            /  \
           20   30
          / \   /
         40 50 60
```

### Code

```cpp
void insert(Node*& root, int value) {

    Node* newNode = new Node(value);

    if (root == nullptr) {
        root = newNode;
        return;
    }

    queue<Node*> q;
    q.push(root);

    while (!q.empty()) {

        Node* current = q.front();
        q.pop();

        if (current->left == nullptr) {
            current->left = newNode;
            return;
        }
        else {
            q.push(current->left);
        }

        if (current->right == nullptr) {
            current->right = newNode;
            return;
        }
        else {
            q.push(current->right);
        }
    }
}
```

---

# 24. Why `Node*& root`?

This is important.

We write:

```cpp
void insert(Node*& root, int value)
```

instead of:

```cpp
void insert(Node* root, int value)
```

because the root itself may change.

For example, if the tree is empty:

```text
root = NULL
```

and we insert 10:

```text
root → 10
```

Therefore the function needs to modify the original `root`.

---

# 25. Deleting a Node from a Binary Tree

Deletion in a normal binary tree is more complicated than BST deletion.

A common method:

1. Find the node to delete.
2. Find the deepest/rightmost node.
3. Copy deepest node's value into target node.
4. Delete the deepest node.

Example:

```text
             10
            /  \
           20   30
          / \   /
         40 50 60
```

Delete `20`.

Deepest/rightmost node:

```text
60
```

Replace:

```text
20 → 60
```

Then delete original `60`.

Result:

```text
             10
            /  \
           60   30
          / \
         40 50
```

---

# 26. Delete Deepest Node

Helper function:

```cpp
void deleteDeepest(Node* root, Node* target) {

    queue<Node*> q;
    q.push(root);

    while (!q.empty()) {

        Node* current = q.front();
        q.pop();

        if (current->left) {

            if (current->left == target) {
                delete current->left;
                current->left = nullptr;
                return;
            }

            q.push(current->left);
        }

        if (current->right) {

            if (current->right == target) {
                delete current->right;
                current->right = nullptr;
                return;
            }

            q.push(current->right);
        }
    }
}
```

---

# 27. Find Deepest Node

```cpp
Node* getDeepestNode(Node* root) {

    queue<Node*> q;
    q.push(root);

    Node* current = nullptr;

    while (!q.empty()) {

        current = q.front();
        q.pop();

        if (current->left)
            q.push(current->left);

        if (current->right)
            q.push(current->right);
    }

    return current;
}
```

---

# 28. Delete a Given Value

Complete implementation:

```cpp
void deleteNode(Node*& root, int value) {

    if (root == nullptr)
        return;

    if (root->left == nullptr &&
        root->right == nullptr) {

        if (root->data == value) {
            delete root;
            root = nullptr;
        }

        return;
    }

    queue<Node*> q;
    q.push(root);

    Node* target = nullptr;
    Node* deepest = nullptr;

    while (!q.empty()) {

        Node* current = q.front();
        q.pop();

        if (current->data == value)
            target = current;

        deepest = current;

        if (current->left)
            q.push(current->left);

        if (current->right)
            q.push(current->right);
    }

    if (target == nullptr)
        return;

    target->data = deepest->data;

    deleteDeepest(root, deepest);
}
```

This is enough for a normal binary-tree implementation.

---

# 29. Important Difference: Binary Tree vs BST

This will save you from a lot of confusion.

### Binary Tree

No ordering requirement.

```text
        50
       /  \
      80   20
```

This is perfectly valid.

### Binary Search Tree

Must follow:

```text
LEFT < ROOT < RIGHT
```

For example:

```text
        50
       /  \
      30   70
     / \   / \
    20 40 60 80
```

Therefore:

> **Insertion/deletion/search are much easier and more efficient in a BST because of the ordering property.**

We'll cover BST separately after binary trees.

---

# 30. Check if Two Trees are Identical

Very common interview question.

Example:

```text
Tree 1:       Tree 2:

   10            10
  /  \          /  \
 20  30        20   30
```

They are identical.

### Code

```cpp
bool identical(Node* root1, Node* root2) {

    if (root1 == nullptr &&
        root2 == nullptr)
        return true;

    if (root1 == nullptr ||
        root2 == nullptr)
        return false;

    return root1->data == root2->data &&
           identical(root1->left, root2->left) &&
           identical(root1->right, root2->right);
}
```

---

# 31. Count Nodes at a Particular Level

For example:

```text
             10          Level 0
            /  \
           20   30       Level 1
          / \   /
         40 50 60        Level 2
```

Count nodes at level 2:

```text
3
```

Code:

```cpp
int countAtLevel(Node* root, int level) {

    if (root == nullptr)
        return 0;

    if (level == 0)
        return 1;

    return countAtLevel(root->left, level - 1)
         + countAtLevel(root->right, level - 1);
}
```

---

# 32. Print Nodes at K Distance from Root

```cpp
void printKDistance(Node* root, int k) {

    if (root == nullptr)
        return;

    if (k == 0) {
        cout << root->data << " ";
        return;
    }

    printKDistance(root->left, k - 1);
    printKDistance(root->right, k - 1);
}
```

---

# 33. Find Depth of a Given Node

Suppose:

```text
        10
       /  \
      20   30
     /
    40
```

Depth of 40:

```text
2
```

Code:

```cpp
int findDepth(Node* root, int key, int depth) {

    if (root == nullptr)
        return -1;

    if (root->data == key)
        return depth;

    int left = findDepth(root->left, key, depth + 1);

    if (left != -1)
        return left;

    return findDepth(root->right, key, depth + 1);
}
```

Call:

```cpp
cout << findDepth(root, 40, 0);
```

---

# 34. Mirror of a Binary Tree

Original:

```text
        10
       /  \
      20   30
     / \
    40 50
```

Mirror:

```text
        10
       /  \
      30   20
          /  \
         50   40
```

The left and right children are swapped recursively.

### Code

```cpp
void mirror(Node* root) {

    if (root == nullptr)
        return;

    swap(root->left, root->right);

    mirror(root->left);
    mirror(root->right);
}
```

---

# 35. Diameter of Binary Tree

This is a **very important interview/coding-round problem**.

Diameter means the longest path between two nodes.

Example:

```text
             10
            /  \
           20   30
          / \
         40 50
```

One longest path:

```text
40 → 20 → 10 → 30
```

Number of edges:

```text
3
```

A simple solution:

```cpp
int height(Node* root) {

    if (root == nullptr)
        return 0;

    return 1 + max(
        height(root->left),
        height(root->right)
    );
}

int diameter(Node* root) {

    if (root == nullptr)
        return 0;

    int leftHeight = height(root->left);
    int rightHeight = height(root->right);

    int currentDiameter =
        leftHeight + rightHeight;

    int leftDiameter = diameter(root->left);
    int rightDiameter = diameter(root->right);

    return max(currentDiameter,
               max(leftDiameter, rightDiameter));
}
```

This works but can become **O(N²)**.

An optimized O(N) version is better for interviews; we'll cover that as an important tree pattern.

---

# 36. Check if Tree is Balanced

A tree is balanced if the height difference between left and right subtree is not more than 1 at every node.

Example:

```text
        10
       /  \
      20   30
     /
    40
```

is balanced.

But:

```text
10
/
20
/
30
/
40
```

is not balanced.

Optimized solution:

```cpp
int checkHeight(Node* root) {

    if (root == nullptr)
        return 0;

    int left = checkHeight(root->left);

    if (left == -1)
        return -1;

    int right = checkHeight(root->right);

    if (right == -1)
        return -1;

    if (abs(left - right) > 1)
        return -1;

    return 1 + max(left, right);
}

bool isBalanced(Node* root) {

    return checkHeight(root) != -1;
}
```

This runs in:

```text
O(N)
```

---

# 37. Lowest Common Ancestor — LCA

Very common.

Suppose:

```text
             10
            /  \
           20   30
          / \
         40 50
```

LCA of `40` and `50`:

```text
20
```

LCA means:

> Lowest node that has both nodes somewhere below it.

Code:

```cpp
Node* LCA(Node* root, int a, int b) {

    if (root == nullptr)
        return nullptr;

    if (root->data == a ||
        root->data == b)
        return root;

    Node* left = LCA(root->left, a, b);
    Node* right = LCA(root->right, a, b);

    if (left != nullptr && right != nullptr)
        return root;

    if (left != nullptr)
        return left;

    return right;
}
```

---

# 38. Root-to-Leaf Paths

Given:

```text
        10
       /  \
      20   30
     / \
    40 50
```

Paths:

```text
10 → 20 → 40
10 → 20 → 50
10 → 30
```

Code:

```cpp
void printPaths(Node* root, vector<int>& path) {

    if (root == nullptr)
        return;

    path.push_back(root->data);

    if (root->left == nullptr &&
        root->right == nullptr) {

        for (int x : path)
            cout << x << " ";

        cout << endl;
    }

    printPaths(root->left, path);
    printPaths(root->right, path);

    path.pop_back();
}
```

The important concept here is:

```text
push
↓
recursion
↓
pop
```

This is also a major **backtracking pattern**.

---

# 39. Sum of Root-to-Leaf Numbers

For example:

```text
      1
     / \
    2   3
```

Paths represent:

```text
12
13
```

Sum:

```text
25
```

Code:

```cpp
int sumNumbers(Node* root, int current) {

    if (root == nullptr)
        return 0;

    current = current * 10 + root->data;

    if (root->left == nullptr &&
        root->right == nullptr)
        return current;

    return sumNumbers(root->left, current)
         + sumNumbers(root->right, current);
}
```

---

# 40. Right View of Binary Tree

Example:

```text
             10
            /  \
           20   30
          / \     \
         40 50     60
```

Right view:

```text
10 30 60
```

The nodes visible from the right side.

A simple BFS solution:

```cpp
vector<int> rightView(Node* root) {

    vector<int> ans;

    if (root == nullptr)
        return ans;

    queue<Node*> q;
    q.push(root);

    while (!q.empty()) {

        int n = q.size();

        for (int i = 0; i < n; i++) {

            Node* current = q.front();
            q.pop();

            if (i == n - 1)
                ans.push_back(current->data);

            if (current->left)
                q.push(current->left);

            if (current->right)
                q.push(current->right);
        }
    }

    return ans;
}
```

---

# 41. Left View

Same concept.

```text
             10
            /  \
           20   30
          / \
         40 50
```

Left view:

```text
10 20 40
```

Code:

```cpp
vector<int> leftView(Node* root) {

    vector<int> ans;

    if (root == nullptr)
        return ans;

    queue<Node*> q;
    q.push(root);

    while (!q.empty()) {

        int n = q.size();

        for (int i = 0; i < n; i++) {

            Node* current = q.front();
            q.pop();

            if (i == 0)
                ans.push_back(current->data);

            if (current->left)
                q.push(current->left);

            if (current->right)
                q.push(current->right);
        }
    }

    return ans;
}
```

---

# 42. Zig-Zag Traversal

Normal:

```text
10
20 30
40 50 60
```

Zig-zag:

```text
10
30 20
40 50 60
```

Next level reverses again.

Code:

```cpp
vector<vector<int>> zigzag(Node* root) {

    vector<vector<int>> ans;

    if (root == nullptr)
        return ans;

    queue<Node*> q;
    q.push(root);

    bool leftToRight = true;

    while (!q.empty()) {

        int n = q.size();

        vector<int> level(n);

        for (int i = 0; i < n; i++) {

            Node* current = q.front();
            q.pop();

            int index;

            if (leftToRight)
                index = i;
            else
                index = n - 1 - i;

            level[index] = current->data;

            if (current->left)
                q.push(current->left);

            if (current->right)
                q.push(current->right);
        }

        ans.push_back(level);

        leftToRight = !leftToRight;
    }

    return ans;
}
```

---

# 43. Complete Basic Binary Tree Program

You should be comfortable writing something like this from scratch:

```cpp
#include <bits/stdc++.h>
using namespace std;

struct Node {

    int data;
    Node* left;
    Node* right;

    Node(int value) {

        data = value;
        left = nullptr;
        right = nullptr;
    }
};


void preorder(Node* root) {

    if (root == nullptr)
        return;

    cout << root->data << " ";

    preorder(root->left);
    preorder(root->right);
}


void inorder(Node* root) {

    if (root == nullptr)
        return;

    inorder(root->left);

    cout << root->data << " ";

    inorder(root->right);
}


void postorder(Node* root) {

    if (root == nullptr)
        return;

    postorder(root->left);
    postorder(root->right);

    cout << root->data << " ";
}


void levelOrder(Node* root) {

    if (root == nullptr)
        return;

    queue<Node*> q;

    q.push(root);

    while (!q.empty()) {

        Node* current = q.front();
        q.pop();

        cout << current->data << " ";

        if (current->left)
            q.push(current->left);

        if (current->right)
            q.push(current->right);
    }
}


int countNodes(Node* root) {

    if (root == nullptr)
        return 0;

    return 1 +
           countNodes(root->left) +
           countNodes(root->right);
}


int height(Node* root) {

    if (root == nullptr)
        return 0;

    return 1 + max(
        height(root->left),
        height(root->right)
    );
}


int sumNodes(Node* root) {

    if (root == nullptr)
        return 0;

    return root->data +
           sumNodes(root->left) +
           sumNodes(root->right);
}


bool search(Node* root, int key) {

    if (root == nullptr)
        return false;

    if (root->data == key)
        return true;

    return search(root->left, key) ||
           search(root->right, key);
}


int main() {

    Node* root = new Node(10);

    root->left = new Node(20);
    root->right = new Node(30);

    root->left->left = new Node(40);
    root->left->right = new Node(50);

    root->right->left = new Node(60);

    cout << "Preorder: ";
    preorder(root);

    cout << "\nInorder: ";
    inorder(root);

    cout << "\nPostorder: ";
    postorder(root);

    cout << "\nLevel Order: ";
    levelOrder(root);

    cout << "\nNumber of nodes: ";
    cout << countNodes(root);

    cout << "\nHeight: ";
    cout << height(root);

    cout << "\nSum: ";
    cout << sumNodes(root);

    cout << "\nSearch 50: ";

    if (search(root, 50))
        cout << "Found";
    else
        cout << "Not Found";

    return 0;
}
```

---

# 44. Expected Binary Tree Questions

For your DSA/coding preparation, I would divide them into three levels.

## 🟢 Level 1 — Must Know

These are basic implementation questions.

| #  | Question              |
| -- | --------------------- |
| 1  | Create a binary tree  |
| 2  | Print preorder        |
| 3  | Print inorder         |
| 4  | Print postorder       |
| 5  | Level order traversal |
| 6  | Count total nodes     |
| 7  | Count leaf nodes      |
| 8  | Find height           |
| 9  | Find minimum          |
| 10 | Find maximum          |
| 11 | Find sum              |
| 12 | Search for a value    |
| 13 | Update a node         |
| 14 | Insert a node         |
| 15 | Delete a node         |

You should be able to implement these **without looking at notes**.

---

# 45. 🟡 Level 2 — Very Important

These are extremely common in DSA labs/interviews.

### 16. Find depth of a node

### 17. Print nodes at K distance

### 18. Mirror a binary tree

### 19. Check whether two trees are identical

### 20. Count nodes at each level

### 21. Level with maximum number of nodes

### 22. Sum of nodes at each level

### 23. Left view

### 24. Right view

### 25. Top view

### 26. Bottom view

### 27. Zig-zag traversal

### 28. Check balanced tree

### 29. Diameter of tree

### 30. Lowest Common Ancestor

### 31. Root-to-leaf paths

### 32. Maximum root-to-leaf sum

---

# 46. 🔴 Level 3 — Interview/Advanced

After the above, learn:

### 33. Boundary traversal

### 34. Vertical order traversal

### 35. Maximum path sum

### 36. Serialize and deserialize a tree

### 37. Construct tree from preorder + inorder

### 38. Construct tree from postorder + inorder

### 39. Convert binary tree to doubly linked list

### 40. Flatten binary tree to linked list

### 41. Check whether tree is symmetric

### 42. Burn a binary tree

### 43. Distance between two nodes

### 44. Nodes at distance K from target

### 45. Morris traversal

These are worth doing once your basic recursion and traversal patterns are strong.

---

# 47. The Most Important Patterns to Recognize

This is more important than memorizing 40 programs.

### Pattern 1 — Visit every node

Usually:

```cpp
if(root == nullptr)
    return;

solve(root->left);
solve(root->right);
```

Used for:

* search
* count
* sum
* max/min
* height
* diameter
* balanced tree

---

### Pattern 2 — Root + Left + Right

Think:

```text
PREORDER
```

Useful for:

* copying trees
* serialization
* root-to-leaf problems

---

### Pattern 3 — Left + Root + Right

Think:

```text
INORDER
```

Very important later for:

```text
Binary Search Tree
```

because inorder of a BST gives sorted order.

---

### Pattern 4 — Left + Right + Root

Think:

```text
POSTORDER
```

Useful when children must be processed before parent.

For example:

```text
delete/free entire tree
```

---

### Pattern 5 — Level by level

Immediately think:

```text
QUEUE
```

Used in:

* level order
* left view
* right view
* zigzag
* minimum depth
* BFS
* level-based problems

---

### Pattern 6 — Root-to-leaf

Think:

```text
DFS + vector/path
```

Used in:

* path printing
* path sum
* all root-to-leaf paths
* maximum path-type problems

---

# 48. How to Delete the Entire Tree

This is also a useful question.

We should delete children before deleting parent.

Therefore:

```text
POSTORDER
```

is natural.

```cpp
void deleteTree(Node*& root) {

    if (root == nullptr)
        return;

    deleteTree(root->left);
    deleteTree(root->right);

    delete root;

    root = nullptr;
}
```

Why postorder?

Because:

```text
Left
Right
Root
```

We delete children first and parent last.

---

# 49. Complexity

For a normal binary tree with `N` nodes:

| Operation            | Complexity |
| -------------------- | ---------: |
| Search               |       O(N) |
| Traversal            |       O(N) |
| Count nodes          |       O(N) |
| Height               |       O(N) |
| Find min             |       O(N) |
| Find max             |       O(N) |
| Sum                  |       O(N) |
| Insert (level order) |       O(N) |
| Delete               |       O(N) |

Why are most operations `O(N)`?

Because a normal binary tree gives us **no information about where a particular value is located**.

That changes dramatically when we study the **Binary Search Tree (BST)**.

---

# 50. One Very Important Warning

Do **not** mix these three structures:

```text
Binary Tree
Binary Search Tree
Heap
```

They are different.

### Binary Tree

```text
At most 2 children
```

No ordering requirement.

### BST

```text
Left < Root < Right
```

Used for efficient searching/insertion/deletion.

### Heap

Usually a complete binary tree with:

```text
Parent >= children   (max heap)
```

or

```text
Parent <= children   (min heap)
```

Used heavily in:

```text
priority_queue
heap sort
top K problems
Dijkstra
```

---

# 51. What You Should Be Able to Write in a Coding Round

For **Binary Trees**, practice writing these from an empty editor:

### Basic — absolutely required

```cpp
struct Node
```

```cpp
preorder()
```

```cpp
inorder()
```

```cpp
postorder()
```

```cpp
levelOrder()
```

```cpp
search()
```

```cpp
countNodes()
```

```cpp
countLeaves()
```

```cpp
height()
```

```cpp
findMax()
```

```cpp
findMin()
```

```cpp
sum()
```

```cpp
insert()
```

```cpp
deleteNode()
```

```cpp
update()
```

### Then master:

```text
Mirror
Identical trees
K-distance
LCA
Diameter
Balanced tree
Left view
Right view
Zigzag
Root-to-leaf paths
Maximum path sum
Tree construction
```

---

# 52. Your Binary Tree Learning Order

I recommend doing the topic in this exact order:

```text
                    BINARY TREE
                         │
                         ↓
                  Node Structure
                         │
                         ↓
                  Create a Tree
                         │
             ┌───────────┴───────────┐
             ↓                       ↓
         Recursion                 Queue
             │                       │
             ↓                       ↓
       Pre/In/Post             Level Order
             │
             ↓
       Basic Operations
             │
      ┌──────┼─────────┐
      ↓      ↓         ↓
    Search  Insert    Delete
      │
      ↓
 Height / Count / Sum / Min / Max
      │
      ↓
   Tree Problems
      │
 ┌────┼────┬─────┬──────┐
 ↓    ↓    ↓     ↓      ↓
LCA  View  Dia.  Mirror  Balanced
      │
      ↓
 Advanced Tree Problems
      │
      ↓
 Binary Search Tree
```

### ⭐ Most important takeaway

For binary trees, **recursion is the main weapon**.

Whenever you see:

> "Given a binary tree, find/calculate/check something for all nodes..."

your first thought should usually be:

```cpp
if (root == nullptr)
    return ...;

leftAnswer = solve(root->left);
rightAnswer = solve(root->right);

return combine(root, leftAnswer, rightAnswer);
```

And whenever the question says:

> **"level by level"**

think:

```cpp
queue<Node*> q;
```

That two-pattern understanding will solve a surprisingly large portion of binary-tree problems.


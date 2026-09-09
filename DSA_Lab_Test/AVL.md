# 🌳 AVL Tree — Complete C++ DSA Tutorial

Now we move one step further:

```text
Binary Tree
     ↓
Binary Search Tree (BST)
     ↓
AVL Tree
```

An **AVL Tree** is basically a **self-balancing Binary Search Tree**.

The most important things to understand are:

1. What AVL means
2. Balance factor
3. Rotations
4. LL, RR, LR, RL cases
5. Insertion
6. Deletion
7. Search
8. Update/Edit
9. Traversals
10. Height
11. Minimum/Maximum
12. Expected coding-round questions

---

# 1. What is an AVL Tree?

AVL stands for:

**Adelson-Velsky and Landis**

An AVL tree is a BST that automatically keeps itself **balanced**.

Remember the problem with an ordinary BST.

If we insert:

```text
10
20
30
40
50
```

into a normal BST:

```text
10
  \
   20
     \
      30
        \
         40
           \
            50
```

This is technically a BST, but it has become almost like a linked list.

Its height is:

```text
O(N)
```

Therefore search can become:

```text
O(N)
```

An AVL tree prevents this from happening.

---

# 2. AVL Tree's Main Rule

For every node:

```text
|height(left subtree) - height(right subtree)| <= 1
```

In other words, the difference between the heights of the two children can only be:

```text
-1
0
+1
```

This difference is called the:

# Balance Factor

---

# 3. Balance Factor

Formula:

```text
Balance Factor =
height(left subtree) - height(right subtree)
```

For example:

```text
        50
       /  \
      30   70
```

Both subtrees have the same height.

Therefore:

```text
BF(50) = 0
```

---

Consider:

```text
       50
      /
     30
```

Left height = 1

Right height = 0

Therefore:

```text
BF(50) = 1 - 0
       = 1
```

Still balanced.

---

Consider:

```text
        50
       /
      30
     /
    20
```

At `50`:

```text
left height  = 2
right height = 0

BF = 2 - 0
   = 2
```

Therefore:

```text
❌ Unbalanced
```

---

# 4. Valid Balance Factors

An AVL tree requires:

```text
BF = -1 → balanced
BF =  0 → balanced
BF = +1 → balanced
```

But:

```text
BF = +2 → unbalanced
BF = -2 → unbalanced
```

So:

```text
        Balance Factor

             0
           /   \
         -1     +1
           
       Valid

      -2 or +2
          ↓
      Rotation needed
```

This is the **most important concept in AVL trees**.

---

# 5. AVL Node Structure

We need one extra piece of information compared with a normal BST.

A normal BST node:

```cpp
struct Node {
    int data;
    Node* left;
    Node* right;
};
```

AVL node:

```cpp
struct Node {
    int data;
    Node* left;
    Node* right;
    int height;
};
```

Why do we store `height`?

Because we repeatedly need:

```text
height(left)
height(right)
```

to calculate the balance factor.

---

# 6. Node Constructor

```cpp
struct Node {

    int data;

    Node* left;
    Node* right;

    int height;

    Node(int value) {

        data = value;

        left = nullptr;
        right = nullptr;

        height = 1;
    }
};
```

Why:

```cpp
height = 1;
```

?

Because a newly created node is a tree containing one node.

We are using:

```text
height(NULL) = 0
```

Therefore:

```text
       10

height = 1
```

---

# 7. Get Height

Very simple:

```cpp
int getHeight(Node* root) {

    if (root == nullptr)
        return 0;

    return root->height;
}
```

---

# 8. Get Balance Factor

```cpp
int getBalance(Node* root) {

    if (root == nullptr)
        return 0;

    return getHeight(root->left)
         - getHeight(root->right);
}
```

For example:

```text
       50
      /
     30
```

```text
height(30) = 1
height(NULL) = 0

BF(50) = 1 - 0
       = 1
```

---

# 9. Update Height

Whenever the tree structure changes, we need to update the height.

Formula:

```text
height(node)
=
1 + max(
    height(left),
    height(right)
)
```

Code:

```cpp
void updateHeight(Node* root) {

    root->height =
        1 + max(
            getHeight(root->left),
            getHeight(root->right)
        );
}
```

This function is extremely important.

---

# 10. Why Rotations?

Suppose we insert:

```text
30
20
10
```

BST gives:

```text
      30
     /
    20
   /
  10
```

Balance factor of 30:

```text
BF = 2
```

Unbalanced.

AVL says:

> We need to rearrange the tree while preserving BST ordering.

This rearrangement is called a:

# Rotation

---

# 11. Four AVL Rotation Cases

You absolutely must know these four:

```text
1. LL
2. RR
3. LR
4. RL
```

A very easy way to remember:

```text
LL → Right Rotation

RR → Left Rotation

LR → Left Rotation + Right Rotation

RL → Right Rotation + Left Rotation
```

---

# 12. LL Case

Insert:

```text
30
20
10
```

Tree:

```text
        30
       /
      20
     /
    10
```

The imbalance is on:

```text
Left → Left
```

Therefore:

```text
LL case
```

We perform:

```text
RIGHT ROTATION
```

Result:

```text
       20
      /  \
     10   30
```

---

# 13. Right Rotation

This is the first rotation you should understand properly.

Before:

```text
        y
       /
      x
     /
    T1
```

More completely:

```text
          y
         / \
        x   T4
       / \
      T1  T2
```

After right rotation:

```text
          x
         / \
        T1  y
           / \
          T2 T4
```

The BST ordering is preserved.

---

# 14. C++ Right Rotation

```cpp
Node* rightRotate(Node* y) {

    Node* x = y->left;

    Node* T2 = x->right;

    // Rotation
    x->right = y;

    y->left = T2;

    // Update heights
    updateHeight(y);
    updateHeight(x);

    return x;
}
```

This code is worth understanding extremely well.

---

# 15. RR Case

Insert:

```text
10
20
30
```

BST:

```text
10
  \
   20
     \
      30
```

Imbalance:

```text
Right → Right
```

Therefore:

```text
RR case
```

We perform:

```text
LEFT ROTATION
```

Result:

```text
       20
      /  \
     10   30
```

---

# 16. Left Rotation

Before:

```text
          x
         / \
        T1  y
           / \
          T2 T3
```

After:

```text
          y
         / \
        x   T3
       / \
      T1 T2
```

Code:

```cpp
Node* leftRotate(Node* x) {

    Node* y = x->right;

    Node* T2 = y->left;

    // Rotation
    y->left = x;

    x->right = T2;

    // Update heights
    updateHeight(x);
    updateHeight(y);

    return y;
}
```

---

# 17. LR Case

This one is slightly more complicated.

Insert:

```text
30
10
20
```

Tree:

```text
       30
      /
     10
       \
        20
```

The imbalance is:

```text
Left → Right
```

Therefore:

```text
LR
```

We need:

```text
LEFT ROTATION
        ↓
RIGHT ROTATION
```

First:

```text
       30
      /
     20
    /
   10
```

Then right rotate:

```text
       20
      /  \
     10   30
```

---

# 18. RL Case

Insert:

```text
10
30
20
```

Tree:

```text
10
  \
   30
   /
  20
```

Imbalance:

```text
Right → Left
```

Therefore:

```text
RL
```

Perform:

```text
RIGHT ROTATION
       ↓
LEFT ROTATION
```

Result:

```text
       20
      /  \
     10   30
```

---

# 19. Four Cases — Memorize This Table

| Case | Example insertion | Rotation     |
| ---- | ----------------- | ------------ |
| LL   | 30,20,10          | Right        |
| RR   | 10,20,30          | Left         |
| LR   | 30,10,20          | Left + Right |
| RL   | 10,30,20          | Right + Left |

This table is **extremely important**.

---

# 20. How Do We Identify the Case?

Suppose:

```text
balance > 1
```

The tree is too heavy on the **left**.

Then inspect the inserted value.

### If value goes into left child's left side:

```text
LL
```

Right rotation.

### If value goes into left child's right side:

```text
LR
```

Left rotation on child, then right rotation.

---

If:

```text
balance < -1
```

the tree is too heavy on the **right**.

### If value goes into right child's right side:

```text
RR
```

Left rotation.

### If value goes into right child's left side:

```text
RL
```

Right rotation on child, then left rotation.

---

# 21. AVL Insertion

Now we can combine everything.

Algorithm:

```text
1. Insert like normal BST
2. Update height
3. Calculate balance factor
4. If unbalanced:
       LL → right rotation
       RR → left rotation
       LR → left + right
       RL → right + left
5. Return new root
```

---

# 22. Complete AVL Insert Code

```cpp
Node* insert(Node* root, int value) {

    // Normal BST insertion
    if (root == nullptr)
        return new Node(value);

    if (value < root->data) {

        root->left =
            insert(root->left, value);
    }

    else if (value > root->data) {

        root->right =
            insert(root->right, value);
    }

    else {

        // Duplicate
        return root;
    }


    // Update height
    updateHeight(root);


    // Calculate balance factor
    int balance = getBalance(root);


    // LL Case
    if (balance > 1 &&
        value < root->left->data) {

        return rightRotate(root);
    }


    // RR Case
    if (balance < -1 &&
        value > root->right->data) {

        return leftRotate(root);
    }


    // LR Case
    if (balance > 1 &&
        value > root->left->data) {

        root->left =
            leftRotate(root->left);

        return rightRotate(root);
    }


    // RL Case
    if (balance < -1 &&
        value < root->right->data) {

        root->right =
            rightRotate(root->right);

        return leftRotate(root);
    }


    return root;
}
```

This is the **core AVL insertion implementation**.

---

# 23. Let's Trace AVL Insertion

Insert:

```text
30, 20, 10
```

### Insert 30

```text
30
```

Balanced.

---

### Insert 20

```text
   30
  /
20
```

Balance:

```text
+1
```

Still balanced.

---

### Insert 10

```text
    30
   /
  20
 /
10
```

At 30:

```text
BF = +2
```

And:

```text
10 < 20
```

Therefore:

```text
LL
```

Right rotation.

Result:

```text
     20
    /  \
   10   30
```

Balanced again.

---

# 24. AVL Search

Search is exactly like BST.

```cpp
bool search(Node* root, int key) {

    if (root == nullptr)
        return false;

    if (root->data == key)
        return true;

    if (key < root->data)
        return search(root->left, key);

    return search(root->right, key);
}
```

Because AVL is still a BST.

---

# 25. Iterative Search

```cpp
bool search(Node* root, int key) {

    while (root != nullptr) {

        if (root->data == key)
            return true;

        if (key < root->data)
            root = root->left;
        else
            root = root->right;
    }

    return false;
}
```

---

# 26. AVL Inorder Traversal

Exactly the same as BST.

```cpp
void inorder(Node* root) {

    if (root == nullptr)
        return;

    inorder(root->left);

    cout << root->data << " ";

    inorder(root->right);
}
```

And remember:

> **Inorder traversal of an AVL tree is sorted.**

Because an AVL tree is a BST.

---

# 27. Preorder

```cpp
void preorder(Node* root) {

    if (root == nullptr)
        return;

    cout << root->data << " ";

    preorder(root->left);
    preorder(root->right);
}
```

---

# 28. Postorder

```cpp
void postorder(Node* root) {

    if (root == nullptr)
        return;

    postorder(root->left);
    postorder(root->right);

    cout << root->data << " ";
}
```

---

# 29. Level Order

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

        if (current->left)
            q.push(current->left);

        if (current->right)
            q.push(current->right);
    }
}
```

---

# 30. Find Minimum

Same as BST.

Keep going left:

```cpp
Node* minNode(Node* root) {

    while (root->left != nullptr)
        root = root->left;

    return root;
}
```

---

# 31. Find Maximum

Keep going right:

```cpp
Node* maxNode(Node* root) {

    while (root->right != nullptr)
        root = root->right;

    return root;
}
```

---

# 32. AVL Deletion — VERY IMPORTANT

Deletion is more difficult than insertion.

The overall process is:

```text
1. Delete like normal BST
2. Update height
3. Calculate balance factor
4. Rebalance using rotations
```

Remember:

```text
BST deletion
     +
AVL rebalancing
```

---

# 33. AVL Deletion Cases

First, ordinary BST deletion still has three cases.

### Case 1

Node has no children.

```text
Delete it.
```

### Case 2

Node has one child.

```text
Replace node with child.
```

### Case 3

Node has two children.

```text
Use inorder successor.
```

Then comes the AVL-specific part:

```text
Update height
Calculate balance
Rotate if required
```

---

# 34. AVL Delete Code

```cpp
Node* deleteNode(Node* root, int key) {

    // Normal BST deletion
    if (root == nullptr)
        return root;


    if (key < root->data) {

        root->left =
            deleteNode(root->left, key);
    }

    else if (key > root->data) {

        root->right =
            deleteNode(root->right, key);
    }

    else {

        // Node with 0 or 1 child

        if (root->left == nullptr ||
            root->right == nullptr) {

            Node* child;

            if (root->left != nullptr)
                child = root->left;
            else
                child = root->right;


            // No child
            if (child == nullptr) {

                delete root;

                return nullptr;
            }

            // One child
            else {

                Node* temp = child;

                delete root;

                return temp;
            }
        }


        // Node with two children

        Node* successor =
            minNode(root->right);

        root->data = successor->data;

        root->right =
            deleteNode(root->right,
                       successor->data);
    }


    // Update height
    updateHeight(root);


    // Balance factor
    int balance = getBalance(root);


    // LL Case
    if (balance > 1 &&
        getBalance(root->left) >= 0) {

        return rightRotate(root);
    }


    // LR Case
    if (balance > 1 &&
        getBalance(root->left) < 0) {

        root->left =
            leftRotate(root->left);

        return rightRotate(root);
    }


    // RR Case
    if (balance < -1 &&
        getBalance(root->right) <= 0) {

        return leftRotate(root);
    }


    // RL Case
    if (balance < -1 &&
        getBalance(root->right) > 0) {

        root->right =
            rightRotate(root->right);

        return leftRotate(root);
    }


    return root;
}
```

This is one of the most important AVL programs you should practice.

---

# 35. Why Are Delete Conditions Different?

Notice insertion used:

```cpp
value < root->left->data
```

etc.

But deletion uses:

```cpp
getBalance(root->left)
```

Why?

During insertion, we know **which value was inserted** and can determine the direction.

During deletion, the imbalance can result from a subtree becoming shorter, so looking at the child subtree's **balance factor** is safer.

Therefore deletion uses:

```text
Balance of current node
+
Balance of child
```

to determine the rotation.

---

# 36. AVL Update/Edit

Same issue as BST.

Suppose:

```text
       50
      /  \
    30    70
```

Changing:

```text
30 → 100
```

directly would violate the BST property.

Therefore:

```text
UPDATE
  ↓
Delete old value
  ↓
Insert new value
  ↓
AVL automatically balances
```

Code:

```cpp
Node* update(Node* root,
             int oldValue,
             int newValue) {

    root = deleteNode(root, oldValue);

    root = insert(root, newValue);

    return root;
}
```

This is the safest general implementation.

---

# 37. Count Nodes

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

# 38. Count Leaf Nodes

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

# 39. Height of AVL Tree

Since height is already stored:

```cpp
int height(Node* root) {

    if (root == nullptr)
        return 0;

    return root->height;
}
```

This is different from calculating it recursively every time.

That's one of the reasons we store height in each AVL node.

---

# 40. Check Whether an AVL Tree is Balanced

You can calculate balance recursively:

```cpp
bool isBalanced(Node* root) {

    if (root == nullptr)
        return true;

    int balance = getBalance(root);

    if (abs(balance) > 1)
        return false;

    return isBalanced(root->left) &&
           isBalanced(root->right);
}
```

---

# 41. Find Kth Smallest

Because AVL is a BST:

```text
Inorder = sorted
```

So the same BST technique works.

```cpp
void kthSmallest(Node* root,
                 int& k,
                 int& answer) {

    if (root == nullptr)
        return;

    kthSmallest(root->left, k, answer);

    k--;

    if (k == 0) {
        answer = root->data;
        return;
    }

    kthSmallest(root->right, k, answer);
}
```

---

# 42. Find LCA in AVL

Again, AVL is a BST.

Therefore:

```cpp
Node* LCA(Node* root, int a, int b) {

    while (root != nullptr) {

        if (a < root->data &&
            b < root->data) {

            root = root->left;
        }

        else if (a > root->data &&
                 b > root->data) {

            root = root->right;
        }

        else {

            return root;
        }
    }

    return nullptr;
}
```

---

# 43. Find Floor

Same as BST:

```cpp
int floorValue(Node* root, int x) {

    int answer = -1;

    while (root != nullptr) {

        if (root->data == x)
            return x;

        if (root->data < x) {

            answer = root->data;
            root = root->right;
        }

        else {

            root = root->left;
        }
    }

    return answer;
}
```

---

# 44. Find Ceil

```cpp
int ceilValue(Node* root, int x) {

    int answer = -1;

    while (root != nullptr) {

        if (root->data == x)
            return x;

        if (root->data > x) {

            answer = root->data;
            root = root->left;
        }

        else {

            root = root->right;
        }
    }

    return answer;
}
```

---

# 45. AVL Tree Complete Implementation

This is the **main program you should practice**.

```cpp
#include <bits/stdc++.h>
using namespace std;


struct Node {

    int data;

    Node* left;
    Node* right;

    int height;

    Node(int value) {

        data = value;

        left = nullptr;
        right = nullptr;

        height = 1;
    }
};


// -------------------------
// HEIGHT
// -------------------------

int getHeight(Node* root) {

    if (root == nullptr)
        return 0;

    return root->height;
}


// -------------------------
// BALANCE FACTOR
// -------------------------

int getBalance(Node* root) {

    if (root == nullptr)
        return 0;

    return getHeight(root->left)
         - getHeight(root->right);
}


// -------------------------
// UPDATE HEIGHT
// -------------------------

void updateHeight(Node* root) {

    root->height =
        1 + max(
            getHeight(root->left),
            getHeight(root->right)
        );
}


// -------------------------
// RIGHT ROTATION
// -------------------------

Node* rightRotate(Node* y) {

    Node* x = y->left;

    Node* T2 = x->right;

    x->right = y;

    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}


// -------------------------
// LEFT ROTATION
// -------------------------

Node* leftRotate(Node* x) {

    Node* y = x->right;

    Node* T2 = y->left;

    y->left = x;

    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}


// -------------------------
// INSERT
// -------------------------

Node* insert(Node* root, int value) {

    // Normal BST insertion

    if (root == nullptr)
        return new Node(value);


    if (value < root->data) {

        root->left =
            insert(root->left, value);
    }

    else if (value > root->data) {

        root->right =
            insert(root->right, value);
    }

    else {

        return root;
    }


    // Update height

    updateHeight(root);


    // Balance

    int balance = getBalance(root);


    // LL

    if (balance > 1 &&
        value < root->left->data) {

        return rightRotate(root);
    }


    // RR

    if (balance < -1 &&
        value > root->right->data) {

        return leftRotate(root);
    }


    // LR

    if (balance > 1 &&
        value > root->left->data) {

        root->left =
            leftRotate(root->left);

        return rightRotate(root);
    }


    // RL

    if (balance < -1 &&
        value < root->right->data) {

        root->right =
            rightRotate(root->right);

        return leftRotate(root);
    }


    return root;
}


// -------------------------
// MINIMUM NODE
// -------------------------

Node* minNode(Node* root) {

    Node* current = root;

    while (current->left != nullptr)
        current = current->left;

    return current;
}


// -------------------------
// DELETE
// -------------------------

Node* deleteNode(Node* root, int key) {

    if (root == nullptr)
        return root;


    // Search left

    if (key < root->data) {

        root->left =
            deleteNode(root->left, key);
    }


    // Search right

    else if (key > root->data) {

        root->right =
            deleteNode(root->right, key);
    }


    // Node found

    else {

        // 0 or 1 child

        if (root->left == nullptr ||
            root->right == nullptr) {

            Node* child;

            if (root->left)
                child = root->left;
            else
                child = root->right;


            // No child

            if (child == nullptr) {

                delete root;

                return nullptr;
            }


            // One child

            Node* temp = child;

            delete root;

            return temp;
        }


        // Two children

        Node* successor =
            minNode(root->right);

        root->data = successor->data;

        root->right =
            deleteNode(root->right,
                       successor->data);
    }


    // Update height

    updateHeight(root);


    // Balance factor

    int balance = getBalance(root);


    // LL

    if (balance > 1 &&
        getBalance(root->left) >= 0) {

        return rightRotate(root);
    }


    // LR

    if (balance > 1 &&
        getBalance(root->left) < 0) {

        root->left =
            leftRotate(root->left);

        return rightRotate(root);
    }


    // RR

    if (balance < -1 &&
        getBalance(root->right) <= 0) {

        return leftRotate(root);
    }


    // RL

    if (balance < -1 &&
        getBalance(root->right) > 0) {

        root->right =
            rightRotate(root->right);

        return leftRotate(root);
    }


    return root;
}


// -------------------------
// SEARCH
// -------------------------

bool search(Node* root, int key) {

    while (root != nullptr) {

        if (root->data == key)
            return true;

        if (key < root->data)
            root = root->left;
        else
            root = root->right;
    }

    return false;
}


// -------------------------
// INORDER
// -------------------------

void inorder(Node* root) {

    if (root == nullptr)
        return;

    inorder(root->left);

    cout << root->data << " ";

    inorder(root->right);
}


// -------------------------
// PREORDER
// -------------------------

void preorder(Node* root) {

    if (root == nullptr)
        return;

    cout << root->data << " ";

    preorder(root->left);
    preorder(root->right);
}


// -------------------------
// POSTORDER
// -------------------------

void postorder(Node* root) {

    if (root == nullptr)
        return;

    postorder(root->left);

    postorder(root->right);

    cout << root->data << " ";
}


// -------------------------
// LEVEL ORDER
// -------------------------

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


// -------------------------
// COUNT NODES
// -------------------------

int countNodes(Node* root) {

    if (root == nullptr)
        return 0;

    return 1
         + countNodes(root->left)
         + countNodes(root->right);
}


// -------------------------
// COUNT LEAVES
// -------------------------

int countLeaves(Node* root) {

    if (root == nullptr)
        return 0;

    if (root->left == nullptr &&
        root->right == nullptr)
        return 1;

    return countLeaves(root->left)
         + countLeaves(root->right);
}


// -------------------------
// UPDATE
// -------------------------

Node* update(Node* root,
             int oldValue,
             int newValue) {

    root = deleteNode(root, oldValue);

    root = insert(root, newValue);

    return root;
}


// -------------------------
// MAIN
// -------------------------

int main() {

    Node* root = nullptr;


    vector<int> values = {
        30, 20, 10,
        40, 50,
        25
    };


    // CREATE AVL TREE

    for (int value : values)
        root = insert(root, value);


    cout << "Inorder: ";

    inorder(root);


    cout << "\nPreorder: ";

    preorder(root);


    cout << "\nPostorder: ";

    postorder(root);


    cout << "\nLevel Order: ";

    levelOrder(root);


    cout << "\nHeight: ";

    cout << getHeight(root);


    cout << "\nNodes: ";

    cout << countNodes(root);


    cout << "\nLeaves: ";

    cout << countLeaves(root);


    cout << "\nSearch 25: ";

    if (search(root, 25))
        cout << "Found";
    else
        cout << "Not Found";


    // DELETE

    root = deleteNode(root, 40);


    cout << "\n\nAfter deleting 40: ";

    inorder(root);


    // UPDATE

    root = update(root, 25, 35);


    cout << "\nAfter updating 25 -> 35: ";

    inorder(root);


    return 0;
}
```

---

# 46. The Most Important AVL Algorithm

If you only remember one algorithm, remember:

```text
              INSERT / DELETE
                    |
                    ↓
             Normal BST operation
                    |
                    ↓
             Update height
                    |
                    ↓
           Calculate balance factor
                    |
            ┌───────┴────────┐
            ↓                ↓
       balance > 1       balance < -1
            ↓                ↓
        Left heavy        Right heavy
            ↓                ↓
         LL / LR          RR / RL
            ↓                ↓
         Rotate            Rotate
```

---

# 47. AVL Complexity

This is the biggest reason AVL exists.

Because AVL maintains its height at:

```text
O(log N)
```

Therefore:

| Operation |      AVL |
| --------- | -------: |
| Search    | O(log N) |
| Insert    | O(log N) |
| Delete    | O(log N) |
| Find Min  | O(log N) |
| Find Max  | O(log N) |
| LCA       | O(log N) |
| Floor     | O(log N) |
| Ceil      | O(log N) |

Traversal still requires:

```text
O(N)
```

because you have to visit all nodes.

---

# 48. AVL vs BST

This comparison is very important for exams.

| Feature         | BST        | AVL          |
| --------------- | ---------- | ------------ |
| Binary tree     | Yes        | Yes          |
| Search property | Yes        | Yes          |
| Self-balancing  | ❌          | ✅            |
| Stores height   | Usually no | Yes          |
| Balance factor  | No         | Yes          |
| Rotations       | No         | Yes          |
| Average search  | O(log N)   | O(log N)     |
| Worst search    | O(N)       | O(log N)     |
| Worst insert    | O(N)       | O(log N)     |
| Worst delete    | O(N)       | O(log N)     |
| Implementation  | Easier     | More complex |

---

# 49. AVL vs Normal BST Example

Suppose we insert:

```text
10 20 30 40 50 60 70
```

### Normal BST

Can become:

```text
10
  \
   20
     \
      30
        \
         40
           \
            50
              \
               60
                 \
                  70
```

Height:

```text
O(N)
```

---

### AVL

AVL performs rotations automatically.

It might look approximately like:

```text
             40
           /    \
         20      60
        /  \    /  \
      10   30  50   70
```

Height:

```text
O(log N)
```

So searching remains efficient.

---

# 50. Expected AVL Coding Questions

For your preparation, I would divide them into three levels.

## 🟢 Level 1 — MUST KNOW

You should be able to write these from scratch:

### 1. Define AVL Node

```cpp
struct Node
```

### 2. Get height

```cpp
getHeight()
```

### 3. Calculate balance factor

```cpp
getBalance()
```

### 4. Update height

```cpp
updateHeight()
```

### 5. Right rotation

```cpp
rightRotate()
```

### 6. Left rotation

```cpp
leftRotate()
```

### 7. Insert

```cpp
insert()
```

### 8. Search

```cpp
search()
```

### 9. Inorder

```cpp
inorder()
```

### 10. Preorder

```cpp
preorder()
```

### 11. Postorder

```cpp
postorder()
```

### 12. Level order

```cpp
levelOrder()
```

### 13. Find minimum

### 14. Find maximum

### 15. Delete

**Especially all four rebalancing cases after deletion.**

---

# 51. 🟡 Level 2 — Very Important

### 16. Count nodes

### 17. Count leaves

### 18. Find kth smallest

### 19. Find kth largest

### 20. Find LCA

### 21. Find predecessor

### 22. Find successor

### 23. Find floor

### 24. Find ceil

### 25. Search values in range

### 26. Check if AVL is balanced

### 27. Update/Edit a value

### 28. Build AVL from an array

### 29. Delete all nodes

### 30. Print balance factor of every node

---

# 52. 🔴 Level 3 — Advanced

These are useful for stronger interviews:

### 31. Convert sorted array to AVL

### 32. Convert BST to AVL

### 33. Merge two AVL trees

### 34. Join two AVL trees

### 35. Split AVL tree

### 36. AVL tree iterator

### 37. Find kth smallest in O(log N)

### 38. Range queries using AVL

### 39. AVL with duplicate values

### 40. AVL augmented with subtree size

The last few are generally beyond a basic DSA lab but are good advanced topics.

---

# 53. Expected Exam/Theory Questions

You can also expect questions like:

### Q1. What is an AVL tree?

Answer:

> An AVL tree is a self-balancing Binary Search Tree in which the difference between the heights of the left and right subtrees of every node is at most 1.

---

### Q2. What is the balance factor?

```text
BF = height(left) - height(right)
```

Valid:

```text
-1, 0, +1
```

---

### Q3. What are the four AVL rotations?

```text
LL → Right Rotation

RR → Left Rotation

LR → Left Rotation + Right Rotation

RL → Right Rotation + Left Rotation
```

---

### Q4. Why do we need AVL trees?

Because a normal BST can become skewed:

```text
O(N)
```

while AVL maintains:

```text
O(log N)
```

height.

---

### Q5. What is the complexity of AVL search?

```text
O(log N)
```

---

### Q6. Why is AVL deletion more complicated than insertion?

Because after deletion, removing a node can cause imbalance at **multiple ancestors**, so the tree needs height updates and rebalancing while recursion returns toward the root.

---

# 54. Rotation Cheat Sheet ⭐

Keep this table in your notes:

```text
             AVL ROTATIONS

       Imbalance       Solution

          LL       →   Right Rotate

          RR       →   Left Rotate

          LR       →   Left Rotate child
                       +
                       Right Rotate node

          RL       →   Right Rotate child
                       +
                       Left Rotate node
```

Or remember:

```text
LL → R
RR → L
LR → L-R
RL → R-L
```

---

# 55. How to Identify Rotation in an Exam

Suppose you are given:

```text
30
20
10
```

Ask:

> Where did the new node go from the unbalanced node?

```text
30
 ↓
LEFT
 ↓
20
 ↓
LEFT
 ↓
10
```

Therefore:

```text
LL
```

Use:

```text
RIGHT ROTATION
```

---

Suppose:

```text
10
20
30
```

Path:

```text
RIGHT → RIGHT
```

Therefore:

```text
RR
```

Use:

```text
LEFT ROTATION
```

---

Suppose:

```text
30
10
20
```

Path:

```text
LEFT → RIGHT
```

Therefore:

```text
LR
```

Use:

```text
LEFT + RIGHT
```

---

Suppose:

```text
10
30
20
```

Path:

```text
RIGHT → LEFT
```

Therefore:

```text
RL
```

Use:

```text
RIGHT + LEFT
```

---

# 56. One More Very Important Point: Rotation Doesn't Break BST

Students often wonder:

> "If we move nodes around, won't the BST property be destroyed?"

No.

The rotations are specifically designed to preserve the ordering.

For example:

```text
        30
       /
      20
     /
    10
```

After right rotation:

```text
       20
      /  \
     10   30
```

Inorder before:

```text
10 20 30
```

Inorder after:

```text
10 20 30
```

Therefore BST ordering is preserved.

---

# 57. The Three Trees You Have Learned So Far

You have now reached an important point in your DSA preparation:

```text
                    TREE
                      |
             ┌────────┴────────┐
             ↓                 ↓
       Binary Tree             BST
             |                 |
      No ordering          Left < Root < Right
                               |
                               ↓
                              AVL
                               |
                        Self-balancing BST
                               |
                               ↓
                         Balance Factor
                               |
                               ↓
                           Rotations
```

The conceptual progression is:

```text
Binary Tree
   ↓
Learn recursion + traversal
   ↓
BST
   ↓
Learn ordering + efficient search
   ↓
AVL
   ↓
Learn balancing + rotations
```

---

# ⭐ 58. What You Should Be Able to Code Without Looking

For a coding round, I would make these your **AVL checklist**:

```text
☐ Node structure with height

☐ getHeight()

☐ getBalance()

☐ updateHeight()

☐ rightRotate()

☐ leftRotate()

☐ insert()

☐ deleteNode()

☐ search()

☐ inorder()

☐ preorder()

☐ postorder()

☐ levelOrder()

☐ minNode()

☐ maxNode()

☐ kthSmallest()

☐ kthLargest()

☐ LCA()

☐ predecessor()

☐ successor()

☐ floor()

☐ ceil()
```

And above all, you should be able to look at:

```text
LL
RR
LR
RL
```

and immediately know the required rotation.

### The core AVL pattern to remember:

```text
                 INSERT / DELETE
                       ↓
                  BST operation
                       ↓
                 update height
                       ↓
                 balance factor
                       ↓
            ┌──────────┴──────────┐
            ↓                     ↓
         BF > +1               BF < -1
            ↓                     ↓
       Left imbalance        Right imbalance
            ↓                     ↓
        LL / LR               RR / RL
            ↓                     ↓
        rotations              rotations
            ↓                     ↓
             └──────→ Balanced ←─┘
```

**One key distinction from your previous BST lesson:** in a BST you mainly worry about `left < root < right`; in an AVL tree you must worry about that **plus** the height/balance condition. That's why AVL insertion and deletion are more code-heavy, but they guarantee `O(log N)` search, insertion, and deletion.


# 🌳 Binary Search Tree (BST) — Complete C++ DSA Tutorial

Now we move from a **Binary Tree** to a **Binary Search Tree (BST)**.

BST is extremely important because unlike a normal binary tree, it has an **ordering property** that makes operations such as **search, insertion and deletion much more efficient**.

---

# 1. What is a BST?

A **Binary Search Tree** is a binary tree where every node follows this rule:

```text
             ROOT
            /    \
           /      \
      smaller    larger
```

For every node:

```text
All values in LEFT subtree < node
All values in RIGHT subtree > node
```

For example:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

Check node `50`:

```text
Left side: 20, 30, 40
Right side: 60, 70, 80
```

Therefore:

```text
20 < 50
30 < 50
40 < 50

60 > 50
70 > 50
80 > 50
```

This is a valid BST.

---

# 2. Why is BST Useful?

Suppose we have:

```text
50 30 70 20 40 60 80
```

If this were an ordinary binary tree, to search for `60`, we might need to check many nodes.

But in a BST:

```text
Search 60

60 < 50 ? NO
        ↓
      Go RIGHT

60 < 70 ? YES
        ↓
      Go LEFT

Found 60
```

We don't need to search everywhere.

This is the main advantage of BST.

---

# 3. BST Node Structure

Same as a binary tree:

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

A node looks like:

```text
       +---------+
       |  data   |
       +---------+
       |  left   |
       +---------+
       |  right  |
       +---------+
```

---

# 4. Creating a BST

The most important thing is understanding **insertion**.

Suppose we insert:

```text
50
30
70
20
40
60
80
```

Start:

```text
50
```

Insert `30`:

```text
30 < 50
```

so go left:

```text
    50
   /
  30
```

Insert `70`:

```text
70 > 50
```

so go right:

```text
    50
   /  \
 30    70
```

Insert `20`:

```text
20 < 50
20 < 30
```

so:

```text
    50
   /  \
 30    70
 /
20
```

Insert `40`:

```text
40 < 50
40 > 30
```

Therefore:

```text
    50
   /  \
 30    70
 / \
20 40
```

Continue with `60`, `80`:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

---

# 5. BST Insert — Most Important Function

Recursive implementation:

```cpp
Node* insert(Node* root, int value) {

    // Empty position found
    if (root == nullptr)
        return new Node(value);

    // Go left
    if (value < root->data)
        root->left = insert(root->left, value);

    // Go right
    else if (value > root->data)
        root->right = insert(root->right, value);

    // If equal, don't insert duplicate
    return root;
}
```

Use:

```cpp
Node* root = nullptr;

root = insert(root, 50);
root = insert(root, 30);
root = insert(root, 70);
root = insert(root, 20);
root = insert(root, 40);
root = insert(root, 60);
root = insert(root, 80);
```

---

# 6. Understand the Recursive Insert

This line:

```cpp
root->left = insert(root->left, value);
```

is extremely important.

Suppose:

```text
       50
      /
    30
```

We want to insert `20`.

At `50`:

```text
20 < 50
```

so:

```cpp
root->left = insert(root->left, 20);
```

Now `root->left` is node `30`.

At `30`:

```text
20 < 30
```

so:

```cpp
root->left = insert(root->left, 20);
```

Now `root->left` is `nullptr`.

So:

```cpp
return new Node(20);
```

Then the returned node gets connected to the tree.

---

# 7. Iterative BST Insertion

You should know this too.

```cpp
Node* insert(Node* root, int value) {

    Node* newNode = new Node(value);

    if (root == nullptr)
        return newNode;

    Node* current = root;
    Node* parent = nullptr;

    while (current != nullptr) {

        parent = current;

        if (value < current->data)
            current = current->left;

        else if (value > current->data)
            current = current->right;

        else {
            delete newNode;
            return root;
        }
    }

    if (value < parent->data)
        parent->left = newNode;
    else
        parent->right = newNode;

    return root;
}
```

For coding interviews, know **both recursive and iterative approaches**, although recursive is usually simpler.

---

# 8. Searching in BST

This is one of the biggest advantages of BST.

Suppose:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

Search for `60`.

```text
60 > 50
      ↓
    RIGHT

60 < 70
      ↓
     LEFT

Found
```

We don't visit `20`, `30`, `40`, `80`, etc.

---

# 9. Recursive Search

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

Usage:

```cpp
if (search(root, 60))
    cout << "Found";
else
    cout << "Not Found";
```

---

# 10. Iterative Search

Also very important:

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

This avoids recursion and uses `O(1)` auxiliary space.

---

# 11. Update/Edit a Node

This is slightly different from a normal binary tree.

Suppose:

```text
       50
      /  \
    30    70
```

You cannot blindly do:

```cpp
root->data = 100;
```

because the BST property may become invalid.

For example:

```text
       50
      /  \
    30    70
```

If you change `30` to `80`:

```text
       50
      /  \
    80    70
```

This is **not a BST**.

Therefore, the safest approach is:

```text
Delete old value
Insert new value
```

---

# 12. BST Update

```cpp
Node* update(Node* root, int oldValue, int newValue) {

    root = deleteNode(root, oldValue);

    root = insert(root, newValue);

    return root;
}
```

Usage:

```cpp
root = update(root, 40, 45);
```

The BST remains valid.

---

# 13. BST Traversals

All the binary-tree traversals still apply.

For:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

### Preorder

```text
50 30 20 40 70 60 80
```

### Inorder

```text
20 30 40 50 60 70 80
```

### Postorder

```text
20 40 30 60 80 70 50
```

### Level Order

```text
50 30 70 20 40 60 80
```

---

# ⭐ 14. MOST IMPORTANT BST PROPERTY

## Inorder traversal of a BST is sorted.

For:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

Inorder:

```text
20 30 40 50 60 70 80
```

Sorted!

This property is used in many interview questions.

Whenever you see:

> "Check whether a binary tree is a BST"

or

> "Find kth smallest element in BST"

think:

```text
INORDER
```

---

# 15. Minimum Value in BST

In a BST:

```text
SMALLEST VALUE
       ↓
Keep going LEFT
```

Example:

```text
             50
           /    \
         30      70
        /
      20
     /
    10
```

Minimum = `10`.

### Code

```cpp
int findMin(Node* root) {

    if (root == nullptr)
        return INT_MAX;

    while (root->left != nullptr)
        root = root->left;

    return root->data;
}
```

---

# 16. Maximum Value in BST

Opposite:

```text
LARGEST VALUE
       ↓
Keep going RIGHT
```

```cpp
int findMax(Node* root) {

    if (root == nullptr)
        return INT_MIN;

    while (root->right != nullptr)
        root = root->right;

    return root->data;
}
```

---

# 17. Find Minimum Node

Sometimes deletion needs the actual node instead of just its value.

```cpp
Node* minNode(Node* root) {

    Node* current = root;

    while (current != nullptr &&
           current->left != nullptr) {

        current = current->left;
    }

    return current;
}
```

---

# 18. BST Deletion — MOST IMPORTANT

Deletion is probably the most important BST operation to understand.

There are **three cases**.

---

## Case 1: Delete a Leaf Node

Example:

```text
       50
      /  \
    30    70
   / \
 20  40
```

Delete `20`.

`20` has no children.

Simply remove it:

```text
       50
      /  \
    30    70
      \
      40
```

---

# 19. Case 2: Delete Node with One Child

Example:

```text
       50
      /
    30
   /
 20
```

Delete `30`.

`30` has one child:

```text
20
```

So `20` takes the place of `30`:

```text
       50
      /
     20
```

---

# 20. Case 3: Delete Node with Two Children

This is the important one.

Consider:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

Delete `50`.

`50` has two children:

```text
30
70
```

What should replace `50`?

We can use:

### Option 1

**Inorder successor**

The smallest value in the right subtree.

Right subtree:

```text
       70
      /  \
     60   80
```

Smallest = `60`.

So:

```text
50 → 60
```

Then delete the original `60`.

Result:

```text
             60
           /    \
         30      70
        /  \      \
      20   40      80
```

---

# 21. What is Inorder Successor?

For a node:

> Inorder successor = smallest value greater than that node.

For:

```text
20 30 40 50 60 70 80
```

Successor of `50`:

```text
60
```

Successor of `60`:

```text
70
```

Successor of `70`:

```text
80
```

---

# 22. BST Delete Code

This is a code you should memorize/understand extremely well.

```cpp
Node* deleteNode(Node* root, int key) {

    // Value not found
    if (root == nullptr)
        return nullptr;

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

        // Case 1: No left child
        if (root->left == nullptr) {

            Node* temp = root->right;

            delete root;

            return temp;
        }

        // Case 2: No right child
        if (root->right == nullptr) {

            Node* temp = root->left;

            delete root;

            return temp;
        }

        // Case 3: Two children

        Node* successor =
            minNode(root->right);

        root->data = successor->data;

        root->right =
            deleteNode(root->right,
                       successor->data);
    }

    return root;
}
```

---

# 23. Understand the Delete Code

Suppose we have:

```text
       50
      /  \
    30    70
```

Delete `30`.

At `50`:

```text
30 < 50
```

so:

```cpp
root->left = deleteNode(root->left, 30);
```

Now we reach `30`.

Suppose `30` has no right child.

This executes:

```cpp
Node* temp = root->left;
delete root;
return temp;
```

The parent reconnects itself to the returned child.

This is the key idea behind recursive BST deletion.

---

# 24. Find Height of BST

Same as binary tree:

```cpp
int height(Node* root) {

    if (root == nullptr)
        return 0;

    return 1 + max(
        height(root->left),
        height(root->right)
    );
}
```

---

# 25. Count Nodes

```cpp
int countNodes(Node* root) {

    if (root == nullptr)
        return 0;

    return 1 +
           countNodes(root->left) +
           countNodes(root->right);
}
```

---

# 26. Count Leaf Nodes

```cpp
int countLeaves(Node* root) {

    if (root == nullptr)
        return 0;

    if (root->left == nullptr &&
        root->right == nullptr)
        return 1;

    return countLeaves(root->left) +
           countLeaves(root->right);
}
```

---

# 27. Find Kth Smallest Element

This is a **very common BST question**.

Example:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

Sorted order:

```text
20 30 40 50 60 70 80
```

Therefore:

```text
1st smallest = 20
2nd smallest = 30
3rd smallest = 40
4th smallest = 50
```

Since inorder gives sorted order, perform inorder traversal.

### Code

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

Usage:

```cpp
int k = 3;
int answer = -1;

kthSmallest(root, k, answer);

cout << answer;
```

Output:

```text
40
```

---

# 28. Kth Largest Element

Same idea but reverse inorder:

```text
RIGHT → ROOT → LEFT
```

```cpp
void kthLargest(Node* root,
                int& k,
                int& answer) {

    if (root == nullptr)
        return;

    kthLargest(root->right, k, answer);

    k--;

    if (k == 0) {
        answer = root->data;
        return;
    }

    kthLargest(root->left, k, answer);
}
```

---

# 29. Validate Whether a Tree is BST

Very important interview question.

Consider:

```text
       50
      /  \
    30    70
```

Valid.

But:

```text
       50
      /  \
    30    70
         /
        40
```

This is invalid.

Why?

Because `40` is in the right subtree of `50`, but:

```text
40 < 50
```

Therefore it violates the BST property.

---

# 30. Method 1 — Inorder Check

Perform inorder traversal.

For a valid BST, values must be strictly increasing.

```cpp
bool isBST(Node* root, long long& prev) {

    if (root == nullptr)
        return true;

    if (!isBST(root->left, prev))
        return false;

    if (root->data <= prev)
        return false;

    prev = root->data;

    return isBST(root->right, prev);
}
```

Usage:

```cpp
long long prev = LLONG_MIN;

if (isBST(root, prev))
    cout << "Valid BST";
else
    cout << "Not a BST";
```

---

# 31. Method 2 — Min/Max Range

Another very important approach.

Every node must be within a valid range.

```cpp
bool isBST(Node* root,
           long long minValue,
           long long maxValue) {

    if (root == nullptr)
        return true;

    if (root->data <= minValue ||
        root->data >= maxValue)
        return false;

    return isBST(root->left,
                 minValue,
                 root->data)
        &&
           isBST(root->right,
                 root->data,
                 maxValue);
}
```

Call:

```cpp
isBST(root, LLONG_MIN, LLONG_MAX);
```

This is a very useful pattern.

---

# 32. Find LCA in BST

LCA = Lowest Common Ancestor.

This is easier in a BST than in a normal binary tree.

Example:

```text
             50
           /    \
         30      70
        /  \    /  \
      20   40  60   80
```

LCA of:

```text
20 and 40
```

is:

```text
30
```

LCA of:

```text
20 and 60
```

is:

```text
50
```

---

# 33. LCA Logic

At node `50`:

If both values are smaller:

```text
Go LEFT
```

If both are larger:

```text
Go RIGHT
```

Otherwise:

```text
Current node is LCA
```

### Code

```cpp
Node* LCA(Node* root, int a, int b) {

    if (root == nullptr)
        return nullptr;

    if (a < root->data &&
        b < root->data) {

        return LCA(root->left, a, b);
    }

    if (a > root->data &&
        b > root->data) {

        return LCA(root->right, a, b);
    }

    return root;
}
```

---

# 34. LCA Iterative Version

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

This uses `O(1)` extra space.

---

# 35. Find Floor in BST

Floor of `x` means:

> Largest value ≤ x.

Example:

```text
20 30 40 50 60 70 80
```

For:

```text
x = 55
```

Floor:

```text
50
```

### Code

```cpp
int floorBST(Node* root, int x) {

    int answer = -1;

    while (root != nullptr) {

        if (root->data == x)
            return root->data;

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

# 36. Find Ceil in BST

Ceil means:

> Smallest value ≥ x.

For:

```text
20 30 40 50 60 70 80
```

and:

```text
x = 55
```

Ceil:

```text
60
```

Code:

```cpp
int ceilBST(Node* root, int x) {

    int answer = -1;

    while (root != nullptr) {

        if (root->data == x)
            return root->data;

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

# 37. Find Predecessor

Inorder predecessor:

> Largest value smaller than the current value.

For:

```text
20 30 40 50 60 70 80
```

Predecessor of `50`:

```text
40
```

---

# 38. Find Successor

Inorder successor:

> Smallest value greater than the current value.

Successor of `50`:

```text
60
```

A simple successor search:

```cpp
Node* successor(Node* root, int key) {

    Node* answer = nullptr;

    while (root != nullptr) {

        if (key < root->data) {

            answer = root;
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

# 39. Range Search

Question:

> Print all BST values between `L` and `R`.

Example:

```text
BST:
20 30 40 50 60 70 80

L = 35
R = 65
```

Output:

```text
40 50 60
```

Because BST lets us skip unnecessary branches.

```cpp
void rangeSearch(Node* root,
                 int L,
                 int R) {

    if (root == nullptr)
        return;

    if (root->data > L)
        rangeSearch(root->left, L, R);

    if (root->data >= L &&
        root->data <= R)
        cout << root->data << " ";

    if (root->data < R)
        rangeSearch(root->right, L, R);
}
```

---

# 40. Convert Sorted Array to Balanced BST

Very common.

Given:

```text
1 2 3 4 5 6 7
```

We want:

```text
        4
       / \
      2   6
     / \ / \
    1  3 5  7
```

Take the middle element as root.

Then recursively build left and right.

```cpp
Node* sortedArrayToBST(vector<int>& arr,
                       int left,
                       int right) {

    if (left > right)
        return nullptr;

    int mid = left + (right - left) / 2;

    Node* root = new Node(arr[mid]);

    root->left =
        sortedArrayToBST(arr,
                         left,
                         mid - 1);

    root->right =
        sortedArrayToBST(arr,
                         mid + 1,
                         right);

    return root;
}
```

Usage:

```cpp
vector<int> arr = {1,2,3,4,5,6,7};

Node* root =
    sortedArrayToBST(arr, 0, arr.size()-1);
```

---

# 41. Delete Entire BST

Same postorder idea:

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

---

# 42. Print Values in a Range

Already covered, but this is an important coding-round pattern.

Remember:

```text
BST + range
       ↓
Use BST property to prune branches
```

Don't blindly traverse the whole tree if you can avoid it.

---

# 43. Find Closest Value to K

Example:

```text
BST:
10 20 30 40 50 60 70

K = 46
```

Closest:

```text
50
```

Code:

```cpp
int closestValue(Node* root, int k) {

    int answer = root->data;

    while (root != nullptr) {

        if (abs(root->data - k) <
            abs(answer - k)) {

            answer = root->data;
        }

        if (root->data < k)
            root = root->right;
        else if (root->data > k)
            root = root->left;
        else
            return root->data;
    }

    return answer;
}
```

---

# 44. Two Sum in BST

Question:

> Does there exist a pair of nodes whose sum is `K`?

Example:

```text
BST:
        5
       / \
      3   7
     / \ / \
    2  4 6  8
```

For:

```text
K = 9
```

Possible pair:

```text
2 + 7 = 9
```

One simple solution:

```cpp
bool twoSumBST(Node* root,
               int k,
               unordered_set<int>& seen) {

    if (root == nullptr)
        return false;

    if (seen.count(k - root->data))
        return true;

    seen.insert(root->data);

    return twoSumBST(root->left, k, seen) ||
           twoSumBST(root->right, k, seen);
}
```

Usage:

```cpp
unordered_set<int> seen;

cout << twoSumBST(root, 9, seen);
```

---

# 45. Convert BST to Sorted Vector

Because inorder is sorted:

```cpp
void inorderToVector(Node* root,
                     vector<int>& result) {

    if (root == nullptr)
        return;

    inorderToVector(root->left, result);

    result.push_back(root->data);

    inorderToVector(root->right, result);
}
```

Usage:

```cpp
vector<int> result;

inorderToVector(root, result);
```

---

# 46. Build BST from an Array

Suppose input:

```text
50 30 70 20 40 60 80
```

We simply insert each value:

```cpp
Node* buildBST(vector<int>& arr) {

    Node* root = nullptr;

    for (int value : arr) {
        root = insert(root, value);
    }

    return root;
}
```

Usage:

```cpp
vector<int> arr = {
    50, 30, 70, 20, 40, 60, 80
};

Node* root = buildBST(arr);
```

---

# 47. Complete BST Program

This is a good program to practice until you can write it without looking.

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


// INSERT
Node* insert(Node* root, int value) {

    if (root == nullptr)
        return new Node(value);

    if (value < root->data)
        root->left = insert(root->left, value);

    else if (value > root->data)
        root->right = insert(root->right, value);

    return root;
}


// SEARCH
bool search(Node* root, int key) {

    if (root == nullptr)
        return false;

    if (root->data == key)
        return true;

    if (key < root->data)
        return search(root->left, key);

    return search(root->right, key);
}


// INORDER
void inorder(Node* root) {

    if (root == nullptr)
        return;

    inorder(root->left);

    cout << root->data << " ";

    inorder(root->right);
}


// PREORDER
void preorder(Node* root) {

    if (root == nullptr)
        return;

    cout << root->data << " ";

    preorder(root->left);
    preorder(root->right);
}


// POSTORDER
void postorder(Node* root) {

    if (root == nullptr)
        return;

    postorder(root->left);
    postorder(root->right);

    cout << root->data << " ";
}


// MINIMUM
Node* minNode(Node* root) {

    while (root != nullptr &&
           root->left != nullptr) {

        root = root->left;
    }

    return root;
}


// MAXIMUM
Node* maxNode(Node* root) {

    while (root != nullptr &&
           root->right != nullptr) {

        root = root->right;
    }

    return root;
}


// DELETE
Node* deleteNode(Node* root, int key) {

    if (root == nullptr)
        return nullptr;

    if (key < root->data) {

        root->left =
            deleteNode(root->left, key);
    }

    else if (key > root->data) {

        root->right =
            deleteNode(root->right, key);
    }

    else {

        // No left child
        if (root->left == nullptr) {

            Node* temp = root->right;

            delete root;

            return temp;
        }

        // No right child
        if (root->right == nullptr) {

            Node* temp = root->left;

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

    return root;
}


// HEIGHT
int height(Node* root) {

    if (root == nullptr)
        return 0;

    return 1 + max(
        height(root->left),
        height(root->right)
    );
}


// COUNT NODES
int countNodes(Node* root) {

    if (root == nullptr)
        return 0;

    return 1 +
           countNodes(root->left) +
           countNodes(root->right);
}


// COUNT LEAVES
int countLeaves(Node* root) {

    if (root == nullptr)
        return 0;

    if (root->left == nullptr &&
        root->right == nullptr)
        return 1;

    return countLeaves(root->left) +
           countLeaves(root->right);
}


int main() {

    Node* root = nullptr;

    vector<int> values = {
        50, 30, 70, 20, 40, 60, 80
    };

    // Create BST
    for (int value : values)
        root = insert(root, value);


    cout << "Inorder: ";
    inorder(root);

    cout << "\nPreorder: ";
    preorder(root);

    cout << "\nPostorder: ";
    postorder(root);


    cout << "\n\nSearch 60: ";

    if (search(root, 60))
        cout << "Found";
    else
        cout << "Not Found";


    cout << "\nMinimum: ";
    cout << minNode(root)->data;


    cout << "\nMaximum: ";
    cout << maxNode(root)->data;


    cout << "\nHeight: ";
    cout << height(root);


    cout << "\nNumber of nodes: ";
    cout << countNodes(root);


    cout << "\nLeaf nodes: ";
    cout << countLeaves(root);


    // Delete
    root = deleteNode(root, 30);

    cout << "\n\nAfter deleting 30: ";

    inorder(root);

    return 0;
}
```

---

# 48. BST Complexity

This is extremely important for interviews.

Let:

```text
N = number of nodes
H = height of tree
```

Search, insertion and deletion actually take:

```text
O(H)
```

Why?

Because we travel from the root down one path.

### Balanced BST

If tree is balanced:

```text
H ≈ log N
```

Therefore:

```text
Search      O(log N)
Insert      O(log N)
Delete      O(log N)
```

### Worst case

If values are inserted in sorted order:

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

This is basically a linked list.

Then:

```text
H = N
```

So:

```text
Search      O(N)
Insert      O(N)
Delete      O(N)
```

---

# 49. Why Sorted Input Can Be Dangerous

Suppose:

```cpp
vector<int> arr = {
    10,20,30,40,50,60,70
};
```

Insert sequentially.

You get:

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

This is a terrible BST.

Instead, if we insert approximately middle values first:

```text
40
20 60
10 30 50 70
```

we get a much more balanced tree.

This is why **self-balancing BSTs** such as AVL trees and Red-Black trees exist.

---

# 50. Expected BST Questions for Your Coding Round

I would prioritize them like this.

## 🟢 Level 1 — Must Know

You should be able to code these without help:

### 1. Create a BST

### 2. Insert a node

### 3. Search a value

### 4. Inorder traversal

### 5. Preorder traversal

### 6. Postorder traversal

### 7. Level order traversal

### 8. Find minimum

### 9. Find maximum

### 10. Find height

### 11. Count nodes

### 12. Count leaf nodes

### 13. Delete a node

**Especially understand the 3 deletion cases.**

---

# 51. 🟡 Level 2 — Very Important

### 14. Check whether a binary tree is a BST

### 15. Find kth smallest

### 16. Find kth largest

### 17. Find LCA in BST

### 18. Find inorder successor

### 19. Find inorder predecessor

### 20. Find floor

### 21. Find ceil

### 22. Search values in a range

### 23. Find closest value to K

### 24. Convert BST to sorted array

### 25. Find two nodes whose sum is K

### 26. Find number of nodes in a range

### 27. Find minimum absolute difference between two BST nodes

---

# 52. 🔴 Level 3 — Advanced Interview Questions

After you're comfortable with the above:

### 28. Convert sorted array to balanced BST

### 29. Convert BST to balanced BST

### 30. Delete all nodes outside a range

### 31. Merge two BSTs

### 32. Construct BST from preorder

### 33. Construct BST from postorder

### 34. Recover a corrupted BST

### 35. Serialize and deserialize BST

### 36. BST iterator

### 37. Two-sum using two BST iterators

### 38. Largest BST in a binary tree

### 39. Maximum sum BST in a binary tree

### 40. Convert BST to greater sum tree

---

# 53. Most Important BST Patterns

Don't memorize 40 separate solutions. Learn these patterns.

## Pattern 1 — Searching

```cpp
if (key < root->data)
    root = root->left;
else
    root = root->right;
```

The BST property lets you throw away half the possible directions.

---

## Pattern 2 — Inorder

Whenever you need:

```text
sorted order
kth smallest
kth largest
predecessor
successor
check BST
```

think:

```text
INORDER
```

---

## Pattern 3 — Minimum

```text
Go LEFT repeatedly
```

---

## Pattern 4 — Maximum

```text
Go RIGHT repeatedly
```

---

## Pattern 5 — LCA

At current node:

```text
Both smaller → LEFT

Both larger → RIGHT

Otherwise → CURRENT NODE
```

---

## Pattern 6 — Delete

Always remember:

```text
             Delete
                |
        ┌───────┼────────┐
        ↓       ↓        ↓
      0 child  1 child  2 children
        |       |         |
      remove   replace   successor/
                         predecessor
```

For two children, the easiest approach to remember is:

```text
SUCCESSOR =
minimum of RIGHT subtree
```

Then:

```text
copy successor value
↓
delete successor
```

---

# 54. Binary Tree vs BST — Final Comparison

| Feature          | Binary Tree       | BST         |
| ---------------- | ----------------- | ----------- |
| Maximum children | 2                 | 2           |
| Ordering         | ❌ No              | ✅ Yes       |
| Search           | O(N)              | O(H)        |
| Insert           | Depends on method | O(H)        |
| Delete           | Depends on method | O(H)        |
| Minimum          | O(N)              | O(H)        |
| Maximum          | O(N)              | O(H)        |
| Inorder sorted?  | ❌ No              | ✅ Yes       |
| Recursion        | Very common       | Very common |

And:

```text
Balanced BST:
H = O(log N)

Worst-case BST:
H = O(N)
```

---

# ⭐ What You Should Memorize for the Coding Round

If you have limited time, make sure these **10 things** are perfect:

```text
1. Node structure
2. Insert
3. Search
4. Inorder
5. Preorder
6. Postorder
7. Find minimum/maximum
8. Delete — all 3 cases
9. Validate BST
10. Kth smallest
```

Then learn:

```text
11. LCA
12. Floor/Ceil
13. Successor/Predecessor
14. Range search
15. Sorted array → balanced BST
16. Two Sum in BST
17. Closest value
18. Merge BSTs
```

The **single most important conceptual difference** from the Binary Tree you just learned is this:

```text
Binary Tree
    ↓
"No particular ordering"
    ↓
Usually need to explore both sides


BST
    ↓
LEFT < ROOT < RIGHT
    ↓
Can decide LEFT or RIGHT
    ↓
Search / Insert / Delete become O(H)
```

And remember this golden rule:

> **If you see a BST and the problem involves sorted order, kth smallest/largest, predecessor/successor, or validation, think INORDER first.**


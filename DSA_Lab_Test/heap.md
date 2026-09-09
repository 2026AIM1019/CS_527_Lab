Absolutely. **Heap is a very important topic for DSA and competitive programming**, especially because it leads directly to **Priority Queue, Heap Sort, K-th largest/smallest, Top-K problems, Median problems, Dijkstra/Prim concepts, scheduling**, etc.

I’ll teach it in the same sequence as AVL: **concept → representation → create → insert → delete → update → heapify → build heap → heap sort → STL priority_queue → important problems → coding-round questions → complexity → cheat sheet.**

# 1. What is a Heap?

A **Heap** is a special type of **Complete Binary Tree**.

There are two main types:

### Max Heap

The parent is always **greater than or equal to** its children.

```text
             50
           /    \
         30      40
        /  \    /  \
      10   20  35   25
```

For every node:

```text
parent >= children
```

Therefore, the **largest element is always at the root**.

---

### Min Heap

The parent is always **less than or equal to** its children.

```text
             10
           /    \
         20      15
        /  \    /  \
      30   25  40   35
```

For every node:

```text
parent <= children
```

Therefore, the **smallest element is always at the root**.

---

# 2. Heap vs Binary Search Tree

This is extremely important.

| Property                  | BST                               | Heap                 |
| ------------------------- | --------------------------------- | -------------------- |
| Type                      | Binary tree                       | Complete binary tree |
| Left < root               | Yes                               | No                   |
| Right > root              | Yes                               | No                   |
| Complete tree             | Not necessarily                   | Yes                  |
| Root                      | Not necessarily min/max           | Always min/max       |
| Searching arbitrary value | Efficient                         | Not efficient        |
| Insert                    | O(log N) average                  | O(log N)             |
| Delete                    | O(log N) average                  | O(log N)             |
| Find minimum              | O(log N) or O(1) depending on BST | O(1) in Min Heap     |
| Find maximum              | O(log N) or O(1) depending on BST | O(1) in Max Heap     |

### Very important:

A heap is **NOT a BST**.

For example, this is a valid Max Heap:

```text
          50
        /    \
      20      40
     /  \    /  \
   10   15  35   30
```

Notice:

```text
40 > 20
```

but that's completely fine.

There is no requirement that:

```text
left < root < right
```

Only the **parent-child relationship** matters.

---

# 3. Why is Heap a Complete Binary Tree?

A Complete Binary Tree means:

* Every level is completely filled except possibly the last.
* The last level is filled from **left to right**.

Example:

```text
             50
           /    \
         30      40
        /  \    /
      10   20  35
```

This is complete.

But:

```text
             50
           /    \
         30      40
           \    /
           20  35
```

This is NOT complete.

---

# 4. Most Important Thing: Heap Using Array

Unlike a normal binary tree, we usually don't need pointers for a heap.

We store it in an array/vector.

For example:

```text
             50
           /    \
         30      40
        /  \    /  \
      10   20  35   25
```

Array:

```text
[50, 30, 40, 10, 20, 35, 25]
```

This is one of the biggest reasons heaps are useful.

---

# 5. Parent and Child Formulas

Assume **0-based indexing**.

For a node at index `i`:

### Parent

```cpp
parent = (i - 1) / 2;
```

### Left child

```cpp
left = 2 * i + 1;
```

### Right child

```cpp
right = 2 * i + 2;
```

For example:

```text
Array:

Index:  0   1   2   3   4   5   6
Value: 50  30  40  10  20  35  25
```

For index `1`:

```text
value = 30
```

Left child:

```text
2*1 + 1 = 3
```

Value:

```text
10
```

Right child:

```text
2*1 + 2 = 4
```

Value:

```text
20
```

Parent:

```text
(1-1)/2 = 0
```

Value:

```text
50
```

---

# 6. Creating a Heap

There are two ways you should understand.

## Method 1: Insert elements one by one

Suppose we want a Max Heap:

```text
10 20 30 40 50
```

We insert one element at a time.

Start:

```text
10
```

Insert 20:

```text
  10
 /
20
```

This violates Max Heap.

So we move 20 upward:

```text
  20
 /
10
```

Insert 30:

```text
    20
   /  \
 10   30
```

Again violation.

Move 30 upward:

```text
    30
   /  \
 10   20
```

This process is called:

# Heapify Up

or

# Sift Up

or

# Bubble Up

---

# 7. Insertion in Max Heap

Suppose:

```text
heap = [50, 30, 40, 10, 20]
```

Insert:

```text
60
```

First put it at the end:

```text
[50, 30, 40, 10, 20, 60]
```

Tree:

```text
          50
        /    \
      30      40
     /  \    /
   10   20  60
```

60 is greater than parent 40.

Swap:

```text
          50
        /    \
      30      60
     /  \    /
   10   20  40
```

Now 60 > 50.

Swap again:

```text
          60
        /    \
      30      50
     /  \    /
   10   20  40
```

Heap restored.

---

# 8. Max Heap Insert Code

```cpp
void insertMaxHeap(vector<int>& heap, int value) {

    // Step 1: Add at the end
    heap.push_back(value);

    int i = heap.size() - 1;

    // Step 2: Move upward
    while (i > 0) {

        int parent = (i - 1) / 2;

        // Parent already bigger
        if (heap[parent] >= heap[i])
            break;

        // Swap
        swap(heap[parent], heap[i]);

        // Move to parent
        i = parent;
    }
}
```

### Complexity

```text
O(log N)
```

because the element can travel from leaf to root.

---

# 9. Min Heap Insert

Exactly the same idea.

But now:

```text
child < parent
```

means violation.

```cpp
void insertMinHeap(vector<int>& heap, int value) {

    heap.push_back(value);

    int i = heap.size() - 1;

    while (i > 0) {

        int parent = (i - 1) / 2;

        if (heap[parent] <= heap[i])
            break;

        swap(heap[parent], heap[i]);

        i = parent;
    }
}
```

---

# 10. Getting the Maximum/Minimum

This is one of the biggest advantages of a heap.

### Max Heap

Maximum is always:

```cpp
heap[0]
```

### Min Heap

Minimum is always:

```cpp
heap[0]
```

Therefore:

```text
Get maximum → O(1)
Get minimum → O(1)
```

depending on heap type.

---

# 11. Delete Root from Heap

This is extremely important.

Suppose Max Heap:

```text
          50
        /    \
      30      40
     /  \    /  \
   10   20  35   25
```

Array:

```text
[50,30,40,10,20,35,25]
```

We want to delete `50`.

### Step 1

Remove root.

### Step 2

Take the last element:

```text
25
```

and put it at root.

```text
          25
        /    \
      30      40
     /  \    /
   10   20  35
```

Now heap property is broken.

### Step 3

Move 25 downward.

Compare children:

```text
30 and 40
```

Take the **larger child**:

```text
40
```

Swap:

```text
          40
        /    \
      30      25
     /  \    /
   10   20  35
```

Now compare 25 with its child 35.

35 is larger.

Swap:

```text
          40
        /    \
      30      35
     /  \    /
   10   20  25
```

Heap restored.

This process is called:

# Heapify Down

or

# Sift Down

---

# 12. Heapify Down in Max Heap

```cpp
void heapifyDown(vector<int>& heap, int i) {

    int n = heap.size();

    while (true) {

        int largest = i;

        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && heap[left] > heap[largest])
            largest = left;

        if (right < n && heap[right] > heap[largest])
            largest = right;

        if (largest == i)
            break;

        swap(heap[i], heap[largest]);

        i = largest;
    }
}
```

---

# 13. Delete Root Code

```cpp
void deleteMax(vector<int>& heap) {

    if (heap.empty())
        return;

    // If only one element
    if (heap.size() == 1) {
        heap.pop_back();
        return;
    }

    // Move last element to root
    heap[0] = heap.back();

    // Remove last element
    heap.pop_back();

    // Restore heap
    heapifyDown(heap, 0);
}
```

Complexity:

```text
O(log N)
```

---

# 14. Delete Any Element

This is slightly more complicated.

Suppose:

```text
[50, 30, 40, 10, 20, 35, 25]
```

We want to delete:

```text
20
```

Find its index.

Then:

1. Replace it with last element.
2. Remove last element.
3. Decide whether to heapify up or down.

Why both?

Because the replacement element may violate the heap property with either:

```text
parent
```

or

```text
children
```

---

## Code

```cpp
void deleteAtIndex(vector<int>& heap, int index) {

    int n = heap.size();

    if (index < 0 || index >= n)
        return;

    // Replace with last element
    heap[index] = heap[n - 1];

    heap.pop_back();

    if (index == 0) {
        heapifyDown(heap, index);
        return;
    }

    int parent = (index - 1) / 2;

    // Need to move upward
    if (heap[index] > heap[parent]) {

        while (index > 0) {

            parent = (index - 1) / 2;

            if (heap[parent] >= heap[index])
                break;

            swap(heap[parent], heap[index]);

            index = parent;
        }
    }
    else {

        // Need to move downward
        heapifyDown(heap, index);
    }
}
```

For a Min Heap, reverse the comparisons.

---

# 15. Updating an Element

Suppose:

```text
Max Heap:
[50, 30, 40, 10, 20, 35, 25]
```

Change:

```text
20 → 60
```

Now:

```text
[50,30,40,10,60,35,25]
```

60 is greater than its parent 30.

Therefore move upward.

```text
          60
        /    \
      50      40
     /  \    / \
   10   30  35 25
```

This is **heapify up**.

---

If:

```text
20 → 5
```

then it may need to move downward.

So updating a heap generally means:

```text
change value
      ↓
compare with parent/children
      ↓
heapify up OR heapify down
```

---

# 16. Build Heap

Suppose we have an ordinary array:

```text
[10, 20, 15, 30, 40, 50]
```

It is not necessarily a heap.

We want to convert it into a Max Heap.

This operation is called:

# Build Heap

The important trick:

**Start from the last non-leaf node.**

For `n` elements:

```cpp
lastNonLeaf = n/2 - 1;
```

Then heapify down each node toward index 0.

---

# 17. Why Start From `n/2 - 1`?

For:

```text
[10,20,15,30,40,50]
```

indexes:

```text
0 1 2 3 4 5
```

Tree:

```text
             10
           /    \
         20      15
        /  \    /
      30   40  50
```

Nodes:

```text
30
40
50
```

are leaves.

Leaves are already valid heaps.

So we start with:

```text
index = n/2 - 1
```

For `n = 6`:

```text
6/2 - 1 = 2
```

So start at index 2.

---

# 18. Build Max Heap Code

```cpp
void buildMaxHeap(vector<int>& arr) {

    int n = arr.size();

    for (int i = n / 2 - 1; i >= 0; i--) {

        int current = i;

        while (true) {

            int largest = current;

            int left = 2 * current + 1;
            int right = 2 * current + 2;

            if (left < n &&
                arr[left] > arr[largest])
                largest = left;

            if (right < n &&
                arr[right] > arr[largest])
                largest = right;

            if (largest == current)
                break;

            swap(arr[current], arr[largest]);

            current = largest;
        }
    }
}
```

There is also a cleaner recursive version:

```cpp
void heapify(vector<int>& arr, int n, int i) {

    int largest = i;

    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n &&
        arr[left] > arr[largest])
        largest = left;

    if (right < n &&
        arr[right] > arr[largest])
        largest = right;

    if (largest != i) {

        swap(arr[i], arr[largest]);

        heapify(arr, n, largest);
    }
}
```

Then:

```cpp
void buildMaxHeap(vector<int>& arr) {

    int n = arr.size();

    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
}
```

---

# 19. Complexity of Build Heap

This is a very common interview question.

You might initially think:

```text
N nodes × log N
= O(N log N)
```

But actual complexity is:

# O(N)

This is an important result.

```text
Build Heap = O(N)
```

Remember this for exams/interviews.

---

# 20. Heap Sort

Heap Sort is one of the most important heap applications.

Suppose:

```text
[5, 3, 8, 4, 1, 2]
```

We want ascending order.

### Step 1

Build a Max Heap.

```text
[8,4,5,3,1,2]
```

Conceptually:

```text
          8
        /   \
       4     5
      / \   /
     3   1 2
```

### Step 2

Largest element is at root:

```text
8
```

Put it at the end.

```text
[2,4,5,3,1,8]
```

Heapify remaining portion.

```text
[5,4,2,3,1,8]
```

Again largest:

```text
5
```

Put it at its final position.

```text
[1,4,2,3,5,8]
```

Continue.

Finally:

```text
[1,2,3,4,5,8]
```

---

# 21. Heap Sort Code

```cpp
void heapify(vector<int>& arr, int n, int i) {

    int largest = i;

    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n &&
        arr[left] > arr[largest]) {
        largest = left;
    }

    if (right < n &&
        arr[right] > arr[largest]) {
        largest = right;
    }

    if (largest != i) {

        swap(arr[i], arr[largest]);

        heapify(arr, n, largest);
    }
}

void heapSort(vector<int>& arr) {

    int n = arr.size();

    // Step 1: Build Max Heap
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // Step 2: Extract maximum
    for (int i = n - 1; i > 0; i--) {

        swap(arr[0], arr[i]);

        heapify(arr, i, 0);
    }
}
```

Use:

```cpp
int main() {

    vector<int> arr = {5, 3, 8, 4, 1, 2};

    heapSort(arr);

    for (int x : arr)
        cout << x << " ";

    return 0;
}
```

Output:

```text
1 2 3 4 5 8
```

---

# 22. Heap Sort Complexity

Build heap:

```text
O(N)
```

Extraction:

```text
N × O(log N)
```

Therefore:

```text
O(N log N)
```

Overall:

```text
Time = O(N log N)
Space = O(1)
```

Heap Sort is:

* In-place → Yes
* Stable → No
* Worst-case → O(N log N)
* Average → O(N log N)
* Best → O(N log N)

---

# 23. Max Heap to Ascending Order

Remember this simple rule:

```text
Max Heap
   ↓
Heap Sort
   ↓
Ascending order
```

For descending order:

```text
Min Heap
   ↓
Heap Sort
   ↓
Descending order
```

---

# 24. Priority Queue

Now we reach one of the **most important practical uses of heaps in C++**.

C++ STL provides:

```cpp
priority_queue
```

Internally it is implemented using a heap-like structure.

---

# 25. Max Priority Queue

By default:

```cpp
priority_queue<int> pq;
```

is a **Max Heap**.

Example:

```cpp
priority_queue<int> pq;

pq.push(10);
pq.push(50);
pq.push(20);
pq.push(40);
```

The top is:

```cpp
cout << pq.top();
```

Output:

```text
50
```

---

# 26. Important Priority Queue Functions

```cpp
pq.push(x);
```

Insert.

```cpp
pq.top();
```

Get highest-priority element.

```cpp
pq.pop();
```

Remove highest-priority element.

```cpp
pq.empty();
```

Check empty.

```cpp
pq.size();
```

Number of elements.

Example:

```cpp
priority_queue<int> pq;

pq.push(30);
pq.push(10);
pq.push(50);
pq.push(20);

while (!pq.empty()) {

    cout << pq.top() << " ";

    pq.pop();
}
```

Output:

```text
50 30 20 10
```

---

# 27. Min Priority Queue

By default:

```cpp
priority_queue<int>
```

is Max Heap.

For Min Heap:

```cpp
priority_queue<int, vector<int>, greater<int>> pq;
```

Example:

```cpp
priority_queue<int, vector<int>, greater<int>> pq;

pq.push(30);
pq.push(10);
pq.push(50);
pq.push(20);

while (!pq.empty()) {

    cout << pq.top() << " ";

    pq.pop();
}
```

Output:

```text
10 20 30 50
```

### Remember:

```cpp
priority_queue<int>
```

→ Max Heap

```cpp
priority_queue<int, vector<int>, greater<int>>
```

→ Min Heap

---

# 28. Heap Creation Using STL

Usually, in competitive programming you don't manually implement a heap unless the problem asks you to.

You can simply use:

```cpp
priority_queue<int> maxHeap;
```

or:

```cpp
priority_queue<int, vector<int>, greater<int>> minHeap;
```

---

# 29. Heap from an Existing Vector

Suppose:

```cpp
vector<int> arr = {5, 2, 8, 1, 9};
```

You can create:

```cpp
priority_queue<int> pq(arr.begin(), arr.end());
```

Now:

```cpp
cout << pq.top();
```

gives:

```text
9
```

For Min Heap:

```cpp
priority_queue<int, vector<int>, greater<int>>
    pq(arr.begin(), arr.end());
```

---

# 30. Pair in Priority Queue

This is extremely important in competitive programming.

Suppose:

```cpp
priority_queue<pair<int,int>> pq;
```

Pairs are compared lexicographically.

For:

```text
(5,10)
(5,20)
(3,100)
```

top will be:

```text
(5,20)
```

because first values are equal and 20 > 10.

---

# 31. Min Heap of Pairs

```cpp
priority_queue<
    pair<int,int>,
    vector<pair<int,int>>,
    greater<pair<int,int>>
> pq;
```

Example:

```cpp
pq.push({5, 100});
pq.push({2, 200});
pq.push({3, 300});
```

Top:

```text
(2,200)
```

This is heavily used in:

* Dijkstra
* Prim's algorithm
* K-th smallest
* Scheduling
* Graph problems

---

# 32. Custom Priority Queue

Suppose we have:

```cpp
struct Student {
    string name;
    int marks;
};
```

We want highest marks first.

```cpp
struct Compare {
    bool operator()(Student a, Student b) {
        return a.marks < b.marks;
    }
};
```

Then:

```cpp
priority_queue<Student,
               vector<Student>,
               Compare> pq;
```

Now:

```cpp
pq.push({"Raj", 80});
pq.push({"Aman", 95});
pq.push({"Rahul", 70});

cout << pq.top().name;
```

Output:

```text
Aman
```

---

# 33. Heapify — The Most Important Concept

You should be very comfortable with these two:

### Heapify Up

Used after:

```text
INSERT
```

Typical direction:

```text
leaf → root
```

---

### Heapify Down

Used after:

```text
DELETE ROOT
BUILD HEAP
HEAP SORT
```

Typical direction:

```text
root → leaf
```

This distinction is extremely useful.

```text
INSERT
   ↓
HEAPIFY UP


DELETE ROOT
   ↓
HEAPIFY DOWN


BUILD HEAP
   ↓
HEAPIFY DOWN


HEAP SORT
   ↓
HEAPIFY DOWN
```

---

# 34. Delete Root vs Delete Any Element

### Delete root

Very efficient:

```text
O(log N)
```

Because we know index:

```text
0
```

### Delete arbitrary value

If we need to first search for it:

```text
O(N)
```

Then heapify:

```text
O(log N)
```

Overall:

```text
O(N)
```

because finding the element dominates.

This is another important interview point.

---

# 35. Searching in Heap

Suppose:

```text
          100
        /     \
       50      80
      /  \    /  \
    20   30  60   70
```

Can we perform normal BST search?

No.

Because heap doesn't have:

```text
left < root < right
```

Therefore arbitrary search can take:

```text
O(N)
```

---

# 36. Find K-th Largest

This is a very common coding-round question.

Suppose:

```text
arr = [3,2,1,5,6,4]
k = 2
```

Answer:

```text
5
```

A simple approach:

Use a **Min Heap of size K**.

```cpp
int kthLargest(vector<int>& arr, int k) {

    priority_queue<
        int,
        vector<int>,
        greater<int>
    > pq;

    for (int x : arr) {

        pq.push(x);

        if (pq.size() > k)
            pq.pop();
    }

    return pq.top();
}
```

### Why does this work?

Keep only the largest `k` elements.

For:

```text
k = 2
```

heap eventually contains:

```text
5 6
```

Minimum among them:

```text
5
```

Therefore:

```text
2nd largest = 5
```

Complexity:

```text
O(N log K)
```

---

# 37. K-th Smallest

Use a Max Heap of size K.

```cpp
int kthSmallest(vector<int>& arr, int k) {

    priority_queue<int> pq;

    for (int x : arr) {

        pq.push(x);

        if (pq.size() > k)
            pq.pop();
    }

    return pq.top();
}
```

Complexity:

```text
O(N log K)
```

---

# 38. Top K Largest Elements

Use Min Heap of size K.

```cpp
vector<int> topKLargest(vector<int>& arr, int k) {

    priority_queue<
        int,
        vector<int>,
        greater<int>
    > pq;

    for (int x : arr) {

        pq.push(x);

        if (pq.size() > k)
            pq.pop();
    }

    vector<int> ans;

    while (!pq.empty()) {

        ans.push_back(pq.top());
        pq.pop();
    }

    return ans;
}
```

---

# 39. Merge K Sorted Arrays

Another famous heap problem.

Suppose:

```text
A = [1,4,7]
B = [2,5,8]
C = [3,6,9]
```

We want:

```text
[1,2,3,4,5,6,7,8,9]
```

Use a Min Heap.

Each heap element can contain:

```text
value
array number
index inside array
```

For example:

```cpp
struct Node {
    int value;
    int arrayIndex;
    int elementIndex;
};
```

Then push the first element of every array into the heap.

Repeatedly:

1. Take smallest.
2. Add to answer.
3. Insert next element from the same array.

Complexity:

```text
O(N log K)
```

where:

```text
K = number of arrays
N = total elements
```

This pattern is extremely important.

---

# 40. Merge K Sorted Linked Lists

Same idea.

Use:

```cpp
priority_queue<Node*, vector<Node*>, Compare>
```

Push the head of every linked list.

Then:

```text
smallest node
     ↓
answer
     ↓
push next node
```

Complexity:

```text
O(N log K)
```

---

# 41. Sort a Nearly Sorted Array

Suppose every element is at most `K` positions away from its correct position.

Example:

```text
[6,5,3,2,8,10,9]
```

If `K = 3`.

Use a Min Heap of size:

```text
K + 1
```

Algorithm:

```cpp
priority_queue<
    int,
    vector<int>,
    greater<int>
> pq;
```

Put first `K+1` elements.

Then:

```text
remove minimum
insert next
```

Complexity:

```text
O(N log K)
```

---

# 42. Find Median of a Stream

This is a **very important advanced heap problem**.

We use **two heaps**:

```text
Max Heap
   +
Min Heap
```

Idea:

```text
smaller half → Max Heap

larger half → Min Heap
```

Example:

```text
Numbers:
10 20 30 40 50
```

Max Heap:

```text
10 20 30
```

Min Heap:

```text
40 50
```

The middle can be obtained from heap tops.

This is commonly asked in coding interviews.

---

# 43. Why Two Heaps?

Suppose numbers are:

```text
1 2 3 4 5 6
```

Divide:

```text
Max Heap:
1 2 3

Min Heap:
4 5 6
```

The boundary is:

```text
3 | 4
```

Median:

```text
(3 + 4) / 2
```

For an odd number of elements, one heap has one extra element.

---

# 44. Find Median Code

A basic implementation:

```cpp
class MedianFinder {

    priority_queue<int> left;

    priority_queue<
        int,
        vector<int>,
        greater<int>
    > right;

public:

    void addNum(int num) {

        if (left.empty() || num <= left.top())
            left.push(num);
        else
            right.push(num);

        // Balance
        if (left.size() > right.size() + 1) {
            right.push(left.top());
            left.pop();
        }

        if (right.size() > left.size()) {
            left.push(right.top());
            right.pop();
        }
    }

    double findMedian() {

        if (left.size() > right.size())
            return left.top();

        return (left.top() + right.top()) / 2.0;
    }
};
```

Insertion:

```text
O(log N)
```

Median:

```text
O(1)
```

---

# 45. Last Stone Weight Type Problems

Heap is perfect when repeatedly removing the largest/smallest.

Example:

```text
stones = [2,7,4,1,8,1]
```

Use:

```cpp
priority_queue<int> pq;
```

Repeatedly:

```text
largest
second largest
     ↓
process them
     ↓
push result if needed
```

This is a classic **Max Heap pattern**.

---

# 46. Task Scheduling

Suppose:

```text
tasks have priorities
```

Always execute the highest-priority task.

Use:

```cpp
priority_queue<int> pq;
```

If smallest priority should execute first:

```cpp
priority_queue<int,
               vector<int>,
               greater<int>> pq;
```

This is the general idea behind many scheduling problems.

---

# 47. Dijkstra Uses a Heap

In Dijkstra's shortest-path algorithm, we repeatedly need:

```text
vertex with minimum current distance
```

So we use a Min Heap:

```cpp
priority_queue<
    pair<int,int>,
    vector<pair<int,int>>,
    greater<pair<int,int>>
> pq;
```

where:

```text
pair = {distance, vertex}
```

Then:

```cpp
auto [dist, node] = pq.top();
pq.pop();
```

This gives the vertex having the smallest distance.

---

# 48. Prim's Algorithm

Prim's MST algorithm also repeatedly needs the minimum-weight edge.

Therefore:

```text
Prim
 ↓
Min Heap
```

This is another major application.

---

# 49. Heap Operations Summary

For a proper heap:

| Operation              |              Complexity |
| ---------------------- | ----------------------: |
| Get root               |                    O(1) |
| Insert                 |                O(log N) |
| Delete root            |                O(log N) |
| Heapify up             |                O(log N) |
| Heapify down           |                O(log N) |
| Build heap             |                    O(N) |
| Heap sort              |              O(N log N) |
| Search arbitrary value |                    O(N) |
| Delete arbitrary value |                    O(N) |
| Update value           | O(log N) if index known |
| Min in Min Heap        |                    O(1) |
| Max in Max Heap        |                    O(1) |

---

# 50. Complete Manual Max Heap Implementation

You should practice this because it teaches the actual heap mechanics.

```cpp
#include <bits/stdc++.h>
using namespace std;

class MaxHeap {

private:

    vector<int> heap;

    void heapifyUp(int index) {

        while (index > 0) {

            int parent = (index - 1) / 2;

            if (heap[parent] >= heap[index])
                break;

            swap(heap[parent], heap[index]);

            index = parent;
        }
    }

    void heapifyDown(int index) {

        int n = heap.size();

        while (true) {

            int largest = index;

            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < n &&
                heap[left] > heap[largest]) {
                largest = left;
            }

            if (right < n &&
                heap[right] > heap[largest]) {
                largest = right;
            }

            if (largest == index)
                break;

            swap(heap[index], heap[largest]);

            index = largest;
        }
    }

public:

    // Insert
    void insert(int value) {

        heap.push_back(value);

        heapifyUp(heap.size() - 1);
    }

    // Get maximum
    int getMax() {

        if (heap.empty())
            throw runtime_error("Heap is empty");

        return heap[0];
    }

    // Delete maximum
    void deleteMax() {

        if (heap.empty())
            return;

        if (heap.size() == 1) {
            heap.pop_back();
            return;
        }

        heap[0] = heap.back();

        heap.pop_back();

        heapifyDown(0);
    }

    // Display
    void display() {

        for (int x : heap)
            cout << x << " ";

        cout << endl;
    }

    // Size
    int size() {

        return heap.size();
    }

    // Empty
    bool empty() {

        return heap.empty();
    }
};

int main() {

    MaxHeap h;

    h.insert(50);
    h.insert(30);
    h.insert(40);
    h.insert(10);
    h.insert(20);
    h.insert(60);

    cout << "Heap: ";
    h.display();

    cout << "Maximum: "
         << h.getMax() << endl;

    h.deleteMax();

    cout << "After deletion: ";
    h.display();

    return 0;
}
```

---

# 51. Expected Coding Questions

Now the most important part for your DSA preparation.

## Level 1 — MUST KNOW

These are the questions you should definitely be able to solve.

### Q1. Implement Max Heap

Operations:

```text
insert
getMax
deleteMax
display
```

---

### Q2. Implement Min Heap

Same operations:

```text
insert
getMin
deleteMin
```

---

### Q3. Implement Heapify

Given an array, convert a subtree into a heap.

---

### Q4. Build Heap

Convert:

```text
[10,20,15,30,40,50]
```

into a Max Heap.

---

### Q5. Heap Sort

Implement:

```cpp
heapSort(arr);
```

---

### Q6. Find K-th Largest

Use:

```text
Min Heap of size K
```

Complexity:

```text
O(N log K)
```

---

### Q7. Find K-th Smallest

Use:

```text
Max Heap of size K
```

---

### Q8. Find Top K Largest

Min Heap.

---

### Q9. Find Top K Smallest

Max Heap.

---

### Q10. Check Whether Array Is a Max Heap

Very common.

```cpp
bool isMaxHeap(vector<int>& arr) {

    int n = arr.size();

    for (int i = 0; i <= n / 2 - 1; i++) {

        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n &&
            arr[i] < arr[left])
            return false;

        if (right < n &&
            arr[i] < arr[right])
            return false;
    }

    return true;
}
```

Complexity:

```text
O(N)
```

---

# 52. Level 2 — VERY IMPORTANT

### Q11. Merge K Sorted Arrays

Pattern:

```text
Min Heap
```

---

### Q12. Merge K Sorted Linked Lists

Pattern:

```text
Min Heap
```

---

### Q13. Sort a Nearly Sorted Array

Pattern:

```text
Min Heap of size K+1
```

---

### Q14. Find Median from Data Stream

Pattern:

```text
Max Heap + Min Heap
```

---

### Q15. K Closest Elements

Depending on the exact problem:

```text
Max Heap
```

or

```text
Min Heap
```

---

### Q16. K Closest Points to Origin

Use a Max Heap of size K.

Distance:

```text
x*x + y*y
```

No need to calculate square root.

---

### Q17. Top K Frequent Elements

Use:

```text
Hash Map
+
Heap
```

First calculate frequencies:

```text
value → frequency
```

Then use heap based on frequency.

---

### Q18. Sort Characters by Frequency

Use:

```text
Hash Map + Heap
```

---

### Q19. Reorganize String

Again:

```text
frequency map
+
Max Heap
```

Choose the most frequent character while ensuring adjacent characters are different.

---

### Q20. Task Scheduler

Usually:

```text
frequency map
+
Max Heap
```

---

# 53. Level 3 — ADVANCED

These are excellent coding-round problems.

### Q21. Find Median of Running Stream

Two heaps.

---

### Q22. Sliding Window Median

Use:

```text
two heaps
```

plus additional management.

---

### Q23. K-th Largest Element in a Stream

Maintain:

```text
Min Heap of size K
```

Every incoming element:

```text
push
if size > K
    pop
```

---

### Q24. Ugly Number

Can be solved using heap + set, although other approaches are more efficient.

---

### Q25. Smallest Range Covering K Sorted Lists

Very important advanced heap problem.

Pattern:

```text
Min Heap
+
track maximum
```

---

### Q26. Minimum Cost to Connect Ropes

This is a **classic heap problem**.

Suppose:

```text
ropes = [4,3,2,6]
```

Always join the two smallest:

```text
2 + 3 = 5
```

Then:

```text
4 + 5 = 9
```

Then:

```text
6 + 9 = 15
```

Total:

```text
5 + 9 + 15 = 29
```

Use Min Heap.

```cpp
int minCost(vector<int>& ropes) {

    priority_queue<
        int,
        vector<int>,
        greater<int>
    > pq(
        ropes.begin(),
        ropes.end()
    );

    int cost = 0;

    while (pq.size() > 1) {

        int a = pq.top();
        pq.pop();

        int b = pq.top();
        pq.pop();

        int sum = a + b;

        cost += sum;

        pq.push(sum);
    }

    return cost;
}
```

Complexity:

```text
O(N log N)
```

---

# 54. K Closest Points

Suppose:

```text
points = [[1,3],[-2,2],[5,8]]
k = 2
```

Distance from origin:

```text
x² + y²
```

We need the smallest K.

Use a **Max Heap of size K**.

Why Max Heap?

Because we want to remove the **farthest among the selected K** whenever a new closer point arrives.

This is a very important pattern:

```text
Want K smallest
       ↓
Max Heap of size K
```

---

# 55. Top K Frequency

Suppose:

```text
arr = [1,1,1,2,2,3]
```

Frequency:

```text
1 → 3
2 → 2
3 → 1
```

For:

```text
k = 2
```

answer:

```text
1,2
```

General pattern:

```text
frequency map
      ↓
heap based on frequency
      ↓
top K
```

This pattern appears repeatedly in interview questions.

---

# 56. Important Heap Patterns

You should memorize these patterns.

### Pattern 1 — K largest

```text
Min Heap of size K
```

### Pattern 2 — K smallest

```text
Max Heap of size K
```

### Pattern 3 — Top K frequent

```text
Hash Map + Heap
```

### Pattern 4 — Merge K sorted

```text
Min Heap
```

### Pattern 5 — Minimum cost

```text
Min Heap
```

### Pattern 6 — Repeatedly remove maximum

```text
Max Heap
```

### Pattern 7 — Repeatedly remove minimum

```text
Min Heap
```

### Pattern 8 — Median

```text
Max Heap + Min Heap
```

### Pattern 9 — Shortest path

```text
Min Heap
```

### Pattern 10 — MST / Prim

```text
Min Heap
```

---

# 57. The Most Important STL Syntax

You should know these without thinking.

## Max Heap

```cpp
priority_queue<int> pq;
```

Insert:

```cpp
pq.push(x);
```

Top:

```cpp
pq.top();
```

Delete top:

```cpp
pq.pop();
```

Size:

```cpp
pq.size();
```

Empty:

```cpp
pq.empty();
```

---

## Min Heap

```cpp
priority_queue<
    int,
    vector<int>,
    greater<int>
> pq;
```

---

## Min Heap of Pairs

```cpp
priority_queue<
    pair<int,int>,
    vector<pair<int,int>>,
    greater<pair<int,int>>
> pq;
```

This syntax is worth memorizing for competitive programming.

---

# 58. Heap vs Priority Queue

These terms are related but not exactly identical.

### Heap

The underlying data structure.

### Priority Queue

An abstract data type where:

```text
highest/lowest priority element
```

can be accessed efficiently.

C++ implements `priority_queue` using a heap-based structure.

So practically:

```text
Heap
 ↓
Priority Queue
 ↓
C++ priority_queue
```

---

# 59. Common Mistakes

### Mistake 1

Thinking heap is BST.

Wrong.

---

### Mistake 2

Thinking every child must be smaller than root in Max Heap.

Not exactly.

Only **direct parent-child relationship** must satisfy:

```text
parent >= child
```

---

### Mistake 3

Using Min Heap when you need K largest.

Remember:

```text
K largest → Min Heap
K smallest → Max Heap
```

---

### Mistake 4

Forgetting:

```cpp
pq.pop()
```

doesn't return the value.

Wrong:

```cpp
int x = pq.pop();
```

Correct:

```cpp
int x = pq.top();
pq.pop();
```

---

### Mistake 5

Thinking build heap is O(N log N).

Actual:

```text
O(N)
```

---

### Mistake 6

Trying binary search in a heap.

You can't.

Arbitrary search is:

```text
O(N)
```

---

# 60. Heap Cheat Sheet

Keep this table for revision:

| Concept        | Remember             |
| -------------- | -------------------- |
| Heap           | Complete Binary Tree |
| Max Heap       | Parent ≥ children    |
| Min Heap       | Parent ≤ children    |
| Max root       | Maximum              |
| Min root       | Minimum              |
| Parent         | `(i-1)/2`            |
| Left child     | `2*i+1`              |
| Right child    | `2*i+2`              |
| Insert         | Heapify Up           |
| Delete Root    | Heapify Down         |
| Build Heap     | Bottom-up Heapify    |
| Build Heap     | O(N)                 |
| Heap Sort      | O(N log N)           |
| Search         | O(N)                 |
| K Largest      | Min Heap             |
| K Smallest     | Max Heap             |
| Median         | Two Heaps            |
| Merge K Sorted | Min Heap             |
| Min Cost       | Min Heap             |
| Dijkstra       | Min Heap             |
| Prim           | Min Heap             |

---

# 61. The Heap Topic You Should Master for DSA

For your coding preparation, I would divide Heap into these levels:

```text
                    HEAP
                      |
          ┌───────────┴───────────┐
          |                       |
      MAX HEAP                MIN HEAP
          |                       |
     Parent >= child         Parent <= child
          |                       |
          └───────────┬───────────┘
                      |
                  ARRAY
                      |
             ┌────────┴────────┐
             |                 |
         Heapify Up        Heapify Down
             |                 |
          Insert           Delete Root
             |                 |
             └────────┬────────┘
                      |
                  Build Heap
                      |
                  Heap Sort
                      |
               Priority Queue
                      |
          ┌───────────┼────────────┐
          |           |            |
        K-th         Top K       Median
       Largest      Frequent    Two Heaps
          |
       Merge K
          |
     Dijkstra / Prim
```

## What you should be able to write without looking at notes

Before moving to the next data structure, make sure you can independently code these:

1. **Max Heap from scratch**
2. **Min Heap from scratch**
3. `insert()`
4. `deleteRoot()`
5. `heapifyUp()`
6. `heapifyDown()`
7. `buildHeap()`
8. `heapSort()`
9. Check whether an array is a heap
10. K-th largest using Min Heap
11. K-th smallest using Max Heap
12. Top K frequent elements
13. Merge K sorted arrays/lists
14. Minimum cost to connect ropes
15. Median from data stream
16. K closest points
17. Use `priority_queue` correctly

Once these are comfortable, you have covered **most of the Heap knowledge needed for normal DSA labs and coding rounds**.


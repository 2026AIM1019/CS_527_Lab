Absolutely. After **Array → Linked List → Stack**, the next important DSA topic is **Queue**.

For a DSA lab, you should know **Queue implementation, STL queue, Circular Queue, Deque, Priority Queue**, and especially problems such as **first non-repeating character, sliding window maximum, BFS, queue using stacks, and circular queue**.

---

# 1. What is a Queue?

A Queue follows:

> **FIFO — First In, First Out**

Think of people standing in a line:

```text
Front                         Rear
  ↓                             ↓
 10 → 20 → 30 → 40
```

The person who entered first leaves first.

So:

```text
enqueue(10)
enqueue(20)
enqueue(30)

dequeue() → 10
dequeue() → 20
```

Compare with Stack:

```text
Stack → LIFO
Queue → FIFO
```

---

# 2. Basic Queue Operations

The important operations are:

| Operation              | Meaning                   |
| ---------------------- | ------------------------- |
| `push()` / `enqueue()` | Add element at rear       |
| `pop()` / `dequeue()`  | Remove element from front |
| `front()`              | Access first element      |
| `back()`               | Access last element       |
| `empty()`              | Check empty               |
| `size()`               | Number of elements        |

For C++ STL `queue`, these are the methods you'll use.

---

# 3. Creating a Queue in C++

Include:

```cpp
#include <queue>
```

or simply:

```cpp
#include <bits/stdc++.h>
using namespace std;
```

Create:

```cpp
queue<int> q;
```

This creates an empty queue of integers.

---

# 4. Add Elements — `push()`

```cpp
q.push(10);
q.push(20);
q.push(30);
```

Queue:

```text
Front                 Rear
 ↓                      ↓
10 → 20 → 30
```

---

# 5. Access Front Element

```cpp
cout << q.front();
```

Output:

```text
10
```

Important:

```cpp
q.front()
```

**does not remove** the element.

---

# 6. Access Last Element

Use:

```cpp
q.back()
```

Example:

```cpp
queue<int> q;

q.push(10);
q.push(20);
q.push(30);

cout << q.back();
```

Output:

```text
30
```

---

# 7. Remove Element — `pop()`

```cpp
q.pop();
```

Before:

```text
10 → 20 → 30
```

After:

```text
20 → 30
```

Important:

Just like `stack::pop()`, `queue::pop()` **doesn't return the removed value**.

Wrong:

```cpp
int x = q.pop();  // WRONG
```

Correct:

```cpp
int x = q.front();
q.pop();
```

---

# 8. Check Empty

```cpp
if(q.empty()) {
    cout << "Queue is empty";
}
```

---

# 9. Find Size

```cpp
cout << q.size();
```

Example:

```cpp
queue<int> q;

q.push(10);
q.push(20);
q.push(30);

cout << q.size();
```

Output:

```text
3
```

---

# 10. Complete STL Queue Example

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {

    queue<int> q;

    q.push(10);
    q.push(20);
    q.push(30);

    cout << "Front: " << q.front() << endl;
    cout << "Rear: " << q.back() << endl;
    cout << "Size: " << q.size() << endl;

    q.pop();

    cout << "New Front: " << q.front() << endl;

    if(q.empty())
        cout << "Empty";
    else
        cout << "Not Empty";

    return 0;
}
```

Output:

```text
Front: 10
Rear: 30
Size: 3
New Front: 20
Not Empty
```

---

# 11. Printing a Queue

You cannot directly do:

```cpp
cout << q;
```

Use:

```cpp
while(!q.empty()) {

    cout << q.front() << " ";

    q.pop();
}
```

Output:

```text
10 20 30
```

But this destroys the queue.

To preserve it:

```cpp
queue<int> temp = q;

while(!temp.empty()) {

    cout << temp.front() << " ";

    temp.pop();
}
```

---

# 12. Can We Update an Element Directly?

This is an important difference from arrays/vectors.

You cannot do:

```cpp
q[2] = 100;  // WRONG
```

A queue intentionally gives access only to:

```cpp
q.front()
q.back()
```

If you need to modify elements in the middle, you generally need to remove/rebuild elements or use another data structure.

For example:

```text
10 → 20 → 30 → 40
```

To change `30` to `100`, you can temporarily remove elements:

```cpp
queue<int> temp;

while(!q.empty()) {

    int x = q.front();
    q.pop();

    if(x == 30)
        x = 100;

    temp.push(x);
}

q = temp;
```

Now:

```text
10 → 20 → 100 → 40
```

But if your problem frequently requires random access/update, **queue is probably not the appropriate data structure**.

---

# 13. Implement Queue Using Array

This is important for a DSA lab.

We'll use:

```cpp
int arr[100];
int front = 0;
int rear = -1;
```

Initially:

```text
front = 0
rear = -1
```

---

## Enqueue

```cpp
void enqueue(int x) {

    if(rear == 99) {
        cout << "Queue Overflow";
        return;
    }

    rear++;
    arr[rear] = x;
}
```

After:

```cpp
enqueue(10);
enqueue(20);
enqueue(30);
```

We have:

```text
front             rear
 ↓                  ↓
10   20   30
```

---

# 14. Dequeue

```cpp
void dequeue() {

    if(front > rear) {
        cout << "Queue Underflow";
        return;
    }

    front++;
}
```

After removing `10`:

```text
front        rear
 ↓             ↓
20    30
```

---

# 15. Peek

```cpp
int peek() {

    if(front > rear)
        return -1;

    return arr[front];
}
```

---

# 16. Complete Array Queue

```cpp
#include <bits/stdc++.h>
using namespace std;

class Queue {
    int arr[100];
    int front;
    int rear;

public:

    Queue() {
        front = 0;
        rear = -1;
    }

    void enqueue(int x) {

        if(rear == 99) {
            cout << "Overflow\n";
            return;
        }

        arr[++rear] = x;
    }

    void dequeue() {

        if(front > rear) {
            cout << "Underflow\n";
            return;
        }

        front++;
    }

    int peek() {

        if(front > rear)
            return -1;

        return arr[front];
    }

    bool empty() {
        return front > rear;
    }

    int size() {
        if(empty())
            return 0;

        return rear - front + 1;
    }
};

int main() {

    Queue q;

    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);

    cout << q.peek() << endl;

    q.dequeue();

    cout << q.peek() << endl;

    return 0;
}
```

---

# 17. Problem With This Array Queue

Suppose the array has size 5:

```text
10 20 30 40 50
↑           ↑
front       rear
```

Now dequeue three:

```text
_  _  _  40 50
         ↑    ↑
       front rear
```

There is free space at the beginning.

But our `rear` is already at the end.

So if we try:

```cpp
enqueue(60);
```

our simple implementation may say:

```text
Overflow
```

even though there is unused space.

This is why we need a:

# Circular Queue

---

# 18. Circular Queue

Instead of treating the array as a straight line:

```text
0 1 2 3 4
```

we treat it as a circle:

```text
       0
    /     \
   4       1
    \     /
      3-2
```

After reaching the last position, we return to index `0`.

The key formula is:

```cpp
rear = (rear + 1) % size;
```

This is extremely important.

---

# 19. Circular Queue Implementation

```cpp
#include <bits/stdc++.h>
using namespace std;

class CircularQueue {

    int arr[5];
    int front;
    int rear;
    int count;

public:

    CircularQueue() {
        front = 0;
        rear = -1;
        count = 0;
    }

    void enqueue(int x) {

        if(count == 5) {
            cout << "Queue Full\n";
            return;
        }

        rear = (rear + 1) % 5;

        arr[rear] = x;

        count++;
    }

    void dequeue() {

        if(count == 0) {
            cout << "Queue Empty\n";
            return;
        }

        front = (front + 1) % 5;

        count--;
    }

    int peek() {

        if(count == 0)
            return -1;

        return arr[front];
    }

    bool empty() {
        return count == 0;
    }

    bool full() {
        return count == 5;
    }
};
```

---

# 20. Queue Using Linked List

Another important implementation.

Structure:

```text
front                       rear
 ↓                            ↓
10 → 20 → 30 → 40 → NULL
```

We maintain two pointers:

```cpp
Node* front;
Node* rear;
```

### Enqueue

```cpp
void enqueue(Node*& front, Node*& rear, int x) {

    Node* newNode = new Node(x);

    if(rear == nullptr) {
        front = rear = newNode;
        return;
    }

    rear->next = newNode;
    rear = newNode;
}
```

### Dequeue

```cpp
void dequeue(Node*& front, Node*& rear) {

    if(front == nullptr)
        return;

    Node* temp = front;

    front = front->next;

    if(front == nullptr)
        rear = nullptr;

    delete temp;
}
```

This gives:

```text
enqueue → O(1)
dequeue → O(1)
```

---

# 21. Queue Complexity

For a properly implemented queue:

| Operation | Complexity |
| --------- | ---------: |
| Enqueue   |       O(1) |
| Dequeue   |       O(1) |
| Front     |       O(1) |
| Back      |       O(1) |
| Empty     |       O(1) |
| Size      |       O(1) |

---

# 22. Deque

Another important C++ STL structure is `deque`.

Deque means:

> **Double Ended Queue**

You can insert/delete from **both ends**.

Create:

```cpp
deque<int> dq;
```

Operations:

```cpp
dq.push_front(10);
dq.push_back(20);

dq.pop_front();
dq.pop_back();
```

Access:

```cpp
dq.front();
dq.back();
dq[2];
```

You can also:

```cpp
dq.size();
dq.empty();
```

---

# 23. Priority Queue

This is another queue-like STL container that you should learn for DSA.

```cpp
priority_queue<int> pq;
```

By default, it is a **max heap**.

```cpp
pq.push(10);
pq.push(50);
pq.push(20);
```

Top:

```cpp
cout << pq.top();
```

Output:

```text
50
```

Remove:

```cpp
pq.pop();
```

---

## Min Priority Queue

Very important syntax:

```cpp
priority_queue<int, vector<int>, greater<int>> pq;
```

Now smallest element is at the top.

```cpp
pq.push(10);
pq.push(50);
pq.push(20);

cout << pq.top();
```

Output:

```text
10
```

Priority queues become extremely important later for **heaps, Dijkstra, scheduling, top-K problems, etc.**

---

# ⭐ Expected Queue Questions for DSA Lab

Now let's get to the important part.

I would prepare these in this order.

---

# Level 1 — Must Know

## 1. Implement Queue Using Array

Implement:

```text
enqueue
dequeue
peek
isEmpty
```

You should be able to write this without notes.

---

# 2. Implement Queue Using Linked List

Maintain:

```text
front
rear
```

and implement:

```text
enqueue
dequeue
peek
```

---

# 3. Implement Circular Queue ⭐

Very likely lab question.

You need to understand:

```cpp
(rear + 1) % n
```

and:

```cpp
(front + 1) % n
```

---

# 4. Reverse a Queue ⭐

Given:

```text
10 20 30 40 50
```

Output:

```text
50 40 30 20 10
```

### Solution using stack

```cpp
queue<int> q;
stack<int> st;

while(!q.empty()) {

    st.push(q.front());
    q.pop();
}

while(!st.empty()) {

    q.push(st.top());
    st.pop();
}
```

Why?

Queue:

```text
10 20 30 40
```

Move into stack:

```text
40
30
20
10
```

Then put back:

```text
40 30 20 10
```

Complexity:

```text
O(n)
```

---

# 5. Generate Binary Numbers from 1 to N ⭐

Given:

```text
N = 5
```

Output:

```text
1
10
11
100
101
```

This is a nice queue problem.

### Solution

Start with:

```cpp
queue<string> q;
q.push("1");
```

Then:

```cpp
for(int i = 0; i < n; i++) {

    string current = q.front();
    q.pop();

    cout << current << endl;

    q.push(current + "0");
    q.push(current + "1");
}
```

This is a very good example of using a queue for **level/order generation**.

---

# 6. First Non-Repeating Character ⭐⭐⭐

Given:

```text
a a b c
```

After each character, determine the first character that has appeared only once.

For:

```text
aabc
```

the outputs can be:

```text
a
#
b
b
```

Use:

```cpp
queue<char> q;
int freq[26] = {0};
```

### Code

```cpp
string s;
cin >> s;

queue<char> q;
int freq[26] = {0};

for(char c : s) {

    freq[c - 'a']++;
    q.push(c);

    while(!q.empty() && freq[q.front() - 'a'] > 1)
        q.pop();

    if(q.empty())
        cout << "# ";
    else
        cout << q.front() << " ";
}
```

This is a good **queue + frequency array** problem.

---

# 7. Queue Using Two Stacks ⭐⭐⭐

This is an important interview question.

Create:

```cpp
stack<int> s1, s2;
```

### Enqueue

```cpp
void enqueue(int x) {
    s1.push(x);
}
```

### Dequeue

```cpp
int dequeue() {

    if(s2.empty()) {

        while(!s1.empty()) {
            s2.push(s1.top());
            s1.pop();
        }
    }

    int x = s2.top();
    s2.pop();

    return x;
}
```

Example:

```text
enqueue 10
enqueue 20
enqueue 30
```

`s1`:

```text
30
20
10
```

Move to `s2`:

```text
10
20
30
```

Now `s2.top()` is `10`.

Therefore FIFO behavior is obtained.

---

# 8. Stack Using Two Queues

The reverse problem is also common.

Implement:

```text
push()
pop()
top()
```

using two queues.

This tests whether you understand the difference between:

```text
FIFO
LIFO
```

---

# 9. Sliding Window Maximum ⭐⭐⭐⭐

This is a **very important DSA problem**.

Given:

```text
1 3 -1 -3 5 3 6 7
```

and:

```text
k = 3
```

Output:

```text
3 3 5 5 6 7
```

For each window:

```text
1 3 -1 → 3

3 -1 -3 → 3

-1 -3 5 → 5

-3 5 3 → 5

5 3 6 → 6

3 6 7 → 7
```

The efficient solution uses a **deque**.

```cpp
vector<int> maxSlidingWindow(vector<int>& v, int k) {

    deque<int> dq;
    vector<int> ans;

    for(int i = 0; i < v.size(); i++) {

        // Remove elements outside window
        while(!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        // Remove smaller elements
        while(!dq.empty() && v[dq.back()] <= v[i])
            dq.pop_back();

        dq.push_back(i);

        if(i >= k - 1)
            ans.push_back(v[dq.front()]);
    }

    return ans;
}
```

Complexity:

```text
O(n)
```

This is significantly better than checking every window separately, which can be O(nk).

---

# 10. First Negative Number in Every Window

Given:

```text
12 -1 -7 8 -15 30 16 28
```

For:

```text
k = 3
```

find the first negative number in every window.

Output:

```text
-1 -1 -7 -15 -15 0
```

This can be solved efficiently using a queue of indices.

Pattern:

```text
Sliding Window + Queue
```

---

# 11. Rotten Oranges ⭐⭐⭐⭐

This is a very important problem because it introduces **BFS using a queue**.

Grid:

```text
2 = rotten orange
1 = fresh orange
0 = empty
```

Example:

```text
2 1 1
1 1 0
0 1 1
```

Every minute, rotten oranges infect neighboring fresh oranges.

Find minimum time until all oranges become rotten.

The key idea:

```cpp
queue<pair<int,int>> q;
```

Put all initially rotten oranges into the queue.

Then:

```text
process current level
↓
infect neighbors
↓
push newly rotten oranges
↓
next level = next minute
```

This is essentially **multi-source BFS**.

---

# 12. Binary Tree Level Order Traversal ⭐⭐⭐

Later, when you learn trees, this will become one of the most important queue applications.

Tree:

```text
        1
       / \
      2   3
     / \
    4   5
```

Level order:

```text
1 2 3 4 5
```

Code:

```cpp
queue<Node*> q;

q.push(root);

while(!q.empty()) {

    Node* curr = q.front();
    q.pop();

    cout << curr->data << " ";

    if(curr->left)
        q.push(curr->left);

    if(curr->right)
        q.push(curr->right);
}
```

This is why you should understand Queue before Trees.

---

# 13. BFS of a Graph ⭐⭐⭐⭐

Another extremely important application.

Given a graph, BFS uses:

```cpp
queue<int> q;
```

Basic pattern:

```cpp
q.push(start);
visited[start] = true;

while(!q.empty()) {

    int node = q.front();
    q.pop();

    cout << node << " ";

    for(int neighbor : graph[node]) {

        if(!visited[neighbor]) {

            visited[neighbor] = true;
            q.push(neighbor);
        }
    }
}
```

We'll study this properly when we reach **Graphs**.

---

# 14. Josephus Problem

People are arranged in a circle:

```text
1 2 3 4 5
```

Every `k`-th person is eliminated.

Find the survivor.

This can be solved using:

* circular queue
* mathematical recurrence

It's a good problem for practicing circular structures.

---

# 15. Implement Queue Using Circular Array

This is one I would **definitely expect in a DSA lab** because it tests:

```text
arrays
+
indices
+
modulo
+
FIFO
```

Remember the central formula:

```cpp
rear = (rear + 1) % n;
```

and:

```cpp
front = (front + 1) % n;
```

---

# ⭐ Queue Problem Priority

For your lab preparation, I'd rank them:

### 🔴 Must Know

```text
1. Queue using array
2. Queue using linked list
3. Enqueue / Dequeue / Front / Rear
4. Circular Queue
5. Reverse Queue
```

### 🟠 Very Important

```text
6. Queue using two stacks
7. Stack using two queues
8. First non-repeating character
9. Generate binary numbers
10. First negative number in every window
```

### 🟡 Advanced / Coding-Round Level

```text
11. Sliding Window Maximum
12. Rotten Oranges
13. BFS
14. Binary Tree Level Order Traversal
15. Josephus Problem
```

---

# 🧠 How to Recognize Queue Problems

This is more important than memorizing solutions.

### Pattern 1 — First Come, First Served

If the problem says:

> Process elements in the order they arrive.

Think:

```text
QUEUE
```

---

### Pattern 2 — Level by Level

If the problem says:

> Process all elements at the current level before moving to the next level.

Think:

```text
QUEUE
```

This is why BFS uses queues.

---

### Pattern 3 — Sliding Window

If you see:

```text
maximum of every window
minimum of every window
first negative in every window
```

Think:

```text
DEQUE / MONOTONIC QUEUE
```

---

### Pattern 4 — BFS

If you see:

```text
shortest path in an unweighted graph
level order
minimum number of steps
spread/infection
```

Think:

```text
QUEUE
```

---

# 🔥 C++ Queue Cheat Sheet

## Normal Queue

```cpp
queue<int> q;
```

Add:

```cpp
q.push(x);
```

Remove:

```cpp
q.pop();
```

First:

```cpp
q.front();
```

Last:

```cpp
q.back();
```

Size:

```cpp
q.size();
```

Empty:

```cpp
q.empty();
```

---

## Deque

```cpp
deque<int> dq;
```

```cpp
dq.push_front(x);
dq.push_back(x);

dq.pop_front();
dq.pop_back();

dq.front();
dq.back();
```

---

## Max Priority Queue

```cpp
priority_queue<int> pq;
```

```cpp
pq.push(x);
pq.top();
pq.pop();
```

---

## Min Priority Queue

```cpp
priority_queue<int, vector<int>, greater<int>> pq;
```

---

# 🎯 What You Should Be Able to Write Without Looking

Before moving to the next DSA topic, make sure you can independently write these:

```text
                    QUEUE
                      │
        ┌─────────────┼─────────────┐
        ↓             ↓             ↓
      Array        Linked List      STL
        │             │             │
        ↓             ↓             ↓
    Enqueue        Enqueue       push()
    Dequeue        Dequeue       pop()
    Peek           Peek          front()
    Empty          Empty         back()
        │
        ↓
  Circular Queue
        │
        ↓
  Reverse Queue
        │
        ↓
 Queue using 2 Stacks
        │
        ↓
 First Non-Repeating
        │
        ↓
 Sliding Window
        │
        ↓
      BFS
```

The **most important transition** from Stack to Queue is to understand these three structures:

```text
Stack       → LIFO
Queue       → FIFO
Deque       → FIFO-like structure with access at BOTH ends
```

Once these are clear, the next major DSA topic should be **Recursion**, because recursion is used heavily in **trees, backtracking, divide-and-conquer, and DP**, and it will make those later topics much easier.


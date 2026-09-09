Absolutely. **Stack is one of the most important DSA topics** for a coding lab because it is simple to implement but leads to many important problems involving **parentheses, expressions, recursion, monotonic stacks, next greater elements, and undo/backtracking ideas**.

Since you're learning C++ for DSA, I'll cover both **how to implement a stack yourself** and the **STL `stack`**, then the problems you should practice.

# 1. What is a Stack?

A stack follows:

> **LIFO = Last In, First Out**

Think of a stack of plates:

```text
       30  ← Top
       20
       10
```

If you remove one plate, `30` comes out first.

So:

```text
push(10)
push(20)
push(30)

pop() → 30
```

---

# 2. Basic Stack Operations

A stack mainly has these operations:

| Operation | Meaning             |
| --------- | ------------------- |
| `push()`  | Add element         |
| `pop()`   | Remove top element  |
| `top()`   | See top element     |
| `empty()` | Check whether empty |
| `size()`  | Number of elements  |

Example:

```text
Initially:

EMPTY

push(10)

10

push(20)

20 ← top
10

push(30)

30 ← top
20
10

pop()

20 ← top
10
```

---

# 3. Using Stack in C++

C++ already provides a stack in STL.

Include:

```cpp
#include <stack>
```

or, as you usually do in competitive programming:

```cpp
#include <bits/stdc++.h>
using namespace std;
```

Create:

```cpp
stack<int> st;
```

This creates an empty integer stack.

---

# 4. `push()`

Add an element:

```cpp
st.push(10);
st.push(20);
st.push(30);
```

Stack:

```text
30 ← top
20
10
```

---

# 5. `top()`

Get the top element:

```cpp
cout << st.top();
```

Output:

```text
30
```

### Important

`top()` **does not remove** the element.

After:

```cpp
st.top();
```

the stack is still:

```text
30
20
10
```

---

# 6. `pop()`

Remove the top element:

```cpp
st.pop();
```

Before:

```text
30
20
10
```

After:

```text
20
10
```

### Important

`pop()` **does not return the removed value**.

This is wrong:

```cpp
int x = st.pop();   // WRONG
```

Instead:

```cpp
int x = st.top();
st.pop();
```

---

# 7. `empty()`

Check whether stack is empty:

```cpp
if(st.empty()) {
    cout << "Stack is empty";
}
```

It returns:

```text
true / false
```

---

# 8. `size()`

```cpp
cout << st.size();
```

Example:

```cpp
stack<int> st;

st.push(10);
st.push(20);
st.push(30);

cout << st.size();
```

Output:

```text
3
```

---

# 9. Printing a Stack

There is no direct:

```cpp
cout << st;
```

Instead:

```cpp
while(!st.empty()) {
    cout << st.top() << " ";
    st.pop();
}
```

Output:

```text
30 20 10
```

### Warning

This **destroys the stack**.

If you want to preserve it:

```cpp
stack<int> temp = st;

while(!temp.empty()) {
    cout << temp.top() << " ";
    temp.pop();
}
```

---

# 10. Complete Stack Example

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {

    stack<int> st;

    st.push(10);
    st.push(20);
    st.push(30);

    cout << "Top = " << st.top() << endl;

    cout << "Size = " << st.size() << endl;

    st.pop();

    cout << "New top = " << st.top() << endl;

    if(st.empty())
        cout << "Empty";
    else
        cout << "Not Empty";

    return 0;
}
```

Output:

```text
Top = 30
Size = 3
New top = 20
Not Empty
```

---

# 11. Stack Using Array

For a DSA lab, you should also know how to **implement a stack yourself**.

Suppose:

```cpp
int arr[100];
```

We need a variable:

```cpp
int top = -1;
```

`top = -1` means:

```text
Stack is empty
```

---

## Push

```cpp
void push(int x) {

    if(top == 99) {
        cout << "Stack Overflow";
        return;
    }

    top++;
    arr[top] = x;
}
```

Example:

```text
Initially:

top = -1

push(10)

top = 0
arr[0] = 10

push(20)

top = 1
arr[1] = 20
```

---

# 12. Pop Using Array

```cpp
void pop() {

    if(top == -1) {
        cout << "Stack Underflow";
        return;
    }

    top--;
}
```

---

# 13. Peek/Top Using Array

```cpp
int peek() {

    if(top == -1) {
        cout << "Stack is empty";
        return -1;
    }

    return arr[top];
}
```

---

# 14. Complete Stack Implementation

This is worth practicing once from scratch.

```cpp
#include <bits/stdc++.h>
using namespace std;

class Stack {
    int arr[100];
    int top;

public:

    Stack() {
        top = -1;
    }

    void push(int x) {

        if(top == 99) {
            cout << "Overflow\n";
            return;
        }

        arr[++top] = x;
    }

    void pop() {

        if(top == -1) {
            cout << "Underflow\n";
            return;
        }

        top--;
    }

    int peek() {

        if(top == -1)
            return -1;

        return arr[top];
    }

    bool empty() {
        return top == -1;
    }

    int size() {
        return top + 1;
    }
};

int main() {

    Stack st;

    st.push(10);
    st.push(20);
    st.push(30);

    cout << st.peek() << endl;

    st.pop();

    cout << st.peek() << endl;

    return 0;
}
```

---

# 15. Stack Using Linked List

You can also implement stack using a linked list.

The **head acts as the top**.

```text
top
 ↓
30 → 20 → 10 → NULL
```

Push:

```text
40

top
 ↓
40 → 30 → 20 → 10
```

Pop:

```text
top
 ↓
30 → 20 → 10
```

### Push

```cpp
void push(Node*& top, int x) {

    Node* newNode = new Node(x);

    newNode->next = top;

    top = newNode;
}
```

### Pop

```cpp
void pop(Node*& top) {

    if(top == nullptr)
        return;

    Node* temp = top;

    top = top->next;

    delete temp;
}
```

Both array and linked-list implementations are useful for understanding the data structure.

---

# 16. Complexity of Stack

| Operation | Complexity |
| --------- | ---------: |
| `push()`  |       O(1) |
| `pop()`   |       O(1) |
| `top()`   |       O(1) |
| `empty()` |       O(1) |
| `size()`  |       O(1) |

This is an important viva question.

---

# ⭐ Now the Important Part: Stack Problems

For your DSA coding lab, these are the problems I'd prioritize.

---

# Question 1 — Implement Stack

### Problem

Implement:

```text
push
pop
peek
isEmpty
```

using an array.

You should be able to write this **without looking at notes**.

---

# Question 2 — Reverse a String Using Stack

Given:

```text
hello
```

Output:

```text
olleh
```

### Solution

```cpp
string s;
cin >> s;

stack<char> st;

for(char c : s)
    st.push(c);

while(!st.empty()) {
    cout << st.top();
    st.pop();
}
```

Why does it work?

Input:

```text
h e l l o
```

Stack:

```text
o
l
l
e
h
```

Pop gives:

```text
o l l e h
```

---

# Question 3 — Check Balanced Parentheses ⭐⭐⭐

This is **one of the most important stack problems**.

Input:

```text
{[()]}
```

Output:

```text
Balanced
```

Input:

```text
{[(])}
```

Output:

```text
Not Balanced
```

### Solution

```cpp
bool isBalanced(string s) {

    stack<char> st;

    for(char c : s) {

        if(c == '(' || c == '[' || c == '{') {
            st.push(c);
        }

        else {

            if(st.empty())
                return false;

            char top = st.top();
            st.pop();

            if(c == ')' && top != '(')
                return false;

            if(c == ']' && top != '[')
                return false;

            if(c == '}' && top != '{')
                return false;
        }
    }

    return st.empty();
}
```

Usage:

```cpp
if(isBalanced(s))
    cout << "Balanced";
else
    cout << "Not Balanced";
```

### Why stack?

Because the **last opening bracket must be closed first**.

For:

```text
{ [ ( ) ] }
```

the last opening bracket is:

```text
(
```

so it must be closed first.

That's exactly LIFO.

---

# Question 4 — Valid Parentheses

A simpler version:

```text
Input:
(()())

Output:
YES
```

```text
Input:
())(

Output:
NO
```

Solution:

```cpp
int balance = 0;

for(char c : s) {

    if(c == '(')
        balance++;

    else
        balance--;

    if(balance < 0) {
        cout << "NO";
        return 0;
    }
}

if(balance == 0)
    cout << "YES";
else
    cout << "NO";
```

For only `(` and `)`, you don't actually need a stack.

This is an important lesson:

> Don't automatically use a stack just because the problem mentions parentheses.

---

# Question 5 — Min Stack ⭐⭐⭐

Design a stack that supports:

```text
push()
pop()
top()
getMin()
```

with `getMin()` in **O(1)**.

Example:

```text
push(5)
push(2)
push(10)
push(1)
```

Then:

```text
getMin() → 1
```

After popping `1`:

```text
getMin() → 2
```

### Simple solution

Use two stacks:

```cpp
stack<int> st;
stack<int> minSt;
```

Push:

```cpp
void push(int x) {

    st.push(x);

    if(minSt.empty())
        minSt.push(x);
    else
        minSt.push(min(x, minSt.top()));
}
```

Pop:

```cpp
void pop() {

    st.pop();
    minSt.pop();
}
```

Get minimum:

```cpp
int getMin() {
    return minSt.top();
}
```

Both stacks always have the same number of elements.

---

# Question 6 — Evaluate Postfix Expression ⭐⭐⭐

Expression:

```text
2 3 1 * + 9 -
```

Means:

```text
2 + (3 * 1) - 9
```

Answer:

```text
-4
```

### Algorithm

If token is a number:

```text
push
```

If token is an operator:

```text
b = pop()
a = pop()

result = a operator b

push(result)
```

Example:

```text
2 3 1 * + 9 -
```

When `*`:

```text
3 * 1 = 3
```

When `+`:

```text
2 + 3 = 5
```

When `-`:

```text
5 - 9 = -4
```

### Code

```cpp
stack<int> st;

for(char c : expression) {

    if(isdigit(c)) {
        st.push(c - '0');
    }
    else {

        int b = st.top();
        st.pop();

        int a = st.top();
        st.pop();

        if(c == '+')
            st.push(a + b);

        else if(c == '-')
            st.push(a - b);

        else if(c == '*')
            st.push(a * b);

        else if(c == '/')
            st.push(a / b);
    }
}

cout << st.top();
```

For multi-digit numbers, you'd tokenize the expression rather than processing each character.

---

# Question 7 — Infix to Postfix ⭐⭐⭐

This is a **classic DSA lab/viva question**.

Infix:

```text
A + B * C
```

Postfix:

```text
A B C * +
```

Why?

Because multiplication has higher precedence.

Stack is used for operators.

### Important precedence

```text
^       highest
* / %
+ -
```

Basic algorithm:

```text
Operand → directly output

'(' → push

')' → pop until '('

Operator →
    pop higher/equal precedence operators
    push current operator
```

This is worth learning after you understand balanced parentheses.

---

# Question 8 — Next Greater Element ⭐⭐⭐

This is one of the **most important stack patterns**.

Given:

```text
4 5 2 25
```

Find the first greater element to the right.

Output:

```text
5 25 25 -1
```

For `4`:

```text
right side = 5 2 25
first greater = 5
```

For `5`:

```text
25
```

For `2`:

```text
25
```

For `25`:

```text
none → -1
```

### Efficient solution

Use a **monotonic stack**.

```cpp
vector<int> ans(n, -1);
stack<int> st;

for(int i = n - 1; i >= 0; i--) {

    while(!st.empty() && st.top() <= v[i])
        st.pop();

    if(!st.empty())
        ans[i] = st.top();

    st.push(v[i]);
}
```

Complexity:

```text
O(n)
```

This is much better than the obvious nested-loop solution:

```text
O(n²)
```

---

# Question 9 — Previous Greater Element

Same idea, but look to the **left**.

Input:

```text
4 5 2 10
```

Output:

```text
-1 -1 5 -1
```

You can solve it using a monotonic stack.

---

# Question 10 — Next Smaller Element

Input:

```text
4 8 5 2 25
```

Find the first smaller element on the right.

Output:

```text
2 5 2 -1 -1
```

Again:

> **Monotonic stack**

is the key pattern.

---

# Question 11 — Stock Span Problem ⭐⭐⭐

Given stock prices:

```text
100 80 60 70 60 75 85
```

Output:

```text
1 1 1 2 1 4 6
```

For `75`, there are 4 consecutive days:

```text
60 70 60 75
```

where previous prices were smaller/equal.

This is a classic application of a **monotonic stack**.

---

# Question 12 — Largest Rectangle in Histogram ⭐⭐⭐⭐

This is a famous harder stack problem.

Given:

```text
2 1 5 6 2 3
```

Histogram:

```text
    █
    █ █
█   █ █
█ █ █ █ █
```

Maximum rectangle area:

```text
10
```

because:

```text
5 × 2 = 10
```

This uses a monotonic stack.

You should learn this after mastering:

1. Next greater
2. Previous smaller
3. Stock span

---

# Question 13 — Remove Adjacent Duplicates

Input:

```text
abbaca
```

Process:

```text
a
ab
abb → remove bb
aa → remove aa
c
```

Output:

```text
ca
```

Using stack:

```cpp
string ans;

for(char c : s) {

    if(!ans.empty() && ans.back() == c)
        ans.pop_back();
    else
        ans.push_back(c);
}

cout << ans;
```

Here a `string` itself behaves like a stack because:

```cpp
push_back()
pop_back()
back()
```

are available.

---

# Question 14 — Sort a Stack

Given:

```text
3 1 4 2
```

Sort it using another stack.

Expected result:

```text
1 2 3 4
```

This tests whether you understand how to manipulate stacks without using arrays/sorting directly.

---

# Question 15 — Implement Two Stacks in One Array

Suppose:

```cpp
int arr[10];
```

Implement:

```text
Stack 1 → from left
Stack 2 → from right
```

Concept:

```text
Stack 1 → → →     ← ← ← Stack 2
```

Use:

```cpp
int top1 = -1;
int top2 = n;
```

This is a nice **implementation/viva question**.

---

# Question 16 — Implement Queue Using Two Stacks ⭐⭐⭐

This is a very common conceptual question.

Queue:

```text
FIFO
```

Stack:

```text
LIFO
```

Can we create a queue using two stacks?

Yes.

Use:

```cpp
stack<int> s1, s2;
```

For enqueue:

```cpp
s1.push(x);
```

For dequeue:

```cpp
if(s2.empty()) {

    while(!s1.empty()) {
        s2.push(s1.top());
        s1.pop();
    }
}

int x = s2.top();
s2.pop();
```

This teaches how one data structure can be built from another.

---

# ⭐ Most Important Stack Questions for Your Lab

I would rank them like this:

### 🔴 Must Know

```text
1. Implement stack using array
2. Implement stack using linked list
3. Push / Pop / Peek
4. Reverse string
5. Balanced parentheses
6. Valid parentheses
7. Infix → Postfix
8. Postfix evaluation
```

### 🟠 Very Important

```text
9. Next Greater Element
10. Previous Greater Element
11. Next Smaller Element
12. Stock Span
13. Min Stack
14. Queue using two stacks
15. Sort a stack
```

### 🟡 Advanced

```text
16. Largest Rectangle in Histogram
17. Maximal Rectangle
18. Remove adjacent duplicates
19. Decode string using stack
20. Reverse stack recursively
```

---

# 🧠 Patterns You Should Recognize

This is more important than memorizing individual questions.

### Pattern 1 — LIFO

If the problem says something like:

> "Last thing added should be processed first."

Think:

```text
STACK
```

---

### Pattern 2 — Matching

If you see:

```text
()
[]
{}
```

Think:

```text
STACK
```

---

### Pattern 3 — Previous/Next Greater/Smaller

If you see:

```text
Next greater
Previous greater
Next smaller
Previous smaller
Stock span
```

Think:

```text
MONOTONIC STACK
```

---

### Pattern 4 — Expression Problems

If you see:

```text
Infix
Prefix
Postfix
Operator precedence
Expression evaluation
```

Think:

```text
STACK
```

---

# ⭐ Stack Cheat Sheet for C++

For STL:

```cpp
stack<int> st;
```

Add:

```cpp
st.push(x);
```

Top:

```cpp
st.top();
```

Remove:

```cpp
st.pop();
```

Size:

```cpp
st.size();
```

Empty:

```cpp
st.empty();
```

---

## The 10 lines you should know by heart

```cpp
stack<int> st;

st.push(10);
st.push(20);
st.push(30);

cout << st.top();

st.pop();

cout << st.top();

cout << st.size();

if(st.empty())
    cout << "Empty";
```

---

# 🎯 Recommended Practice Order

For your C++ → DSA preparation, I would **not jump directly to histogram or monotonic stacks**.

Practice in this exact order:

```text
                 STACK
                   ↓
          ┌────────┴────────┐
          ↓                 ↓
     Implementation      STL stack
          ↓                 ↓
     Push/Pop/Peek      Basic operations
          └────────┬────────┘
                   ↓
          Reverse String
                   ↓
        Balanced Parentheses
                   ↓
        Infix → Postfix
                   ↓
       Postfix Evaluation
                   ↓
           Min Stack
                   ↓
       Next Greater Element
                   ↓
       Previous Smaller
                   ↓
          Stock Span
                   ↓
    Largest Rectangle Histogram
```

If you can comfortably solve **up through Next Greater Element + Stock Span**, you will have a strong foundation for stack questions in a typical DSA coding lab.

The next natural topic after Stack is **Queue**, where I'll show you **normal Queue, Circular Queue, Deque, Priority Queue, implementations using arrays/linked lists, STL syntax, and the expected coding-round questions**.


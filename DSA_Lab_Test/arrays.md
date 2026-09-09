Absolutely. Since your goal is **C++ specifically for DSA/competitive programming**, you should learn arrays and vectors not just as syntax, but also the **STL functions and patterns that commonly appear in coding rounds**.

# 1. Array in C++

An array stores multiple values of the **same data type** in continuous memory.

### Basic declaration

```cpp
int arr[5];
```

This creates an array of 5 integers.

You can initialize it:

```cpp
int arr[5] = {10, 20, 30, 40, 50};
```

Or let C++ determine the size:

```cpp
int arr[] = {10, 20, 30, 40, 50};
```

Size = 5.

### Access elements

Array indexing starts from **0**.

```cpp
cout << arr[0];   // 10
cout << arr[2];   // 30
cout << arr[4];   // 50
```

Modify:

```cpp
arr[2] = 100;
```

Now:

```text
10 20 100 40 50
```

---

# 2. Taking Array Input

Most important pattern for DSA:

```cpp
int n;
cin >> n;

int arr[n];

for(int i = 0; i < n; i++) {
    cin >> arr[i];
}
```

However, **variable-length arrays like `int arr[n]` are not standard C++**. For DSA, prefer `vector`.

For a fixed maximum size:

```cpp
int arr[1000];

int n;
cin >> n;

for(int i = 0; i < n; i++) {
    cin >> arr[i];
}
```

---

# 3. Printing an Array

```cpp
for(int i = 0; i < n; i++) {
    cout << arr[i] << " ";
}
```

Output:

```text
10 20 30 40 50
```

---

# 4. Important Array Operations

Suppose:

```cpp
int arr[] = {10, 20, 30, 40, 50};
int n = 5;
```

## Access

```cpp
arr[2]
```

Time:

```text
O(1)
```

---

## Update

```cpp
arr[2] = 100;
```

Time:

```text
O(1)
```

---

## Traversal

```cpp
for(int i = 0; i < n; i++)
    cout << arr[i];
```

Time:

```text
O(n)
```

---

## Search

Linear search:

```cpp
int x;
cin >> x;

bool found = false;

for(int i = 0; i < n; i++) {
    if(arr[i] == x) {
        found = true;
        break;
    }
}
```

Time:

```text
O(n)
```

---

# 5. Finding Maximum and Minimum

### Maximum

```cpp
int mx = arr[0];

for(int i = 1; i < n; i++) {
    mx = max(mx, arr[i]);
}

cout << mx;
```

### Minimum

```cpp
int mn = arr[0];

for(int i = 1; i < n; i++) {
    mn = min(mn, arr[i]);
}
```

Time:

```text
O(n)
```

---

# 6. Reverse an Array

### Using loop

```cpp
int i = 0;
int j = n - 1;

while(i < j) {
    swap(arr[i], arr[j]);
    i++;
    j--;
}
```

Example:

```text
Before:
1 2 3 4 5

After:
5 4 3 2 1
```

### Using STL

```cpp
reverse(arr, arr + n);
```

This is very useful in coding rounds.

---

# 7. Sort an Array

```cpp
sort(arr, arr + n);
```

Ascending:

```text
1 2 3 4 5
```

Descending:

```cpp
sort(arr, arr + n, greater<int>());
```

Result:

```text
5 4 3 2 1
```

Time complexity:

```text
O(n log n)
```

---

# 8. Important STL Functions for Arrays

You should remember these:

```cpp
sort(arr, arr+n);
reverse(arr, arr+n);
swap(a, b);
max(a, b);
min(a, b);
```

Also:

```cpp
*max_element(arr, arr+n)
```

gives maximum.

```cpp
*min_element(arr, arr+n)
```

gives minimum.

Example:

```cpp
int arr[] = {4, 2, 8, 1, 9};

cout << *max_element(arr, arr+5);
```

Output:

```text
9
```

---

# 9. What is Vector?

A `vector` is basically a **dynamic array**.

Normal array:

```cpp
int arr[100];
```

Its size is fixed.

Vector:

```cpp
vector<int> v;
```

Its size can grow/shrink dynamically.

You need:

```cpp
#include <vector>
```

Usually in competitive programming you'll simply use:

```cpp
#include <bits/stdc++.h>
using namespace std;
```

---

# 10. Creating Vectors

Empty vector:

```cpp
vector<int> v;
```

Vector of size 5:

```cpp
vector<int> v(5);
```

Initially:

```text
0 0 0 0 0
```

Vector of size 5 containing 10:

```cpp
vector<int> v(5, 10);
```

Result:

```text
10 10 10 10 10
```

Initialize directly:

```cpp
vector<int> v = {10, 20, 30, 40, 50};
```

---

# 11. Taking Vector Input

Very common:

```cpp
int n;
cin >> n;

vector<int> v(n);

for(int i = 0; i < n; i++) {
    cin >> v[i];
}
```

---

# 12. Adding Elements — `push_back()`

```cpp
vector<int> v;

v.push_back(10);
v.push_back(20);
v.push_back(30);
```

Vector becomes:

```text
10 20 30
```

This is one of the **most important vector operations**.

---

# 13. Removing Last Element — `pop_back()`

```cpp
v.pop_back();
```

Before:

```text
10 20 30
```

After:

```text
10 20
```

---

# 14. Size

```cpp
cout << v.size();
```

Example:

```cpp
vector<int> v = {10,20,30};

cout << v.size();
```

Output:

```text
3
```

---

# 15. Accessing Vector Elements

```cpp
cout << v[0];
```

or:

```cpp
cout << v.at(0);
```

For DSA, you'll mostly use:

```cpp
v[i]
```

---

# 16. First and Last Element

```cpp
v.front();
```

First element.

```cpp
v.back();
```

Last element.

Example:

```cpp
vector<int> v = {10,20,30,40};

cout << v.front();  // 10
cout << v.back();   // 40
```

---

# 17. Insert in Vector

You can insert using an iterator.

```cpp
vector<int> v = {10,20,40,50};

v.insert(v.begin() + 2, 30);
```

Result:

```text
10 20 30 40 50
```

But remember:

**Insertion in the middle is O(n)** because elements have to shift.

---

# 18. Delete from Vector

```cpp
v.erase(v.begin() + 2);
```

Example:

```text
10 20 30 40 50
```

After:

```text
10 20 40 50
```

Again, middle deletion is generally:

```text
O(n)
```

---

# 19. Clear Entire Vector

```cpp
v.clear();
```

After this:

```cpp
v.empty()
```

will return `true`.

---

# 20. Check Whether Vector is Empty

```cpp
if(v.empty()) {
    cout << "Empty";
}
```

---

# 21. Sort Vector

Very important.

```cpp
sort(v.begin(), v.end());
```

Descending:

```cpp
sort(v.begin(), v.end(), greater<int>());
```

---

# 22. Reverse Vector

```cpp
reverse(v.begin(), v.end());
```

---

# 23. Find an Element

```cpp
auto it = find(v.begin(), v.end(), 30);
```

If found:

```cpp
if(it != v.end()) {
    cout << "Found";
}
```

If you want its index:

```cpp
int index = it - v.begin();
```

---

# 24. Count an Element

```cpp
int c = count(v.begin(), v.end(), 5);
```

Example:

```cpp
vector<int> v = {1,5,2,5,5,3};

cout << count(v.begin(), v.end(), 5);
```

Output:

```text
3
```

---

# 25. Maximum and Minimum in Vector

```cpp
int mx = *max_element(v.begin(), v.end());
int mn = *min_element(v.begin(), v.end());
```

---

# 26. Array vs Vector

| Feature       | Array            | Vector                       |
| ------------- | ---------------- | ---------------------------- |
| Size          | Fixed            | Dynamic                      |
| Access        | O(1)             | O(1)                         |
| Add at end    | Not dynamic      | `push_back()`                |
| Remove last   | Not directly     | `pop_back()`                 |
| Insert middle | O(n)             | O(n)                         |
| Delete middle | O(n)             | O(n)                         |
| Sort          | `sort(a,a+n)`    | `sort(v.begin(),v.end())`    |
| Reverse       | `reverse(a,a+n)` | `reverse(v.begin(),v.end())` |

### For DSA

I strongly recommend becoming **very comfortable with `vector`**, because many coding-round problems use vectors instead of raw arrays.

---

# 27. Most Important Array DSA Questions

Now let's move to the part that is much more relevant for your **DSA coding lab/coding round**.

Don't just memorize solutions. These questions cover important patterns.

---

## Question 1 — Find Maximum Element

### Problem

Given an array, find the maximum element.

Example:

```text
Input:
5
10 4 25 7 2

Output:
25
```

### Solution

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    cin >> n;

    vector<int> v(n);

    for(int i = 0; i < n; i++)
        cin >> v[i];

    int mx = v[0];

    for(int i = 1; i < n; i++) {
        mx = max(mx, v[i]);
    }

    cout << mx;

    return 0;
}
```

Complexity:

```text
Time: O(n)
Space: O(n)   // because vector stores input
```

---

# Question 2 — Find Second Largest Element

This is **very common**.

Input:

```text
6
10 5 20 8 20 15
```

Output:

```text
15
```

Here we want the **second distinct largest** element.

### Solution

```cpp
int largest = INT_MIN;
int second = INT_MIN;

for(int x : v) {

    if(x > largest) {
        second = largest;
        largest = x;
    }
    else if(x > second && x != largest) {
        second = x;
    }
}

cout << second;
```

### Important concept

Don't simply sort unless you are specifically allowed to.

This solution is:

```text
O(n)
```

Sorting would be:

```text
O(n log n)
```

---

# Question 3 — Reverse Array

Input:

```text
1 2 3 4 5
```

Output:

```text
5 4 3 2 1
```

Solution:

```cpp
int i = 0;
int j = n - 1;

while(i < j) {
    swap(v[i], v[j]);
    i++;
    j--;
}
```

This teaches the **two-pointer technique**.

---

# Question 4 — Check if Array is Sorted

Input:

```text
1 2 3 4 5
```

Output:

```text
YES
```

Solution:

```cpp
bool sorted = true;

for(int i = 1; i < n; i++) {
    if(v[i] < v[i-1]) {
        sorted = false;
        break;
    }
}

if(sorted)
    cout << "YES";
else
    cout << "NO";
```

Time:

```text
O(n)
```

---

# Question 5 — Remove Duplicates from Sorted Array

Input:

```text
1 1 2 2 2 3 4 4
```

Output:

```text
1 2 3 4
```

This is an important **two-pointer problem**.

```cpp
int j = 0;

for(int i = 1; i < n; i++) {

    if(v[i] != v[j]) {
        j++;
        v[j] = v[i];
    }
}

int newSize = j + 1;

for(int i = 0; i < newSize; i++)
    cout << v[i] << " ";
```

---

# Question 6 — Find Missing Number

Given numbers from `0` to `n`, one number is missing.

Example:

```text
Input:
0 1 2 4 5

Output:
3
```

### XOR solution

```cpp
int ans = 0;

for(int i = 0; i <= n; i++)
    ans ^= i;

for(int x : v)
    ans ^= x;

cout << ans;
```

Why does this work?

Because:

```text
x ^ x = 0
x ^ 0 = x
```

So all existing numbers cancel out, leaving the missing number.

This is a **very good coding-round question**.

---

# Question 7 — Move All Zeros to End

Input:

```text
0 1 0 3 12
```

Output:

```text
1 3 12 0 0
```

### Solution

```cpp
int j = 0;

for(int i = 0; i < n; i++) {

    if(v[i] != 0) {
        swap(v[i], v[j]);
        j++;
    }
}
```

Again, this teaches **two pointers**.

---

# Question 8 — Two Sum

One of the most famous array problems.

Given:

```text
2 7 11 15
```

Target:

```text
9
```

Find two numbers whose sum is 9.

Answer:

```text
2 + 7 = 9
```

### Hash map solution

```cpp
unordered_map<int, int> mp;

for(int i = 0; i < n; i++) {

    int required = target - v[i];

    if(mp.find(required) != mp.end()) {
        cout << mp[required] << " " << i;
        return 0;
    }

    mp[v[i]] = i;
}
```

This introduces an extremely important DSA structure:

```text
unordered_map
```

Average complexity:

```text
O(n)
```

---

# Question 9 — Maximum Subarray Sum

This is **extremely important**.

Input:

```text
-2 1 -3 4 -1 2 1 -5 4
```

Maximum subarray:

```text
4 -1 2 1
```

Sum:

```text
6
```

### Kadane's Algorithm

```cpp
int current = v[0];
int best = v[0];

for(int i = 1; i < n; i++) {

    current = max(v[i], current + v[i]);

    best = max(best, current);
}

cout << best;
```

Complexity:

```text
O(n)
```

You **must know Kadane's algorithm** for array-based DSA.

---

# Question 10 — Stock Buy and Sell

Given stock prices:

```text
7 1 5 3 6 4
```

Buy at:

```text
1
```

Sell at:

```text
6
```

Profit:

```text
5
```

### Solution

```cpp
int minPrice = v[0];
int maxProfit = 0;

for(int i = 1; i < n; i++) {

    maxProfit = max(maxProfit, v[i] - minPrice);

    minPrice = min(minPrice, v[i]);
}

cout << maxProfit;
```

Complexity:

```text
O(n)
```

---

# Question 11 — Rotate Array by One Position

Input:

```text
1 2 3 4 5
```

Right rotation:

```text
5 1 2 3 4
```

```cpp
int last = v[n-1];

for(int i = n-1; i > 0; i--) {
    v[i] = v[i-1];
}

v[0] = last;
```

---

# Question 12 — Rotate Array by K Positions

Example:

```text
1 2 3 4 5 6 7
```

Rotate right by:

```text
3
```

Result:

```text
5 6 7 1 2 3 4
```

Efficient solution using reverse:

```cpp
k = k % n;

reverse(v.begin(), v.end());

reverse(v.begin(), v.begin() + k);

reverse(v.begin() + k, v.end());
```

This is a classic interview/coding-round technique.

---

# Question 13 — Find Duplicate Number

Example:

```text
1 3 4 2 2
```

Output:

```text
2
```

A simple solution using a set:

```cpp
unordered_set<int> s;

for(int x : v) {

    if(s.count(x)) {
        cout << x;
        break;
    }

    s.insert(x);
}
```

Later, you should learn the **Floyd cycle detection** solution, which gives:

```text
O(n) time
O(1) extra space
```

That is a more advanced interview solution.

---

# Question 14 — Majority Element

Find the element appearing more than `n/2` times.

Example:

```text
2 2 1 1 1 2 2
```

Answer:

```text
2
```

### Boyer-Moore Voting Algorithm

```cpp
int candidate = 0;
int count = 0;

for(int x : v) {

    if(count == 0)
        candidate = x;

    if(x == candidate)
        count++;
    else
        count--;
}

cout << candidate;
```

Very important algorithm.

---

# Question 15 — Find Intersection of Two Arrays

Example:

```text
A = 1 2 2 3
B = 2 2 4
```

Intersection:

```text
2 2
```

Depending on the exact question, you can solve this using:

* sorting + two pointers
* hash map
* set

This is an important pattern to understand rather than memorize one implementation.

---

# Question 16 — Merge Two Sorted Arrays

Example:

```text
A = 1 3 5
B = 2 4 6
```

Output:

```text
1 2 3 4 5 6
```

### Two-pointer solution

```cpp
int i = 0;
int j = 0;

vector<int> ans;

while(i < n && j < m) {

    if(a[i] < b[j]) {
        ans.push_back(a[i]);
        i++;
    }
    else {
        ans.push_back(b[j]);
        j++;
    }
}

while(i < n) {
    ans.push_back(a[i]);
    i++;
}

while(j < m) {
    ans.push_back(b[j]);
    j++;
}
```

Complexity:

```text
O(n + m)
```

---

# Question 17 — Find Pair With Given Difference

Example:

```text
1 5 3 4 2
```

Difference:

```text
2
```

Pairs include:

```text
1,3
3,5
2,4
```

Can be solved using:

```text
sorting + two pointers
```

or

```text
unordered_set
```

---

# Question 18 — Product of Array Except Self

Example:

```text
1 2 3 4
```

Output:

```text
24 12 8 6
```

The important restriction in the standard version is:

> Don't use division.

This teaches the **prefix/suffix technique**.

---

# Question 19 — Subarray With Given Sum

Example:

```text
1 4 20 3 10 5
```

Target:

```text
33
```

Subarray:

```text
20 3 10
```

For positive numbers, a **sliding window** works.

For arbitrary integers, prefix sum + hash map is generally needed.

This distinction is important in coding rounds.

---

# Question 20 — Maximum Consecutive 1s

Input:

```text
1 1 0 1 1 1
```

Output:

```text
3
```

Solution:

```cpp
int current = 0;
int best = 0;

for(int x : v) {

    if(x == 1) {
        current++;
        best = max(best, current);
    }
    else {
        current = 0;
    }
}

cout << best;
```

---

# What I Would Expect in an IIT DSA Coding Lab

Rather than preparing 100 random array questions, I would prepare these **patterns**:

| Pattern         | Questions you should master            |
| --------------- | -------------------------------------- |
| Basic traversal | max, min, sum, search                  |
| Two pointers    | reverse, remove duplicates, move zeros |
| Sorting         | sort, merge arrays, pair problems      |
| Hashing         | Two Sum, frequency, duplicates         |
| Prefix Sum      | subarray sum, range sum                |
| Sliding Window  | longest/shortest subarray              |
| Kadane          | maximum subarray                       |
| Binary Search   | search, first/last occurrence          |
| Rotation        | rotate by K                            |
| Greedy          | stock buy/sell                         |
| XOR             | missing number, unique element         |
| Voting          | majority element                       |
| Prefix/Suffix   | product except self                    |

### ⭐ Priority for your coding round

If your lab gives you limited preparation time, I would prioritize:

1. **Maximum / minimum**
2. **Second largest**
3. **Reverse array**
4. **Check sorted**
5. **Remove duplicates**
6. **Move zeros**
7. **Rotate array**
8. **Two Sum**
9. **Missing number**
10. **Find duplicate**
11. **Majority element**
12. **Maximum subarray — Kadane**
13. **Stock buy/sell**
14. **Merge two sorted arrays**
15. **Subarray sum**
16. **Longest subarray**
17. **Product except self**
18. **Binary search variants**

And for your C++ transition from Python, pay special attention to **`vector`, iterators, `sort`, `reverse`, `lower_bound`, `upper_bound`, `unordered_map`, `unordered_set`, `pair`, and range-based `for` loops**. These will make your C++ DSA code much shorter and more natural.

If you want, we can continue your C++ DSA course **topic-by-topic**: next I can teach you **`vector` completely (all important methods, iterators, 2D vectors, passing vectors to functions, references, `pair`, sorting with custom comparators, and 15–20 practice questions)** before moving to strings, stack, queue, map, set, linked list, trees, graphs, DP, etc.


## 1. Local Beam Search

python
```
import random

def heuristic(x):
    return -(x - 10) ** 2

def local_beam_search(k, max_iterations):
    states = [random.randint(0, 20) for _ in range(k)]

    for _ in range(max_iterations):
        candidates = []

        for state in states:
            candidates.extend([
                max(0, state - 1),
                min(20, state + 1)
            ])

        states = sorted(
            set(candidates),
            key=heuristic,
            reverse=True
        )[:k]

        if 10 in states:
            return 10

    return max(states, key=heuristic)

k = int(input("Enter beam width: "))
max_iterations = int(input("Enter maximum iterations: "))

result = local_beam_search(k, max_iterations)
print("Best solution:", result)
print("Heuristic value:", heuristic(result))
```

## 2. Simulated Annealing

python
```
import random
import math

def objective(x):
    return -(x - 10) ** 2

def simulated_annealing(initial, temperature, cooling_rate, max_iterations):
    current = initial
    best = current

    for _ in range(max_iterations):
        if temperature <= 0.001:
            break

        neighbor = current + random.choice([-1, 1])
        neighbor = max(0, min(20, neighbor))

        delta = objective(neighbor) - objective(current)

        if delta > 0 or random.random() < math.exp(delta / temperature):
            current = neighbor

        if objective(current) > objective(best):
            best = current

        temperature *= cooling_rate

    return best

initial = int(input("Enter initial state (0-20): "))
temperature = float(input("Enter initial temperature: "))
cooling_rate = float(input("Enter cooling rate (0-1): "))
max_iterations = int(input("Enter maximum iterations: "))

result = simulated_annealing(initial, temperature, cooling_rate, max_iterations)
print("Best solution:", result)
print("Objective value:", objective(result))
```

## 3. Genetic Algorithm

python
```
import random

def fitness(x):
    return -(x - 10) ** 2 + 100

def genetic_algorithm(population_size, generations, mutation_rate):
    population = [random.randint(0, 20) for _ in range(population_size)]

    for _ in range(generations):
        population.sort(key=fitness, reverse=True)

        if fitness(population[0]) == 100:
            break

        new_population = population[:2]

        while len(new_population) < population_size:
            parent1 = random.choice(population[:max(1, population_size // 2)])
            parent2 = random.choice(population[:max(1, population_size // 2)])

            child = (parent1 + parent2) // 2

            if random.random() < mutation_rate:
                child += random.choice([-1, 1])

            child = max(0, min(20, child))
            new_population.append(child)

        population = new_population

    best = max(population, key=fitness)
    return best

population_size = int(input("Enter population size: "))
generations = int(input("Enter number of generations: "))
mutation_rate = float(input("Enter mutation rate (0-1): "))

result = genetic_algorithm(population_size, generations, mutation_rate)
print("Best solution:", result)
print("Fitness value:", fitness(result))
```

## 4. A\* Search

python
```
import heapq

def a_star(graph, start, goal, heuristic):
    priority_queue = [(heuristic[start], 0, start, [start])]
    visited = set()

    while priority_queue:
        f, g, node, path = heapq.heappop(priority_queue)

        if node == goal:
            return path, g

        if node in visited:
            continue

        visited.add(node)

        for neighbor, cost in graph.get(node, []):
            if neighbor not in visited:
                new_g = g + cost
                new_f = new_g + heuristic[neighbor]
                heapq.heappush(
                    priority_queue,
                    (new_f, new_g, neighbor, path + [neighbor])
                )

    return None, float("inf")

graph = {
    'A': [('B', 1), ('C', 4)],
    'B': [('A', 1), ('C', 2), ('D', 5)],
    'C': [('A', 4), ('B', 2), ('D', 1)],
    'D': [('B', 5), ('C', 1)]
}

heuristic = {
    'A': 4,
    'B': 3,
    'C': 1,
    'D': 0
}

start = input("Enter start node (A/B/C/D): ").upper()
goal = input("Enter goal node (A/B/C/D): ").upper()

path, cost = a_star(graph, start, goal, heuristic)

if path:
    print("Path:", " -> ".join(path))
    print("Total cost:", cost)
else:
    print("No path found")
```

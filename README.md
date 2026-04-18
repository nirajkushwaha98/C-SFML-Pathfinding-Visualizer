# 🚀 Pathfinding Visualizer (C++ + SFML)

## 📌 Overview

This project is a **Pathfinding Visualizer** built using **C++** and the **SFML (Simple and Fast Multimedia Library)**.
It demonstrates how pathfinding algorithms work on a 2D grid by visualizing each step in real-time.

The application allows users to interact with the grid, place obstacles, and observe how the algorithm finds the shortest path between two points.

---

## 🎯 Objectives

* Understand graph-based pathfinding algorithms
* Visualize traversal step-by-step
* Strengthen concepts of **Data Structures (Queue, Graph, BFS)**
* Build a GUI-based project using C++

---

## 🧠 Concepts Used

* Graph Representation (Grid as Graph)
* Breadth First Search (BFS)
* Queue (FIFO)
* Path Reconstruction using Parent Mapping
* Event Handling (Mouse & Keyboard)
* 2D Graphics Rendering

---

## 🛠️ Tech Stack

* Language: **C++**
* Graphics Library: **SFML**
* STL: vector, queue, map

---

## ⚙️ Features

* Interactive 2D grid visualization
* Add/remove obstacles using mouse
* Set start and end nodes
* Real-time BFS traversal visualization
* Shortest path highlighting
* Grid reset functionality

---

## 🎮 Controls

| Action          | Key / Input       |
| --------------- | ----------------- |
| Add obstacle    | Left Mouse Click  |
| Remove obstacle | Right Mouse Click |
| Set Start Node  | Press `S`         |
| Set End Node    | Press `E`         |
| Run Algorithm   | Press `Space`     |
| Reset Grid      | Press `R`         |

---

## 🧮 Algorithm Explanation

### Breadth First Search (BFS)

* BFS explores nodes level by level
* It guarantees the **shortest path** in an unweighted graph
* A **queue** is used to process nodes in FIFO order

### Steps:

1. Start from the source node
2. Explore all neighbors
3. Mark visited nodes
4. Store parent of each node
5. Reconstruct path from destination

---

## 📂 Project Structure

```
project/
│── main.cpp
│── grid.h / grid.cpp
│── pathfinding.h / pathfinding.cpp
```

---

## ▶️ How to Run

### 1. Install SFML

Download SFML from the official website and set up include and library paths.

### 2. Compile

```
g++ main.cpp -o app -lsfml-graphics -lsfml-window -lsfml-system
```

### 3. Run

```
./app
```

---

## 🚀 Future Improvements

* Add Dijkstra Algorithm
* Add A* (A-Star) Algorithm
* Step-by-step speed control
* Adjustable grid size
* Better UI design

---

## 💀 Limitations

* Works only for unweighted graphs (BFS)
* Limited grid size
* Basic UI

---

## 📌 Conclusion

This project demonstrates how fundamental data structures and algorithms can be applied to solve real-world problems.
It also highlights the integration of **DSA concepts with graphical visualization using C++**.

---

## 🙌 Acknowledgment

Inspired by algorithm visualization tools and academic learning in Data Structures.

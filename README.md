# 🏭 Multithreaded Factory Simulator

Academic project developed for the **Operating Systems** course (**Universidad Carlos III de Madrid**). The project simulates a **multithreaded manufacturing system** implementing the classical **Producer–Consumer** pattern using **POSIX Threads**, **mutexes**, **condition variables**, and **semaphores**.

## 📌 Overview

The simulator models a factory composed of multiple independent production lines running concurrently.

Each production line consists of:

* A **Producer** thread that generates products.
* A **Consumer** thread that processes products.
* A shared **thread-safe circular buffer** representing the conveyor belt.

The project focuses on synchronization, concurrent execution, and safe resource sharing while preventing race conditions and deadlocks.

---

## 🏗️ System Architecture

The factory is organized in a hierarchical structure:

```text
Factory Manager
      │
      ├── Process Manager (Production Line 1)
      │         ├── Producer
      │         ├── Conveyor Belt (Circular Buffer)
      │         └── Consumer
      │
      ├── Process Manager (Production Line 2)
      │         ├── Producer
      │         ├── Conveyor Belt
      │         └── Consumer
      │
      └── ...
```

The architecture includes:

* Factory manager responsible for configuration and scheduling.
* Independent production-line managers.
* Producer and consumer worker threads.
* Thread-safe circular buffer shared between threads.
* Semaphore-based synchronization between production lines.

---

## ⚙️ Synchronization Mechanisms

The simulator combines several synchronization primitives provided by **POSIX Threads**.

| Component                   | Purpose                                        |
| --------------------------- | ---------------------------------------------- |
| **POSIX Threads (pthread)** | Concurrent execution                           |
| **Mutexes**                 | Mutual exclusion on shared resources           |
| **Condition Variables**     | Blocking producers and consumers when required |
| **Semaphores**              | Synchronization between production lines       |
| **Circular Buffer**         | Thread-safe shared queue                       |

These mechanisms guarantee safe communication between concurrent threads while avoiding race conditions.

---

## 🔄 Execution Workflow

Each production line follows the classical Producer–Consumer workflow:

1. The Factory Manager reads the configuration file.
2. Production lines are created according to the specified schedule.
3. Each Process Manager launches one Producer and one Consumer thread.
4. Producers insert items into the shared conveyor belt.
5. Consumers remove items from the conveyor belt.
6. Synchronization primitives coordinate concurrent access to shared resources.

---

## 📂 Project Structure

```text
├── factory_manager.c      # Factory initialization and scheduling
├── process_manager.c      # Production-line management
├── queue.c                # Thread-safe circular buffer
├── queue.h
├── Makefile
└── README.md
```

---

## 🚀 Features

* Multithreaded production-line simulation
* Producer–Consumer implementation
* Thread-safe circular buffer
* Mutex and condition variable synchronization
* Semaphore-based scheduling
* Dynamic configuration through input files
* Error detection and recovery
* Modular architecture

---

## 💻 Build & Run

Compile the project:

```bash
make
```

Run the simulator:

```bash
./factory_manager configuration.txt
```

Example output:

```text
[OK] Process manager created.
[OK] Conveyor belt initialized.
[OK] Producer generated element 0.
[OK] Consumer processed element 0.
...
[OK] Factory simulation completed.
```

---

## 🛡️ Error Handling

The simulator performs extensive input validation and runtime error handling.

Supported error scenarios include:

* Invalid configuration files
* Missing command-line arguments
* Negative or inconsistent parameters
* Invalid production-line definitions
* Resource allocation failures

If a production line fails, the Factory Manager detects the error and continues executing the remaining production lines whenever possible.

---

## ✅ Testing

The implementation was validated under multiple scenarios, including:

* Single production line
* Multiple concurrent production lines
* Small and large conveyor buffers
* Maximum supported number of production lines
* Invalid configuration files
* Incorrect input parameters
* Concurrent execution stress tests

Special attention was given to preventing race conditions, deadlocks, and memory leaks.

---

## 🛠️ Technologies

`C` · `POSIX Threads (pthread)` · `Mutexes` · `Condition Variables` · `Semaphores` · `Circular Buffers` · `Concurrent Programming` · `Linux`

---

## 🎯 Learning Outcomes

During the development of this project, the following concepts were applied:

* Concurrent Programming
* Producer–Consumer Pattern
* POSIX Threads
* Thread Synchronization
* Mutexes and Condition Variables
* Semaphores
* Shared Memory Synchronization
* Operating Systems
* Deadlock Prevention
* Thread-Safe Data Structures

---

## 👥 Authors

* Diego Calles Duque
* Tristán Serrano Álvarez

Project developed for the **Operating Systems** course, **Bachelor's Degree in Computer Engineering**, Universidad Carlos III de Madrid (2024–2025).

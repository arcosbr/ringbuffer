# Ring Buffer in C

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C](https://img.shields.io/badge/language-C-blue.svg)
![GCC](https://img.shields.io/badge/compiler-GCC-orange.svg)

## Introduction

This project demonstrates a thread-safe ring buffer implementation in C, showcasing how to enqueue and dequeue elements using POSIX threads (`pthread`) with non-blocking timers. It's an excellent reference for understanding circular buffer mechanics and multithreaded programming in C.

## Features

- **Thread-Safe Ring Buffer:** Implements a circular buffer that safely handles concurrent access from multiple threads.
- **Non-Blocking Timers:** Utilizes separate threads to enqueue and dequeue elements at 1-second intervals without blocking the main execution flow.
- **Comprehensive Error Handling:** Provides detailed status codes to handle various buffer states and potential errors.
- **Modular Design:** Separates the ring buffer implementation (`ring_buffer.h` and `ring_buffer.c`) from the application logic (`main.c`) for clarity and reusability.

## Prerequisites

Before you begin, ensure you have met the following requirements:

- **Operating System:** Windows
- **Compiler:** GCC (via MSYS2)
- **Build Tools:** Make

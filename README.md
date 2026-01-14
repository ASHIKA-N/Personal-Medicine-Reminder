# Personal Medication Reminder (C++ / DSA Project)

A console-based medication management system built using core **Data Structures & Algorithms** concepts.  
The system allows users to securely manage medicine schedules, quantities, expiry reminders, and undo/redo operations.

---

## Features

### 🔐 User Authentication
- Register & Login system  
- Case-insensitive usernames  
- Passwords stored using hashing  
- Duplicate username prevention  
- Per-user data isolation  

### 💊 Medicine Management
- Insert new medicines with time & quantity  
- Delete by name + time  
- Delete all medicines of a specific name  
- Modify existing medicine details  
- Search for medicines by name  
- View all scheduled medicines  

### ⏪ Undo / Redo
- Undo last operation  
- Redo undone operations  
- Implemented using **Stack**  

### 📅 Expiry & Reminder System
- Expiry reminder before exit  
- Daily medicine queue  
- Quantity threshold alerts  
- Custom reminder quantity per user  

### 📁 File Handling
- Persistent storage per user  
- Automatic data load on login  
- Safe commit on exit  
- Option to delete all user data  

---

## Data Structures Used

- **Singly Linked List** – Medicine storage  
- **Stack** – Undo/Redo operations  
- **Queue** – Daily medicine reminders  
- **File I/O** – Persistent storage  
- **Hashing** – Password security  

---

## Project Structure
```text
/cppfolder
├── Expiry.cpp
├── FileHandler.cpp
├── Login.cpp
├── Main.cpp
├── Queue.cpp
└── Stack.cpp

/hppfolder
├── DS.hpp
├── Expiry.hpp
├── FileHandler.hpp
├── Login.hpp
├── Queue.hpp
├── Stack.hpp
└── Struct.hpp

```

---


## How to Run

```text
1. Compile the program:
g++ -std=c++20 ./cppfolder/Main.cpp ./cppfolder/Stack.cpp ./cppfolder/Queue.cpp ./cppfolder/Expiry.cpp ./cppfolder/FileHandler.cpp ./cppfolder/Login.cpp -I./hppfolder -o Main.exe

2. Run the Executable
/Main.exe

```

## Notes
1. This project focuses on DSA implementation, not production-level security
2. Password hashing is for academic purposes
3. Designed for console-based interaction

# Authors
- Ashika N
- Marcus Abraham T
- Mokshit Saran P
- Dhayanithi M

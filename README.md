# Student Management System (C++17)
This is a C++ project that I made to manage student records through a console menu. It lets you add, search, update, display and delete student details. The records are saved in a local file, so they are still available when the program is run again.

## Files
| File | Purpose |
|---|---|
| `student_mgmt.cpp` | Main source code for the project |
| `Makefile` | Commands to build, test and clean the program |
| `students.db` | Sample student records |
| `tests/session1.in` | Input for the first test session |
| `tests/session2.in` | Input for testing data after restarting |
| `tests/edge_cases.in` | Input for testing invalid entries |
| `terminal_transcript.txt` | Saved terminal output from testing |

## How to Build and Run
To compile the program, run:

```bash
make
```

Then start it with:

```bash
./student_mgmt
```

By default, the program uses `students.db`. You can also provide another file:

```bash
./student_mgmt other_file.db
```

Other available commands:

```bash
make test     # Run the test sessions
make debug    # Build with address and undefined behaviour sanitizers
make clean    # Remove generated files
```

If you don't have `make`, you can compile it directly using:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic student_mgmt.cpp -o student_mgmt
```

## Features
- Add students with roll number, name, department and marks.
- Search using a roll number or part of a student's name.
- Update student details. Pressing Enter keeps the existing value.
- Display all students in a table, along with their grades and class average.
- Delete a student after confirmation.
- Save changes to a file and load them when the program starts.

## Input Validation

The program checks the entered values before saving them.

- Roll numbers must be between 1 and 999999 and cannot be duplicated.
- Names can contain letters, spaces, dots, hyphens and apostrophes.
- Department names can also contain digits and `&`.
- Marks must be between 0 and 100 and are stored up to two decimal places.
- Invalid menu choices and incorrect numeric inputs are handled.

### Grading System
| Marks | Grade |
|---|---|
| 90 and above | A+ |
| 80–89.99 | A |
| 70–79.99 | B |
| 60–69.99 | C |
| 50–59.99 | D |
| Below 50 | F |

## How the Code Works
The project is divided into a few main parts:

- **Student:** Stores a student's roll number, name, department and marks.
- **Console:** Handles user input and checks whether the entered values are valid.
- **App:** Displays the menu and performs operations such as adding, searching, updating and deleting records.
- **StudentDatabase:** Stores the records, searches for students and handles loading and saving.

The records are kept sorted by roll number, which makes searching by roll number faster.

## Saving Student Records
The program stores each student on a separate line in `students.db`. The fields are separated by `|`.

```text
roll|name|department|marks
``
The program loads the records when it starts and saves changes to the file. It also checks the data while loading and skips invalid records instead of stopping immediately.

## Testing
I included test inputs to check the main features and some invalid cases.

The tests cover:

- Adding and searching for students.
- Updating and deleting records.
- Preventing duplicate roll numbers.
- Rejecting marks outside the valid range.
- Checking that records remain saved after restarting the program.
- Handling invalid input and incorrect database entries.

The `Makefile` also includes a debug build using AddressSanitizer and UndefinedBehaviorSanitizer to help identify memory and runtime problems.

## Sample Output
The program displays student records in a table with their marks and grades. It also shows the total number of students and the class average.

Add screenshots of your own program output here.

## Limitations
- Names are limited to English letters and the supported punctuation.
- The `|` character cannot be used in text fields because it separates the data.
- The database file is rewritten whenever a change is saved, so this approach is better suited to small projects.
- A C++17-compatible compiler is required.

## Author
Vanshika Chauhan

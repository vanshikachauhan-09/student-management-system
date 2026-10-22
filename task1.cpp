#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Student {
    int roll = 0;
    string name;
    string dept;
    float marks = 0;
};

vector<Student> students;
string fileName = "students.db";

string trim(string s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// prints the prompt and reads one line, returns false at end of input
bool readLine(string prompt, string &line) {
    cout << prompt;
    if (!getline(cin, line)) return false;
    line = trim(line);
    return true;
}

bool validText(string s, bool isDept) {
    bool hasLetter = false;
    for (char ch : s) {
        if (isalpha((unsigned char)ch)) hasLetter = true;
        else if (ch == ' ' || ch == '.' || ch == '-' || ch == '\'') continue;
        else if (isDept && (isdigit((unsigned char)ch) || ch == '&')) continue;
        else return false;
    }
    return hasLetter;
}

bool toInt(string s, int &n) {
    stringstream ss(s);
    char extra;
    return (ss >> n) && !(ss >> extra);
}

bool toFloat(string s, float &f) {
    stringstream ss(s);
    char extra;
    return (ss >> f) && !(ss >> extra);
}

string marksText(float m) {
    stringstream ss;
    ss << fixed << setprecision(2) << m;
    return ss.str();
}

string getGrade(float m) {
    if (m >= 90) return "A+";
    if (m >= 80) return "A";
    if (m >= 70) return "B";
    if (m >= 60) return "C";
    if (m >= 50) return "D";
    return "F";
}

// getText and getMarks return 0 = end of input, 1 = ok, 2 = blank (only if allowBlank)
bool getInt(string prompt, int low, int high, int &n) {
    string line;
    while (true) {
        if (!readLine(prompt, line)) return false;
        if (toInt(line, n) && n >= low && n <= high) return true;
        cout << "  Invalid number. Enter an integer between " << low << " and " << high << ".\n";
    }
}

int getMarks(string prompt, float &m, bool allowBlank) {
    string line;
    while (true) {
        if (!readLine(prompt, line)) return 0;
        if (allowBlank && line == "") return 2;
        if (toFloat(line, m) && m >= 0 && m <= 100) {
            if (m == 0) m = 0;  // turns -0 into 0
            return 1;
        }
        cout << "  Invalid marks. Enter a number between 0 and 100.\n";
    }
}

int getText(string prompt, size_t maxLen, bool isDept, string &text, bool allowBlank) {
    string line;
    while (true) {
        if (!readLine(prompt, line)) return 0;
        if (allowBlank && line == "") return 2;
        if (line.size() > maxLen) {
            cout << "  Too long (max " << maxLen << " characters).\n";
            continue;
        }
        if (validText(line, isDept)) {
            text = line;
            return 1;
        }
        if (isDept) cout << "  Invalid text. Use letters, digits, spaces, '&', '.', '-'.\n";
        else cout << "  Invalid text. Use letters, spaces, '.', '-' or apostrophes.\n";
    }
}

// returns students.end() when the roll number is not found
vector<Student>::iterator findStudent(int roll) {
    return find_if(students.begin(), students.end(),
                   [roll](const Student &s) { return s.roll == roll; });
}

void sortStudents() {
    sort(students.begin(), students.end(),
         [](const Student &a, const Student &b) { return a.roll < b.roll; });
}

void loadFile() {
    ifstream file(fileName);
    if (!file) return;  // no file yet, start empty

    string line;
    int lineNo = 0;
    while (getline(file, line)) {
        lineNo++;
        line = trim(line);
        if (line == "" || line[0] == '#') continue;

        stringstream ss(line);
        string rollText, marksStr, extra;
        Student s;
        getline(ss, rollText, '|');
        getline(ss, s.name, '|');
        getline(ss, s.dept, '|');
        getline(ss, marksStr, '|');

        bool ok = !getline(ss, extra, '|');  // more than 4 fields is bad
        ok = ok && toInt(trim(rollText), s.roll) && s.roll >= 1 && s.roll <= 999999;
        s.name = trim(s.name);
        s.dept = trim(s.dept);
        ok = ok && validText(s.name, false) && s.name.size() <= 49;
        ok = ok && validText(s.dept, true) && s.dept.size() <= 29;
        ok = ok && toFloat(trim(marksStr), s.marks) && s.marks >= 0 && s.marks <= 100;

        if (!ok) {
            cerr << "Warning: line " << lineNo << " is invalid, skipped.\n";
        } else if (findStudent(s.roll) != students.end()) {
            cerr << "Warning: line " << lineNo << " duplicates roll " << s.roll << ", skipped.\n";
        } else {
            students.push_back(s);
        }
    }
    sortStudents();
}

// writes to a temp file first so the old file is never left half written
bool saveFile() {
    string tempName = fileName + ".tmp";
    ofstream file(tempName);
    if (!file) {
        cout << "Warning: cannot write " << tempName << "\n";
        return false;
    }
    for (const Student &s : students) {
        file << s.roll << "|" << s.name << "|" << s.dept << "|" << marksText(s.marks) << "\n";
    }
    file.close();
    if (rename(tempName.c_str(), fileName.c_str()) != 0) {
        cout << "Warning: could not save " << fileName << "\n";
        return false;
    }
    return true;
}

void printLine() {
    cout << "+--------+" << string(52, '-') << "+" << string(32, '-') << "+---------+-------+\n";
}

void printHeader() {
    printLine();
    cout << "| " << left << setw(6) << "Roll" << " | " << setw(50) << "Name" << " | "
         << setw(30) << "Department" << " | " << setw(7) << "Marks" << " | "
         << setw(5) << "Grade" << " |\n";
    printLine();
}

void printRow(const Student &s) {
    cout << "| " << left << setw(6) << s.roll << " | " << setw(50) << s.name << " | "
         << setw(30) << s.dept << " | " << right << setw(7) << marksText(s.marks) << left
         << " | " << setw(5) << getGrade(s.marks) << " |\n";
}

void addStudent() {
    cout << "\n--- Add Student ---\n";
    Student s;
    while (true) {
        if (!getInt("Roll number: ", 1, 999999, s.roll)) return;
        if (findStudent(s.roll) == students.end()) break;
        cout << "  Roll number " << s.roll << " already exists. Choose another.\n";
    }
    if (getText("Name: ", 49, false, s.name, false) == 0) return;
    if (getText("Department: ", 29, true, s.dept, false) == 0) return;
    if (getMarks("Marks (0-100): ", s.marks, false) == 0) return;

    students.push_back(s);
    sortStudents();
    if (saveFile()) cout << "Student " << s.roll << " added and saved.\n";
}

void searchByRoll() {
    cout << "\n--- Search by Roll Number ---\n";
    int roll;
    if (!getInt("Roll number: ", 1, 999999, roll)) return;
    auto it = findStudent(roll);
    if (it == students.end()) {
        cout << "No student found with roll number " << roll << ".\n";
        return;
    }
    cout << "\n";
    printHeader();
    printRow(*it);
    printLine();
}

void searchByName() {
    cout << "\n--- Search by Name ---\n";
    string query;
    if (!readLine("Name (or part of it): ", query)) return;
    if (query == "") {
        cout << "Search text cannot be empty.\n";
        return;
    }
    transform(query.begin(), query.end(), query.begin(), [](unsigned char c) { return tolower(c); });

    int count = 0;
    for (const Student &s : students) {
        string lowerName = s.name;
        transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                  [](unsigned char c) { return tolower(c); });
        if (lowerName.find(query) == string::npos) continue;
        if (count == 0) {
            cout << "\n";
            printHeader();
        }
        printRow(s);
        count++;
    }
    if (count == 0) {
        cout << "No student matches \"" << query << "\".\n";
    } else {
        printLine();
        cout << count << " match(es).\n";
    }
}

void updateStudent() {
    cout << "\n--- Update Student ---\n";
    int roll;
    if (!getInt("Roll number to update: ", 1, 999999, roll)) return;
    auto it = findStudent(roll);
    if (it == students.end()) {
        cout << "No student found with roll number " << roll << ".\n";
        return;
    }

    Student s = *it;
    string text;
    float m;
    int result;
    cout << "Press Enter to keep the current value.\n";

    result = getText("Name [" + s.name + "]: ", 49, false, text, true);
    if (result == 0) return;
    if (result == 1) s.name = text;

    result = getText("Department [" + s.dept + "]: ", 29, true, text, true);
    if (result == 0) return;
    if (result == 1) s.dept = text;

    result = getMarks("Marks [" + marksText(s.marks) + "]: ", m, true);
    if (result == 0) return;
    if (result == 1) s.marks = m;

    *it = s;
    if (saveFile()) cout << "Student " << roll << " updated and saved.\n";
}

void displayAll() {
    if (students.empty()) {
        cout << "\nNo student records found.\n";
        return;
    }
    cout << "\n";
    printHeader();
    double total = 0;
    for (const Student &s : students) {
        printRow(s);
        total += s.marks;
    }
    printLine();
    cout << "Total records: " << students.size()
         << "   Class average: " << marksText((float)(total / (double)students.size())) << "\n";
}

void deleteStudent() {
    cout << "\n--- Delete Student ---\n";
    int roll;
    if (!getInt("Roll number to delete: ", 1, 999999, roll)) return;
    auto it = findStudent(roll);
    if (it == students.end()) {
        cout << "No student found with roll number " << roll << ".\n";
        return;
    }
    cout << "\n";
    printHeader();
    printRow(*it);
    printLine();

    string answer;
    if (!readLine("Delete this record? (y/n): ", answer)) return;
    if (answer != "y" && answer != "Y") {
        cout << "Deletion cancelled.\n";
        return;
    }
    students.erase(it);
    if (saveFile()) cout << "Student " << roll << " deleted and saved.\n";
}

int main(int argc, char *argv[]) {
    if (argc > 1) fileName = argv[1];
    loadFile();
    cout << "Database: " << fileName << " (" << students.size() << " record(s) loaded)\n";

    while (true) {
        cout << "\n==== Student Management System ====\n"
                " 1. Add student\n"
                " 2. Search by roll number\n"
                " 3. Search by name\n"
                " 4. Update student\n"
                " 5. Display all students\n"
                " 6. Delete student\n"
                " 0. Exit\n";

        int choice;
        if (!getInt("Choice: ", 0, 6, choice)) {
            cout << "\nEnd of input. Exiting.\n";
            break;
        }
        if (choice == 0) {
            cout << "Goodbye.\n";
            break;
        }
        if (choice == 1) addStudent();
        else if (choice == 2) searchByRoll();
        else if (choice == 3) searchByName();
        else if (choice == 4) updateStudent();
        else if (choice == 5) displayAll();
        else if (choice == 6) deleteStudent();
    }
    return 0;
}
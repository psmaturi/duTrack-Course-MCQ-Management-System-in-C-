#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
using namespace std;

// Base class for Users
class User {
protected:
    string name, username;

public:
    User(string n, string u) : name(n), username(u) {}
    virtual void menu() = 0; // Pure virtual method for polymorphism
    virtual ~User() {}
};

// Course class
class Course {
    string courseName, courseCode;
    vector<pair<string, char>> mcqs;
    vector<vector<string>> options;

public:
    Course(string name, string code) : courseName(name), courseCode(code) {}
    void addMCQ(const string& question, const vector<string>& optionList, char answer) {
        mcqs.push_back({question, answer});
        options.push_back(optionList);
    }
    bool attemptMCQs() {
        if (mcqs.empty()) return cout << "No MCQs available.\n", false;
        int correct = 0;
        cout << "Answer questions for " << courseName << ":\n";
        for (size_t i = 0; i < mcqs.size(); ++i) {
            cout << i + 1 << ". " << mcqs[i].first << "\n";
            for (size_t j = 0; j < options[i].size(); ++j)
                cout << static_cast<char>('A' + j) << ") " << options[i][j] << "\n";
            char ans; cin >> ans;
            if (toupper(ans) == mcqs[i].second) ++correct;
        }
        int score = (100 * correct) / mcqs.size();
        cout << "Score: " << score << "%\n";
        return score >= 50;
    }
    string getName() const { return courseName; }
    string getCode() const { return courseCode; }
};

// Course Manager
class CourseManager {
    map<string, shared_ptr<Course>> courses;

public:
    void addCourse(const string& name, const string& code) {
        courses[code] = make_shared<Course>(name, code);
    }
    shared_ptr<Course> getCourse(const string& code) {
        return courses.count(code) ? courses[code] : nullptr;
    }
    void displayCourses() {
        cout << "Available courses:\n";
        for (const auto& c : courses)
            cout << "- " << c.second->getName() << " (" << c.first << ")\n";
    }
};

// Student class
class Student : public User {
    string roll;
    vector<shared_ptr<Course>> courses;
    CourseManager& courseManager;

public:
    Student(string n, string r, string u, CourseManager& cm)
        : User(n, u), roll(r), courseManager(cm) {}

    void enroll() {
        courseManager.displayCourses();
        string code;
        cout << "Enter course code to enroll: ";
        cin >> code;
        auto course = courseManager.getCourse(code);
        if (course) {
            courses.push_back(course);
            cout << "Enrolled in " << course->getName() << " successfully.\n";
        } else {
            cout << "Invalid course code.\n";
        }
    }

    void attemptCourses() {
        for (auto& course : courses) {
            cout << (course->attemptMCQs() ? "Passed " : "Failed ") << course->getName() << "!\n";
        }
    }

    void displayCourses() {
        cout << "Enrolled courses:\n";
        for (const auto& c : courses) cout << "- " << c->getName() << "\n";
    }

    void menu() override {
        int opt;
        do {
            cout << "\n1. Enroll\n2. Show Courses\n3. Attempt MCQs\n4. Logout\nChoice: ";
            cin >> opt;
            if (opt == 1) enroll();
            else if (opt == 2) displayCourses();
            else if (opt == 3) attemptCourses();
        } while (opt != 4);
    }
};

// Admin class
class Admin : public User {
    CourseManager& courseManager;

public:
    Admin(string n, string u, CourseManager& cm) : User(n, u), courseManager(cm) {}

    void menu() override {
        int opt;
        do {
            cout << "\n1. Add Course\n2. Show Courses\n3. Add MCQs\n4. Logout\nChoice: ";
            cin >> opt;
            if (opt == 1) {
                string name, code;
                cout << "Course Name: "; cin.ignore(); getline(cin, name);
                cout << "Course Code: "; cin >> code;
                courseManager.addCourse(name, code);
            } else if (opt == 2) {
                courseManager.displayCourses();
            } else if (opt == 3) {
                string code;
                cout << "Course Code: "; cin >> code;
                auto course = courseManager.getCourse(code);
                if (course) {
                    int n; cout << "Number of MCQs: "; cin >> n; cin.ignore();
                    for (int i = 0; i < n; ++i) {
                        string q;
                        vector<string> opts(4);
                        char ans;
                        cout << "Question: "; getline(cin, q);
                        for (int j = 0; j < 4; ++j) {
                            cout << "Option " << static_cast<char>('A' + j) << ": ";
                            getline(cin, opts[j]);
                        }
                        cout << "Answer (A/B/C/D): "; cin >> ans; cin.ignore();
                        course->addMCQ(q, opts, ans);
                    }
                } else {
                    cout << "Invalid course code.\n";
                }
            }
        } while (opt != 4);
    }
};

// User Manager
class UserManager {
    struct Account { string name, roll, password; };
    map<string, Account> accounts;
    string adminID = "admin", adminPassword = "admin123";

public:
    void registerStudent(const string& roll, const string& name, const string& username, const string& pass) {
        accounts[username] = {name, roll, pass};
        cout << "Registration successful!\n";
    }
    bool login(const string& user, const string& pass) {
        return accounts.count(user) && accounts[user].password == pass;
    }
    Account getAccount(const string& user) { return accounts[user]; }
    bool adminLogin(const string& id, const string& pass) {
        return id == adminID && pass == adminPassword;
    }
    bool isRegistered(const string& user) { return accounts.count(user); }
};

// Main function
int main() {
    CourseManager cm;
    UserManager um;
    cm.addCourse("Data Structures", "CS101");
    cm.addCourse("Algorithms", "CS102");

    int choice;
    do {
        cout << "\nMenu:\n1. Student Login\n2. Admin Login\n3. Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            string user, pass;
            cout << "Username: "; cin >> user;
            if (um.isRegistered(user)) {
                cout << "Password: "; cin >> pass;
                if (um.login(user, pass)) {
                    auto acc = um.getAccount(user);
                    Student student(acc.name, acc.roll, user, cm);
                    student.menu();
                } else {
                    cout << "Login failed.\n";
                }
            } else {
                cout << "New user. Registering...\n";
                string roll, name, pass;
                cout << "Roll: "; cin >> roll;
                cout << "Name: "; cin.ignore(); getline(cin, name);
                cout << "Password: "; cin >> pass;
                um.registerStudent(roll, name, user, pass);
            }
        } else if (choice == 2) {
            string id, pass;
            cout << "Admin ID: "; cin >> id;
            cout << "Password: "; cin >> pass;
            if (um.adminLogin(id, pass)) {
                Admin admin("Admin", id, cm);
                admin.menu();
            } else {
                cout << "Admin login failed.\n";
            }
        }
    } while (choice != 3);

    return 0;
}
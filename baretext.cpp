#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void displayMenu() {
    cout << "Baretext Text Editor\n";
    cout << "1. New File\n";
    cout << "2. Edit File\n";
    cout << "3. Save File\n";
    cout << "4. Exit\n";
    cout << "Choose an option: ";
}

void newFile(vector<string>& buffer) {
    buffer.clear();
    string line;
    cout << "Enter text (type 'END' to finish):\n";
    cin.ignore(); // Ignore newline character left in the buffer
    while (getline(cin, line) && line != "END") {
        buffer.push_back(line);
    }
}

void editFile(vector<string>& buffer) {
    string filename;
    cout << "Enter filename to edit: ";
    cin >> filename;

    ifstream file(filename);
    if (file.is_open()) {
        buffer.clear();
        string line;
        while (getline(file, line)) {
            buffer.push_back(line);
        }
        file.close();

        cout << "Current content:\n";
        for (const auto& line : buffer) {
            cout << line << endl;
        }

        cout << "Enter new text (type 'END' to finish):\n";
        cin.ignore(); // Ignore newline character left in the buffer
        while (getline(cin, line) && line != "END") {
            buffer.push_back(line);
        }
    } else {
        cout << "Unable to open file for editing.\n";
    }
}

void saveFile(const vector<string>& buffer) {
    string filename;
    cout << "Enter filename to save: ";
    cin >> filename;

    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& line : buffer) {
            file << line << endl;
        }
        file.close();
    } else {
        cout << "Unable to save file.\n";
    }
}

int main() {
    vector<string> buffer;
    int choice;
    while (true) {
        displayMenu();
        cin >> choice;
        cin.ignore(); // Ignore newline character left in the buffer
        switch (choice) {
            case 1:
                newFile(buffer);
                break;
            case 2:
                editFile(buffer);
                break;
            case 3:
                saveFile(buffer);
                break;
            case 4:
                return 0;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    }
    return 0;
}

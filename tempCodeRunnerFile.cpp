#include <iostream>
#include <string>
#include <chrono>
#include <limits>
#include <vector>
#include "libraryqrcode/qrcodegen.hpp"

using namespace std;
using namespace std::chrono;

struct MemberRecord {
    string name;
    bool hasYearly;
    bool hasMonthly;
    bool hasDaily;
    double totalFee;
};

vector<MemberRecord> memberList;

void displayMenu();
void choices(int choice);
void registerMember();
void Clear_Terminal();
bool checkMember(string name);

int main() {
    int choice;
    do {
      Clear_Terminal();
        displayMenu();
        cout << "Enter choice: ";
        cin >> choice;




        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number (1-4).\n\n";
            continue;
        }




        if (choice < 1 || choice > 6) {
            cout << "Invalid choice. Please enter 1, 2, 3, or 4.\n\n";
            continue;
        }




        choices(choice);
    } while (choice != 6);
    return 0;
}




void Clear_Terminal() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}




void displayMenu () {
    cout << "\n==== Hardcore Fitness Gym ====\n";
    cout << "1. Add Guest (Day Pass ₱60)\n";
    cout << "2. Register Member\n";
    cout << "3. Show Member Records\n";
    cout << "4. Manage Membership\n";
    cout << "5. Scan QR Code\n";
    cout << "6. Exit \n";
}




void choices(int choice) {
    if (choice == 1) {
    Clear_Terminal();
        MemberRecord guest;
        cout << "Enter Guest Name: ";
        cin.ignore(1, '\n');
        getline(cin, guest.name);
    if(checkMember(guest.name)) {
        cout<<"\nThis account already exists.\n";
    }
    else {
        guest.hasYearly = false;
        guest.hasMonthly = false;
        guest.hasDaily = false;
        guest.totalFee = 60;
        memberList.push_back(guest);
        cout << "Guest has been added!\n";
    }
    cout<< "Press Enter..";
    cin.get();
    }
    else if (choice == 2) {
        Clear_Terminal();
        registerMember();
    }
    else if (choice == 3) {
        Clear_Terminal();
        cout << "\n--- Current Member Records ---\n";
        if (memberList.empty()) {
            cout << "No members registered yet.\n";
        } else {
            for (size_t i = 0; i < memberList.size(); ++i) {
                cout << "\nMember #" << i + 1 << endl;
                cout << "Name: " << memberList[i].name << endl;




                if (!memberList[i].hasYearly)
                    cout << "Membership: Guest\n";
                else {
                    cout << "Membership: Yearly Member\n";
                    if (memberList[i].hasMonthly)
                        cout << "- Availment: Monthly Pass (₱600)\n";
                    if (memberList[i].hasDaily)
                        cout << "- Availment: Member Day Pass (₱45)\n";
                }




                cout << "Total Paid: ₱" << memberList[i].totalFee << endl;
            }
        }
    cout<<"\nPress Enter to continue..";
    cin.ignore();
    cin.get();
    }
    else if (choice != 6) {
        cout << "Invalid choice. Please enter 1, 2, 3, or 4.\n";
    }
}




void registerMember() {
    Clear_Terminal();
    MemberRecord m;
    cout << "Enter Member name: ";
    cin.ignore(100, '\n');
    getline(cin, m.name);
    if(checkMember(m.name)) {
    cout << "This account already exists.\n";
    cout<< "Press Enter to continue..";
    cin.get();
    return;
}




    m.hasYearly = false;
    m.hasMonthly = false;
    m.hasDaily = false;
    m.totalFee = 0;




    int yearlyChoice;
    cout << "\nWould you like to avail the Yearly Membership for ₱150? (1 = Yes, 2 = No): ";
    cin >> yearlyChoice;




    if (yearlyChoice != 1) {
        cout << "\nYou cannot avail Monthly or Member Day Pass without Yearly Membership.\n";
        cout << "Registration cancelled.\n";
    cout << "Press enter to continue..";
    cin.ignore();
    cin.get();
        return;
    }




    m.hasYearly = true;
    m.totalFee += 150;
    cout << "Yearly Membership activated.\n";




    int nextChoice;
    cout << "\n==== Hardcore Fitness Gym ====\n";
    cout << "1. Add Guest (Day Pass ₱60)\n";
    cout << "2. Register Member\n";
    cout << "3. Show Member Records\n";
    cout << "4. Manage Membership\n";
    cout << "5. Scan QR Code\n";
    cout << "6. Exit \n";
    cin >> nextChoice; 




    if (nextChoice == 1) {
        m.hasMonthly = true;
        m.totalFee += 600;
        cout << "You added a Monthly Pass.\n";
    }
    else if (nextChoice == 2) {
        m.hasDaily = true;
        m.totalFee += 45;
        cout << "You added a Member Day Pass.\n";
    }
    else {
        cout << "You chose no additional pass.\n";
    }




    memberList.push_back(m);
    cout << "\nMember registration complete!\n";
}




bool checkMember(string name) {
for(auto m: memberList) {
    if(m.name == name) return true;
    }
    return false;
}

#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <filesystem> // so the images can be saved in their own folder
#include "libraryqrcode/qrcodegen.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


using namespace std;
using namespace qrcodegen;


time_t addDays(time_t currentTime, int daysToAdd) {
    return currentTime + (daysToAdd * 24 * 60 * 60);
}


struct MemberRecord {
    string name;
    bool hasYearly;
    bool hasMonthly;
    bool hasDaily;
    double totalFee;
    time_t registrationTime;
    time_t yearlyExpiryTime;
    time_t currentPassExpiry;
};


vector<MemberRecord> memberList;


void displayMenu();
void choices(int choice);
void registerMember();
void manageMembership();
void Clear_Terminal();
bool checkMember(string name);
MemberRecord* findMember(string name);
void generateQRCode(const MemberRecord &member);
void scanQRCodeManual();
void cleanupExpiredMembers();
void checkPassExpiry();


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
            cout << "Invalid input. Please enter a number (1-6).\n\n";
            continue;
        }


        if (choice < 1 || choice > 6) {
            cout << "Invalid choice. Please enter 1 to 6.\n\n";
            continue;
        }


        choices(choice);
    } while (choice != 6);
    return 0;
}


void Clear_Terminal() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void displayMenu() {
    cout << "\n==== Hardcore Fitness Gym ====\n";
    cout << "1. Add Guest (Day Pass PHP 60)\n";
    cout << "2. Register Member\n";
    cout << "3. Show Member Records\n";
    cout << "4. Manage Membership\n";
    cout << "5. Scan QR Code\n";
    cout << "6. Exit\n";
}


void choices(int choice) {
    if (choice == 1) {
        Clear_Terminal();
        MemberRecord guest;
        cout << "Enter Guest Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, guest.name);


        if (checkMember(guest.name)) {
            cout << "\nThis account already exists.\n";
        } else {
            guest.hasYearly = false;
            guest.hasMonthly = false;
            guest.hasDaily = true;
            guest.totalFee = 60;
            time_t now;
            time(&now);
            guest.registrationTime = now;
            guest.currentPassExpiry = addDays(now, 1);
            memberList.push_back(guest);
            cout << "Guest has been added!\n";
        }


        cout << "Press Enter..";
        cin.get();
    }


    else if (choice == 2) {
        Clear_Terminal();
        registerMember();
    }


    else if (choice == 3) {
        cleanupExpiredMembers();
        checkPassExpiry();
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
                        cout << "- Availment: Monthly Pass (PHP 600)\n";
                    if (memberList[i].hasDaily)
                        cout << "- Availment: Member Day Pass (PHP 45)\n";
                    if (!memberList[i].hasMonthly && !memberList[i].hasDaily)
                        cout << "- Availment: None\n" ;
                }


                cout << "Current Pass Expires: " << ctime(&memberList[i].currentPassExpiry);
                cout << "Total Paid: PHP " << memberList[i].totalFee << endl;
            }
        }


        cout << "\nPress Enter to continue..";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }


    else if (choice == 4) {
        Clear_Terminal();
        manageMembership();
    }


    else if (choice == 5) {
        Clear_Terminal();
        scanQRCodeManual();
    }
}


void registerMember() {
    Clear_Terminal();
    MemberRecord m;
    cout << "Enter Member name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, m.name);


    if (checkMember(m.name)) {
        cout << "This account already exists.\n";
        cout << "Press Enter to continue..";
        cin.get();
        return;
    }


    m.hasYearly = false;
    m.hasMonthly = false;
    m.hasDaily = false;
    m.totalFee = 0;
    m.yearlyExpiryTime = 0;
    m.currentPassExpiry = 0;


    int yearlyChoice;
    cout << "\nWould you like to avail the Yearly Membership for PHP 150? (1 = Yes, 2 = No): ";
    cin >> yearlyChoice;


    if (yearlyChoice != 1) {
        cout << "\nYou cannot avail Monthly or Member Day Pass without Yearly Membership.\n";
        cout << "Registration cancelled.\n";
        cout << "Press enter to continue..";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }


    m.hasYearly = true;
    m.totalFee += 150;
    cout << "Yearly Membership activated.\n";
    time(&m.registrationTime);
    m.yearlyExpiryTime = addDays(m.registrationTime, 365);
    m.currentPassExpiry = m.yearlyExpiryTime;


    int passChoice;
    cout << "\nWould you like to avail:\n";
    cout << "1. Monthly Pass (PHP 600)\n";
    cout << "2. Member Day Pass (PHP 45)\n";
    cout << "3. None\n";
    cout << "Enter choice: ";
    cin >> passChoice;


    time_t now;
    time(&now);


    if (passChoice == 1) {
        m.hasMonthly = true;
        m.totalFee += 600;
        m.currentPassExpiry = addDays(now, 30);
    } else if (passChoice == 2) {
        m.hasDaily = true;
        m.totalFee += 45;
        m.currentPassExpiry = addDays(now, 1);
    }


    memberList.push_back(m);
    cout << "\nMember registration complete!\n";


    if (m.hasMonthly) {
        generateQRCode(m);
        cout << "\nQR code generated for " << m.name << " (Monthly Member PNG file).\n";
    } else {
        cout << "\nNo QR code generated (only Monthly Members receive QR codes).\n";
    }


    cout << "Press Enter to continue..";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}


void cleanupExpiredMembers() {
    time_t now;
    time(&now);
    int removedCount = 0;


    for (size_t i = 0; i < memberList.size();) {
        if (memberList[i].yearlyExpiryTime != 0 && memberList[i].yearlyExpiryTime <= now) {
            cout << "REMOVED: " << memberList[i].name
                 << " (Yearly Status Expired on: " << ctime(&memberList[i].yearlyExpiryTime) << ")\n";
            memberList.erase(memberList.begin() + i);
            removedCount++;
        } else {
            ++i;
        }
    }


    if (removedCount > 0)
        cout << "\n--- System Cleanup Complete: " << removedCount << " member(s) removed. ---\n";
    else
        cout << "\nNo expired members found.\n";
}


void checkPassExpiry() {
    time_t now;
    time(&now);


    for (auto &m : memberList) {
        if (m.currentPassExpiry <= now && m.hasYearly) {
            m.hasMonthly = false;
            m.hasDaily = false;
            m.currentPassExpiry = m.yearlyExpiryTime;
        }
    }
}


void manageMembership() {
    cout << "Enter Member Name to Manage: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string name;
    getline(cin, name);


    MemberRecord* memberPtr = findMember(name);
    if (!memberPtr) {
        cout << "No member found with that name.\n";
        cout << "Press Enter to continue..";
        cin.get();
        return;
    }


    if (!memberPtr->hasYearly) {
        cout << "This user is not a registered member yet. Please register first.\n";
        cout << "Press Enter to continue..";
        cin.get();
        return;
    }


    cout << "\n==== Manage Membership ====\n";
    cout << "1. Add Monthly Pass (PHP 600)\n";
    cout << "2. Add Member Day Pass (PHP 45)\n";
    cout << "3. Cancel\n";
    cout << "Enter choice: ";
    int manageChoice;
    cin >> manageChoice;


    time_t now;
    time(&now);


    if (manageChoice == 1 && !memberPtr->hasMonthly) {
        memberPtr->hasMonthly = true;
        memberPtr->totalFee += 600;
        memberPtr->currentPassExpiry = addDays(now, 30);
        cout << "Monthly Pass added successfully.\n";
    } else if (manageChoice == 2 && !memberPtr->hasDaily) {
        memberPtr->hasDaily = true;
        memberPtr->totalFee += 45;
        memberPtr->currentPassExpiry = addDays(now, 1);
        cout << "Member Day Pass added successfully.\n";
    } else {
        cout << "No changes made.\n";
    }


    if (memberPtr->hasMonthly) {
        generateQRCode(*memberPtr);
        cout << "QR code generated for Monthly Member.\n";
    }


    cout << "Press Enter to continue..";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}


bool checkMember(string name) {
    for (auto &m : memberList)
        if (m.name == name) return true;
    return false;
}


MemberRecord* findMember(string name) {
    for (auto &m : memberList)
        if (m.name == name)
            return &m;
    return nullptr;
}


void generateQRCode(const MemberRecord &member) {
    string data = "GYM MEMBER | " + member.name + " | ";
    if (!member.hasYearly)
        data += "Guest";
    else {
        data += "Yearly";
        if (member.hasMonthly) data += " + Monthly";
    }


    const QrCode qr = QrCode::encodeText(data.c_str(), QrCode::Ecc::MEDIUM);
    int size = qr.getSize();
    int scale = 10;
    int border = 4;
    int imgSize = (size + border * 2) * scale;


    vector<unsigned char> img(imgSize * imgSize * 3, 255);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            if (qr.getModule(x, y)) {
                int startX = (x + border) * scale;
                int startY = (y + border) * scale;
                for (int yy = 0; yy < scale; ++yy) {
                    for (int xx = 0; xx < scale; ++xx) {
                        int idx = ((startY + yy) * imgSize + (startX + xx)) * 3;
                        img[idx + 0] = 0;
                        img[idx + 1] = 0;
                        img[idx + 2] = 0;
                    }
                }
            }
        }
    }


    std::filesystem::create_directories("qrcodes"); // creates directory for qr code images
    string filename = "qrcodes/" + member.name + "_QR.png"; // that '"qrcodes/'" part is the folder
    if (!stbi_write_png(filename.c_str(), imgSize, imgSize, 3, img.data(), imgSize * 3))
        cerr << "Failed to write " << filename << "\n";
    else
        cout << "\nQR code saved as " << filename << endl;
}


void scanQRCodeManual() {
    cout << "==== QR Code Verification ====\n";
    cout << "Enter the name shown on the QR code: ";


    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string scannedName;
    getline(cin, scannedName);


    MemberRecord* found = findMember(scannedName);
    if (found) {
        cout << "\nMember Verified Successfully!\n";
        cout << "Name: " << found->name << endl;
        cout << "Membership Type: ";
        if (!found->hasYearly)
            cout << "Guest\n";
        else {
            cout << "Yearly Member";
            if (found->hasMonthly) cout << " + Monthly Pass";
            if (found->hasDaily) cout << " + Daily Pass";
            cout << endl;
        }
        cout << "Total Paid: PHP " << found->totalFee << endl;
    } else {
        cout << "\nMember not found in records.\n";
    }


    cout << "\nPress Enter to continue..";
    cin.get();
}
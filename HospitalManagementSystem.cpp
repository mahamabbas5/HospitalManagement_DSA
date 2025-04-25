#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <queue>

using namespace std;

// ================= Utility Functions =================
bool isAlphaString(const string& str) {
    for (char c : str) {
        if (!isalpha(c)) return false;
    }
    return true;
}

bool isNumericString(const string& str) {
    if (str.empty()) {
        return false;
    }
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

int getValidatedInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cout << "Invalid input. Please enter a valid integer." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            return value;
        }
    }
}

double getValidatedDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cout << "Invalid input. Please enter a valid number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            return value;
        }
    }
}

string getValidatedString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        bool hasDigit = false;
        for (char ch : value) {
            if (isdigit(ch)) {
                hasDigit = true;
                break;
            }
        }
        if (value.empty()) {
            cout << "Input cannot be empty. Please try again.\n";
        } else if (hasDigit) {
            cout << "Input cannot contain numbers. Please enter a valid string.\n";
        } else {
            return value;
        }
    }
}

string getValidatedRole(const string& prompt) {
    string value;
    const vector<string> validRoles = {"doctor", "nurses", "paramedics", "janitors"};
    while (true) {
        cout << prompt;
        getline(cin, value);
        for (const string& role : validRoles) {
            if (value == role) {
                return value;
            }
        }
        cout << "Invalid role. Please enter one of the following: doctor, nurses, paramedics, janitors.\n";
    }
}


// ================= Patient Management =================
struct Patient {
    int id;
    string name;
    int age;
    string condition;
    string doctorName;
    string appointmentTime;
    Patient* next;

    Patient(int id, string name, int age, string condition, string doctorName = "", string appointmentTime = "")
        : id(id), name(name), age(age), condition(condition), doctorName(doctorName), appointmentTime(appointmentTime), next(nullptr) {}
};

class PatientList {
private:
    Patient* head;

public:
    PatientList() : head(nullptr) {}

    void admitPatient(int id, string name, int age, string condition, string doctorName = "", string appointmentTime = "") {
        Patient* newPatient = new Patient(id, name, age, condition, doctorName, appointmentTime);
        newPatient->next = head;
        head = newPatient;
    }

    void displayPatients() const {
        Patient* current = head;
        while (current) {
            cout << "ID: " << current->id << ", Name: " << current->name
                 << ", Age: " << current->age << ", Condition: " << current->condition
                 << ", Doctor: " << current->doctorName
                 << ", Appointment: " << current->appointmentTime << endl;
            current = current->next;
        }
    }

    Patient* searchPatientByID(int id) {
        Patient* current = head;
        while (current) {
            if (current->id == id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    ~PatientList() {
        Patient* current = head;
        while (current) {
            Patient* temp = current;
            current = current->next;
            delete temp;
        }
    }
};

// ================= Doctor Management =================
struct Doctor {
    string name;
    string specialization;
    vector<pair<string, string>> availableTimes; // Time paired with day of the week
    vector<bool> appointmentAvailable; // Tracks availability of appointment times

    Doctor(string name, string specialization, vector<pair<string, string>> availableTimes)
        : name(name), specialization(specialization), availableTimes(availableTimes), appointmentAvailable(availableTimes.size(), true) {}
};

class DoctorManagement {
private:
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> appointmentPriorityQueue; // Using a min-heap for priority-based appointments
    vector<Doctor> doctors;

public:
    DoctorManagement() {
        doctors.push_back(Doctor("Dr. Ahmad", "Cardiologist", {{"9:00 AM", "Monday"}, {"10:00 AM", "Tuesday"}, {"11:00 AM", "Thursday"}}));
        doctors.push_back(Doctor("Dr. Fatima", "Dermatologist", {{"9:30 AM", "Wednesday"}, {"10:30 AM", "Friday"}, {"1:00 PM", "Monday"}}));
        doctors.push_back(Doctor("Dr. Ibrahim", "Neurologist", {{"10:00 AM", "Tuesday"}, {"12:00 PM", "Wednesday"}, {"2:00 PM", "Thursday"}}));
        doctors.push_back(Doctor("Dr. Maham", "Orthopedist", {{"8:30 AM", "Monday"}, {"2:00 PM", "Tuesday"}, {"11:00 AM", "Wednesday"}}));
        doctors.push_back(Doctor("Dr. Shoaib", "Pediatrician", {{"9:00 AM", "Friday"}, {"2:00 PM", "Saturday"}, {"10:00 AM", "Sunday"}}));
        doctors.push_back(Doctor("Dr. Abbas", "Oncologist", {{"10:00 AM", "Monday"}, {"3:00 PM", "Wednesday"}, {"1:00 PM", "Friday"}}));
        doctors.push_back(Doctor("Dr. Zarrar", "Gastroenterologist", {{"8:00 AM", "Tuesday"}, {"11:00 AM", "Thursday"}, {"2:30 PM", "Friday"}}));
    }

    void showDoctors() {
        cout << "From whom you want a checkup:" << endl;
        for (int i = 0; i < doctors.size(); i++) {
            cout << i + 1 << ". " << doctors[i].name << " - " << doctors[i].specialization << endl;
            cout << "Available times: " << endl;
            for (size_t j = 0; j < doctors[i].availableTimes.size(); j++) {
                if (doctors[i].appointmentAvailable[j]) {
                    cout << doctors[i].availableTimes[j].first << " on " << doctors[i].availableTimes[j].second << endl;
                }
            }
            cout << endl;
        }
    }

    bool allocateDoctorAppointment(int doctorIndex, string& appointmentTime) {
        if (doctorIndex < 0 || doctorIndex >= doctors.size()) return false;

        Doctor& doctor = doctors[doctorIndex];

        // Ask the user to select an appointment time from available options
        cout << "Choose an available appointment time:\n";
        vector<int> availableTimesIndex;
        for (size_t i = 0; i < doctor.availableTimes.size(); ++i) {
            if (doctor.appointmentAvailable[i]) {
                cout << doctor.availableTimes[i].first << " on " << doctor.availableTimes[i].second << "\n";
                availableTimesIndex.push_back(i);
            }
        }

        if (availableTimesIndex.empty()) {
            cout << "No appointments available for Dr. " << doctor.name << " in this week.\n";
            return false;
        }

        int timeChoice;
        while (true) {
            cout << "Enter the number for your appointment choice: ";
            string input;
            cin >> input;

            if (isNumericString(input)) {
                timeChoice = stoi(input);

                if (timeChoice > 0 && timeChoice <= availableTimesIndex.size()) {
                    int selectedIndex = availableTimesIndex[timeChoice - 1];
                    appointmentTime = doctor.availableTimes[selectedIndex].first + " on " + doctor.availableTimes[selectedIndex].second;
                    doctor.appointmentAvailable[selectedIndex] = false; // Mark this time as booked
                    return true;
                } else {
                    cout << "Invalid choice. Please try again.\n";
                }
            } else {
                cout << "Wrong choice. Please enter a valid number.\n";
            }
        }

        return false;
    }

    int getDoctorsCount() const {
        return doctors.size();
    }

    string getDoctorName(int index) {
        if (index < 0 || index >= doctors.size()) {
            return "Invalid doctor index";
        }
        return doctors[index].name;
    }

    string getDoctorSpecialization(int index) {
        if (index < 0 || index >= doctors.size()) {
            return "Invalid doctor index";
        }
        return doctors[index].specialization;
    }
};

// ================= Bed Management =================
struct BedNode {
    int bedNumber;
    bool isAvailable;
    int height;
    BedNode* left;
    BedNode* right;

    BedNode(int bedNumber)
        : bedNumber(bedNumber), isAvailable(true), height(1), left(nullptr), right(nullptr) {}
};

class BedManagement {
private:
    BedNode* root;
    vector<int> waitingList;

    int getHeight(BedNode* node) {
        return node ? node->height : 0;
    }

    int getBalanceFactor(BedNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    BedNode* rotateRight(BedNode* y) {
        BedNode* x = y->left;
        BedNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    BedNode* rotateLeft(BedNode* x) {
        BedNode* y = x->right;
        BedNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    BedNode* balance(BedNode* node) {
        int balanceFactor = getBalanceFactor(node);

        if (balanceFactor > 1 && getBalanceFactor(node->left) >= 0)
            return rotateRight(node);

        if (balanceFactor < -1 && getBalanceFactor(node->right) <= 0)
            return rotateLeft(node);

        if (balanceFactor > 1 && getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balanceFactor < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    BedNode* addBed(BedNode* node, int bedNumber) {
        if (!node) return new BedNode(bedNumber);

        if (bedNumber < node->bedNumber)
            node->left = addBed(node->left, bedNumber);
        else if (bedNumber > node->bedNumber)
            node->right = addBed(node->right, bedNumber);

        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

        return balance(node);
    }

    BedNode* searchBed(BedNode* node, bool* found) {
        if (!node) return nullptr;

        if (node->isAvailable) {
            *found = true;
            return node;
        }

        BedNode* leftSearch = searchBed(node->left, found);
        if (*found) return leftSearch;

        return searchBed(node->right, found);
    }

public:
    BedManagement() : root(nullptr) {}

    void addBeds(int bedNumber) {
        root = addBed(root, bedNumber);
    }

    void allocateBed(int patientId) {
        bool found = false;
        BedNode* bed = searchBed(root, &found);

        if (found && bed) {
            bed->isAvailable = false;
            cout << "Bed " << bed->bedNumber << " allocated to patient ID " << patientId << " successfully." << endl;
        } else {
            cout << "No beds available. Adding patient ID " << patientId << " to waiting list." << endl;
            waitingList.push_back(patientId);
        }
    }
};


// ================= Billing System =================
struct BillingRecord {
    int patientID;
    double totalAmount;
    bool isPaid;
    string paymentMethod;

    BillingRecord(int id, double amount, const string& payment = "")
        : patientID(id), totalAmount(amount), isPaid(false), paymentMethod(payment) {}

    void displayBill() const {
        cout << "Patient ID: " << patientID << ", Total Amount: " << totalAmount
             << ", Paid: " << (isPaid ? "Yes" : "No")
             << ", Payment Method: " << (isPaid ? paymentMethod : "Not paid yet") << endl;
    }
};

class BillingSystem {
private:
    vector<BillingRecord> maxHeap;
    vector<BillingRecord> paidBills;
    vector<BillingRecord> records;
    
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (maxHeap[parent].totalAmount >= maxHeap[index].totalAmount) {
                break;
            }
            swap(maxHeap[parent], maxHeap[index]);
            index = parent;
        }
    }

    void heapifyDown(int index) {
        int size = maxHeap.size();
        while (index < size) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int largest = index;

            if (leftChild < size && maxHeap[leftChild].totalAmount > maxHeap[largest].totalAmount) {
                largest = leftChild;
            }

            if (rightChild < size && maxHeap[rightChild].totalAmount > maxHeap[largest].totalAmount) {
                largest = rightChild;
            }

            if (largest == index) {
                break;
            }

            swap(maxHeap[index], maxHeap[largest]);
            index = largest;
        }
    }

public:
    void addBillingRecord(int patientID, double totalAmount, const string& paymentMethod) {
        // Check if a billing record for the same patient ID exists
        for (auto& record : maxHeap) {
            if (record.patientID == patientID) {
                record.totalAmount += totalAmount;
                record.paymentMethod = paymentMethod;
                cout << "Billing record updated for Patient ID " << patientID << endl;
                return;
            }
        }
        // If no existing record, create a new one
        maxHeap.emplace_back(patientID, totalAmount, paymentMethod);
        heapifyUp(maxHeap.size() - 1);
    }
    
    void markAsPaid() {
        if (maxHeap.empty()) {
            throw runtime_error("No bills to mark as paid.");
        }

        BillingRecord record = maxHeap[0];
        record.isPaid = true;
        paidBills.push_back(record);

        maxHeap[0] = maxHeap.back();
        maxHeap.pop_back();
        heapifyDown(0);

        cout << "Bill for Patient ID " << record.patientID << " has been marked as paid." << endl;
    }

    void markBillAsPaidByID(int patientID) {
        bool found = false;
        // Search for the bill by Patient ID in the pending bills
        for (auto& record : maxHeap) {
            if (record.patientID == patientID && !record.isPaid) {
                record.isPaid = true;  // Mark as paid
                paidBills.push_back(record);  // Move to the paid bills
                found = true;
                break;  // Exit once the bill is found and processed
            }
        }
    
        if (found) {
            // Remove the bill from pending bills (since it's now marked as paid)
            for (auto it = maxHeap.begin(); it != maxHeap.end(); ++it) {
                if (it->patientID == patientID && !it->isPaid) {
                    maxHeap.erase(it);  // Erase the record from the pending list
                    break;
                }
            }
            cout << "Bill for Patient ID " << patientID << " has been marked as paid." << endl;
        } else {
            cout << "No pending bill found for Patient ID " << patientID << "." << endl;
        }
    }
    
    void displayAllBills() const {
        if (maxHeap.empty()) {
            cout << "No pending bills." << endl;
        } else {
            cout << "Pending Bills:" << endl;
            for (const auto& record : maxHeap) {
                record.displayBill();
            }
        }

        if (paidBills.empty()) {
            cout << "No paid bills." << endl;
        } else {
            cout << "Paid Bills:" << endl;
            for (const auto& record : paidBills) {
                record.displayBill();
            }
        }
    }

    void searchBillByID(int patientID) const {
        bool found = false;
        for (const auto& record : maxHeap) {
            if (record.patientID == patientID) {
                record.displayBill();
                found = true;
            }
        }
        for (const auto& record : paidBills) {
            if (record.patientID == patientID) {
                record.displayBill();
                found = true;
            }
        }
        if (!found) {
            cout << "No bill found for Patient ID " << patientID << "." << endl;
        }
    }
};


string getValidatedPaymentMethod() {
    string paymentMethod;
    while (true) {
        cout << "Enter Payment Method (Card, Insurance, Cash): ";
        cin >> paymentMethod;

        if (paymentMethod == "Card" || paymentMethod == "Insurance" || paymentMethod == "Cash") {
            return paymentMethod;
        } else {
            cout << "Invalid payment method. Please choose from 'Card', 'Insurance', or 'Cash'." << endl;
        }
    }
}

// ================= Medical Records =================
struct MedicalRecord {
    int patientID;
    string name;
    int age;
    string medicalHistory;
    string prescriptions;
    string doctorNotes;
    MedicalRecord* next;
};

class MedicalSystem {
private:
    vector<MedicalRecord> records;

public:
    MedicalRecord* head = nullptr;
    // Function to add a medical record
    void addRecord() {
        MedicalRecord* newRecord = new MedicalRecord;
    
        while (true) {
        cout << "Enter Patient ID: " ;
        cin >> newRecord->patientID;

        if (cin.fail()) {
            cin.clear();  // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
            cout << "Invalid input. Please enter a numeric Patient ID.\n";
            continue;  // Retry the input
        }
        cin.ignore();  // Consume the leftover newline character
        break;
     } // To consume the leftover newline character
    
        cout << "Enter Name: ";
        getline(cin, newRecord->name);
    
        while(true){
            cout << "Enter Age: ";
            cin >> newRecord->age;
            if (cin.fail()) {
                    cin.clear();  // Clear error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                    cout << "Invalid input. Please enter a valid age.\n";
                    continue;  // Retry the input
                }
                cin.ignore();  // Consume the leftover newline character
                break;
            }
    
        cout << "Enter Medical History: ";
        getline(cin, newRecord->medicalHistory);
    
        cout << "Enter Prescriptions: ";
        getline(cin, newRecord->prescriptions);
    
        cout << "Enter Doctor Notes: ";
        getline(cin, newRecord->doctorNotes);
    
        newRecord->next = nullptr;
    
        // Insert at the end of the linked list
        if (head == nullptr) {
            head = newRecord;
        } else {
            MedicalRecord* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newRecord;
        }
    
        cout << "Record added successfully!\n";
    }
    
    void searchRecord() {
        int id;
        while(true){
            cout << "Enter Patient ID to search: ";
            cin >> id;
            if (cin.fail()) {
                    cin.clear();  // Clear error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                    cout << "Invalid input. Please enter a Patient id with valid integers.\n";
                    continue;  // Retry the input
                }
                cin.ignore();  // Consume the leftover newline character
                break;
            }
    
        MedicalRecord* temp = head;
        while (temp != nullptr) {
            if (temp->patientID == id) {
                cout << "\nRecord Found:\n";
                cout << "Name: " << temp->name << "\nAge: " << temp->age << "\nMedical History: " 
                     << temp->medicalHistory << "\nPrescriptions: " << temp->prescriptions 
                     << "\nDoctor Notes: " << temp->doctorNotes << endl;
                return;
            }
            temp = temp->next;
        }
        cout << "Record not found.\n";
    }
    
    // Function to update a medical record by Patient ID
    void updateRecord() {
        int id;
        while(true){
            cout << "Enter Patient ID to update: ";
            cin >> id;
            if (cin.fail()) {
                    cin.clear();  // Clear error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                    cout << "Invalid input. Please enter a Patient id with valid integers.\n";
                    continue;  // Retry the input
                }
                cin.ignore();  // Consume the leftover newline character
                break;
            }
    
        MedicalRecord* temp = head;
        while (temp != nullptr) {
            if (temp->patientID == id) {
                cout << "Enter New Prescriptions: ";
                getline(cin, temp->prescriptions);
    
                cout << "Enter New Doctor Notes: ";
                getline(cin, temp->doctorNotes);
    
                cout << "Record updated successfully!\n";
                return;
            }
            temp = temp->next;
        }
        cout << "Record not found.\n";
    }
    
    void deleteRecord() {
        int id;
        while(true){
        cout << "Enter Patient ID to delete: ";
        cin >> id;
        if (cin.fail()) {
                cin.clear();  // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                cout << "Invalid input. Please enter a Patient id with valid integers.\n";
                continue;  // Retry the input
            }
            cin.ignore();  // Consume the leftover newline character
            break;
        }
    
        MedicalRecord* temp = head;
        MedicalRecord* prev = nullptr;
    
        while (temp != nullptr && temp->patientID != id) {
            prev = temp;
            temp = temp->next;
        }
    
        if (temp == nullptr) {
            cout << "Record not found.\n";
            return;
        }
    
        if (prev == nullptr) { // Deleting the head node
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        delete temp;
        cout << "Record deleted successfully!\n";
    }

    void displayRecords() {
        if (head == nullptr) {
            cout << "No records to display.\n";
            return;
        }
    
        MedicalRecord* temp = head;
        while (temp != nullptr) {
            cout << "\nPatient ID: " << temp->patientID
                 << "\nName: " << temp->name
                 << "\nAge: " << temp->age
                 << "\nMedical History: " << temp->medicalHistory
                 << "\nPrescriptions: " << temp->prescriptions
                 << "\nDoctor Notes: " << temp->doctorNotes
                 << "\n-------------------------\n";
            temp = temp->next;
        }
    }
};

// ================= Staff Management =================
struct Staff {
    int id;
    string name;
    string role;
    string department;
    string shift;
    Staff* next; // Pointer for chaining
};

class StaffManagement {
private:
    vector<Staff> staffList;
     vector<Staff*> table;
    int numEntries; // Number of elements in the table
    const double loadFactorThreshold = 0.75; // Resize when load factor exceeds this threshold
    
    // Hash function for staff ID
    int hashFunction(int id) {
        return id % table.size();
    }

    // Resize the hash table when the load factor is exceeded
    void resizeTable() {
        vector<Staff*> oldTable = table;
        table.resize(oldTable.size() * 2, nullptr);
        numEntries = 0;
        for (Staff* head : oldTable) {
            while (head != nullptr) {
                Staff* temp = head;
                head = head->next;
                addStaff(temp->id, temp->name, temp->role, temp->department, temp->shift);
                delete temp;
            }
        }
    }

public:
    StaffManagement(int initialSize = 10) : numEntries(0) {
        if (initialSize <= 0) {
            throw invalid_argument("Initial size of the table must be greater than zero.");
        }
        table.resize(initialSize, nullptr);
    }

    ~StaffManagement() {
        for (Staff* head : table) {
            while (head != nullptr) {
                Staff* temp = head;
                head = head->next;
                delete temp;
            }
        }
    }
    void addStaff(int id, const string& name, const string& role, const string& department, const string& shift) {
    if (id < 0) {
        throw invalid_argument("ID cannot be negative.");
    }

    // Check for duplicate ID
    int index = hashFunction(id);
    Staff* current = table[index];
    while (current != nullptr) {
        if (current->id == id) {
            cout << "Error: Staff with ID " << id << " already exists." << endl;
            return;
        }
        current = current->next;
    }
    // Add new staff if ID is unique
    Staff* newStaff = new Staff{id, name, role, department, shift, table[index]};
    table[index] = newStaff;
    numEntries++;

    if (static_cast<double>(numEntries) / table.size() > loadFactorThreshold) {
        resizeTable();
    }
}


    void displayStaff() {
        for (size_t i = 0; i < table.size(); i++) {
            Staff* current = table[i];
            while (current != nullptr) {
                cout << "ID: " << current->id
                     << ", Name: " << current->name
                     << ", Role: " << current->role
                     << ", Department: " << current->department
                     << ", Shift: " << current->shift << endl;
                current = current->next;
            }
        }
    }
    
    void searchStaff(int id) {
        if (id < 0) {
            cout << "Invalid ID. Please enter a non-negative integer." << endl;
            return;
        }
        int index = hashFunction(id);
        Staff* current = table[index];
        while (current != nullptr) {
            if (current->id == id) {
                cout << "Staff found: " << endl;
                cout << "ID: " << current->id
                     << ", Name: " << current->name
                     << ", Role: " << current->role
                     << ", Department: " << current->department
                     << ", Shift: " << current->shift << endl;
                return;
            }
            current = current->next;
        }
        cout << "Staff with ID " << id << " not found." << endl;
    }
    
    void deleteStaff(int id) {
        if (id < 0) {
            cout << "Invalid ID. Please enter a non-negative integer." << endl;
            return;
        }
        int index = hashFunction(id);
        Staff* current = table[index];
        Staff* prev = nullptr;
        while (current != nullptr) {
            if (current->id == id) {
                if (prev == nullptr) {
                    table[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                numEntries--;
                cout << "Staff with ID " << id << " has been deleted." << endl;
                return;
            }
            prev = current;
            current = current->next;
        }
        cout << "Staff with ID " << id << " not found." << endl;
    }
    
    
};


// ================= Main System =================
int main() {
    MainMenu:
    PatientList patientList;
    BillingSystem billingSystem;
    MedicalSystem medicalSystem;
    BedManagement bedManagement;
    StaffManagement staffManagement;
    DoctorManagement doctorManagement;

    int choice;
    do {
        cout << "\nWelcome to Hospital Management System\n";
        cout << "1. Manage Staff\n";
        cout << "2. Manage Patients\n";
        cout << "3. Manage Medical Records\n";
        cout << "4. Manage Billing\n";
        cout << "5. Exit\n";
        choice = getValidatedInt("Enter your choice: ");

        switch (choice) {
            case 1: {
                int choice;
                do {
                    cout << "\n----Staff Management System----\n";
                    cout << "1. Add Staff\n";
                    cout << "2. Display Staff\n";
                    cout << "3. Search Staff\n";
                    cout << "4. Delete Staff\n";
                    cout << "5. Back to Main Menu\n";
                    choice = getValidatedInt("Enter your choice: ");
                    
                    switch (choice) {
                        case 1: {
                            int id = getValidatedInt("Enter ID: ");
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear the input buffer
                            string name = getValidatedString("Enter Name: ");
                            string role = getValidatedRole("Enter Role: ");
                            string department = getValidatedString("Enter Department: ");
                            string shift = getValidatedString("Enter Shift: ");
                            try {
                                staffManagement.addStaff(id, name, role, department, shift);
                                cout << "Staff added successfully." << endl;
                            } catch (const exception& e) {
                                cout << "Error: " << e.what() << endl;
                            }
                            break;
                        }
                        case 2:
                            staffManagement.displayStaff();
                            break;
                        case 3: {
                            int id = getValidatedInt("Enter ID to search: ");
                            staffManagement.searchStaff(id);
                            break;
                        }
                        case 4: {
                            int id = getValidatedInt("Enter ID to delete: ");
                            staffManagement.deleteStaff(id);
                            break;
                        }
                        case 5:
                            goto MainMenu;   
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                } while (choice != 5);
                break;
            }
            
            case 2: {
                for (int i = 1; i <= 5; i++) {
                    bedManagement.addBeds(i);
                }

                int patientCounter = 1;
                while (true) {
                    string name, condition;
                    string ageInput;
                    int age;

                    cout << "\nEnter Patient Name (or 'back' to go to main Menu): ";
                    cin >> name;

                    if (name == "back") break;

                    if (!isAlphaString(name)) {
                        cout << "Please enter a valid name." << endl;
                        continue;
                    }

                    while (true) { 
                        cout << "Enter Patient Age: ";
                        cin >> ageInput;

                        try {
                            if (!isNumericString(ageInput)) {
                                throw invalid_argument("Age must be numeric.");
                            }

                            age = stoi(ageInput);
                            if (age <= 0 || age > 110) {
                                throw out_of_range("Age must be between 1 and 110.");
                            }

                            break; // Age input is valid, break the loop

                        } catch (const invalid_argument& e) {
                            cout << "Invalid input: " << e.what() << ". Please enter age again." << endl;
                        } catch (const out_of_range& e) {
                            cout << "Out of range error: " << e.what() << ". Please enter age again." << endl;
                        }
                    }

                    cout << "Enter Condition (s for severe, ns for not_severe): ";
                    cin >> condition;

                    if (condition == "s") {
                        // For severe patients, admit them and allocate a bed
                        patientList.admitPatient(patientCounter, name, age, "severe", "", "");
                        bedManagement.allocateBed(patientCounter);
                        cout << "Patient " << name << " admitted successfully." << endl;
                        ++patientCounter; // Increment the patient counter
                    } else if (condition == "ns") {
                        // For non-severe patients, show doctor options and schedule an appointment
                        doctorManagement.showDoctors();

                        int doctorChoice;

                        while (true) {
                            string doctorChoiceInput;

                            cout << "Select a doctor by number: ";
                            cin >> doctorChoiceInput;

                            try {
                                if (!isNumericString(doctorChoiceInput)) {
                                    throw invalid_argument("Doctor choice must be numeric.");
                                }

                                doctorChoice = stoi(doctorChoiceInput);

                                if (doctorChoice > 0 && doctorChoice <= doctorManagement.getDoctorsCount()) {
                                    string appointmentTime;
                                    if (doctorManagement.allocateDoctorAppointment(doctorChoice - 1, appointmentTime)) {
                                        // Schedule appointment for non-severe patients
                                        string selectedDoctorName = doctorManagement.getDoctorName(doctorChoice - 1);
                                        string selectedDoctorSpecialization = doctorManagement.getDoctorSpecialization(doctorChoice - 1);
                                        patientList.admitPatient(patientCounter, name, age, "not_severe", selectedDoctorName, appointmentTime);
                                        cout << "Appointment scheduled with " << selectedDoctorName << " (" << selectedDoctorSpecialization << ") at " << appointmentTime << endl;
                                        ++patientCounter; // Increment patient counter
                                        break;
                                    }
                                } else {
                                    cout << "Invalid doctor choice. Please choose again." << endl;
                                }
                            } catch (const invalid_argument& e) {
                                cout << "Invalid input: " << e.what() << ". Please choose a valid doctor." << endl;
                            }
                        }
                    } else {
                        cout << "Invalid condition entered. Please try again.\n";
                    }
                }
                break;
            }
            
            case 3: {
                int choice;
                do {
                    cout << "\n----Medical Records----";
                    cout << "\n1. Add Record";
                    cout << "\n2. Search Record";
                    cout << "\n3. Update Record";
                    cout << "\n4. Delete Record";
                    cout << "\n5. Display All Records";
                    cout << "\n6. Back to Main Menu";
                    choice = getValidatedInt("\nEnter your choice: ");
            
                    switch (choice) {
                        case 1: {
                            medicalSystem.addRecord(); 
                            break;
                        }
                        case 2: {
                            medicalSystem.searchRecord(); 
                            break;
                        }
                        case 3: {
                            medicalSystem.updateRecord(); 
                            break;
                        }
                        case 4: {
                            medicalSystem.deleteRecord(); 
                            break;
                        }
                        case 5: {
                            medicalSystem.displayRecords(); 
                            break;
                        }
                        case 6: {
                            break;
                        }
                        default: cout << "Invalid choice. Please try again.\n";
                    }
                } while (choice != 6);
                break;
            }

           case 4: {
            int choice;
            while (true) {
                try {
                    cout << "\n----Billing System----" << endl;
                    cout << "1. Add Billing Record" << endl;
                    cout << "2. Mark Top Bill as Paid" << endl;
                    cout << "3. Display All Bills" << endl;
                    cout << "4. Search Bill by Patient ID" << endl;
                    cout << "5. Mark Bill as Paid by Patient ID" << endl;
                    cout << "6. Back to Main Menu" << endl;
                    choice = getValidatedInt("Enter your choice: ");
        
                    switch (choice) {
                        case 1: {
                            int patientID = getValidatedInt("Enter Patient ID: ");
                            double totalAmount = getValidatedDouble("Enter Total Amount: ");
                            string paymentMethod = getValidatedPaymentMethod();
                            billingSystem.addBillingRecord(patientID, totalAmount, paymentMethod);
                            break;
                        }
                        case 2:
                            billingSystem.markAsPaid();
                            break;
                        case 3:
                            billingSystem.displayAllBills();
                            break;
                        case 4: {
                            int patientID = getValidatedInt("Enter Patient ID to search: ");
                            billingSystem.searchBillByID(patientID);
                            break;
                        }
                        case 5: {
                            int patientID = getValidatedInt("Enter Patient ID to mark as paid: ");
                            billingSystem.markBillAsPaidByID(patientID);
                            break;
                        }
                        case 6:
                            // Return to main menu
                            goto MainMenu;
                        default:
                            cout << "Invalid choice. Please select a valid option." << endl;
                    }
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
            }
            break;
        }

            case 5:
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 5);

    return 0;
}

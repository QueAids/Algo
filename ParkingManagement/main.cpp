/**
Overview of code (Data Structure main use):
2D array: Represents parking spots                            - done
LinkedList: for vehicle logs,  also for searching             - done
Queue: if parking is full, vehicle goes to a waiting queue.   - done
Stack: to track recently vacated spots                        - done
**/

#include <fstream>
#include <iostream>
using namespace std;

// Linked list for vehicle logs
struct VehicleLog {
  string plateNum;
  string slotNumber;
  VehicleLog *next;
};
VehicleLog *logHead = NULL;

// Queue for waiting vehicles
struct Node {
  string plateNum;
  Node *next;
};
Node *front = NULL;
Node *rear = NULL;

// 2D Array for parking slots
const int rows = 2;
const int cols = 3;
string ParkingArray[rows][cols];

// Stack to track recently vacated spots
struct StackNode {
  string slotNumber;
  StackNode *next;
};
StackNode *stackTop = NULL;

// Function prototypes
void initializeParkingLot();
void ParkVehicle(string plateNum);
void RetrieveVehicle(string plateNum);
void DisplayAvailable();
void DisplayQueue();
void DisplaySlotStatus();
void SearchLicensePlate(string plateNum);
void DisplayStack();
bool isFull();
bool isEmpty();
void systemClear();
void logVehicle(string plateNum, string slotNumber);
void removeLog(string plateNum);
void writeLogToFile(const string &entry);
void writeCurrentParkedVehiclesToFile();
void loadCurrentParkedVehiclesFromFile();
string custom_to_string(int num);
string custom_string_concat(const string &str1, const string &str2);
void pop();
void push(string slotNumber);
bool isStackEmpty();
string topStack();
void enqueue(string plateNum);
void dequeue();

int main() {
  int choice;
  string plateNum;

  initializeParkingLot();
  while (true) {
    cout << "\nParking Lot Management System\n";
    cout << "1. Park a Vehicle\n";
    cout << "2. Retrieve a Vehicle\n";
    cout << "3. Display Available Spots\n";
    cout << "4. Display Waiting Queue\n";
    cout << "5. Display Slot Status\n";
    cout << "6. Search for a License Plate\n";
    cout << "7. Display Recently Vacated Spots\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    // For Error Handling
    if (cin.fail()) {
      cin.clear(); // Clear error flags
      char c;
      while (cin.get(c) && c != '\n')
        ; // Ignore characters until newline
      cout << "Invalid input. Please enter a valid option.\n";
      continue;
    }

    switch (choice) {
    case 1:
      cout << "Enter plate number: ";
      cin >> plateNum;
      ParkVehicle(plateNum);
      break;
    case 2:
      cout << "Enter plate number to retrieve: ";
      cin >> plateNum;
      RetrieveVehicle(plateNum);
      break;
    case 3:
      DisplayAvailable();
      break;
    case 4:
      DisplayQueue();
      break;
    case 5:
      DisplaySlotStatus();
      break;
    case 6:
      cout << "Enter plate number to search: ";
      cin >> plateNum;
      SearchLicensePlate(plateNum);
      break;
    case 7:
      DisplayStack();
      break;
    case 8:
      cout << "Exiting...\n";
      return 0;
    default:
      cout << "\nInvalid. Please try again.\n";
    }
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
    systemClear();
  }
  return 0;
}

//------------------------------------------------------------------
void initializeParkingLot() {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      ParkingArray[i][j] = "EMPTY";
    }
  }
  loadCurrentParkedVehiclesFromFile();
}

void ParkVehicle(string plateNum) {
  if (isFull()) {
    cout << "Parking lot is full. Adding vehicle to the waiting queue.\n";
    enqueue(plateNum);
  } else {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        if (ParkingArray[i][j] == "EMPTY") {
          ParkingArray[i][j] = plateNum; // Park the vehicle
          string slotNumber =
              custom_string_concat(string(1, 'A' + i), custom_to_string(j + 1));
          logVehicle(plateNum, slotNumber); // Log the vehicle
          writeLogToFile(custom_string_concat(
              "Parked: ",
              custom_string_concat(
                  plateNum, custom_string_concat(" at slot ", slotNumber))));
          writeCurrentParkedVehiclesToFile();
          cout << "Vehicle with plate number " << plateNum
               << " is parked at slot " << slotNumber << ".\n";
          return;
        }
      }
    }
  }
}

void RetrieveVehicle(string plateNum) {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (ParkingArray[i][j] == plateNum) {
        string slotNumber =
            custom_string_concat(string(1, 'A' + i), custom_to_string(j + 1));
        ParkingArray[i][j] = "EMPTY"; // Vacate the parking spot
        removeLog(plateNum);
        writeLogToFile(custom_string_concat(
            "Retrieved: ",
            custom_string_concat(
                plateNum, custom_string_concat(" from slot ", slotNumber))));
        writeCurrentParkedVehiclesToFile();
        cout << "Vehicle with plate number " << plateNum
             << " retrieved from slot " << slotNumber << ".\n";

        // Push vacated slot to stack
        push(slotNumber);

        if (!isEmpty()) {
          dequeue();
        }
        return;
      }
    }
  }
  cout << "Vehicle with plate number " << plateNum
       << " not found in the parking lot.\n";
}

void DisplayAvailable() {
  cout << "\nParking Lot Status:\n";
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      string slotNumber =
          custom_string_concat(string(1, 'A' + i), custom_to_string(j + 1));
      if (ParkingArray[i][j] == "EMPTY") {
        cout << slotNumber << " [EMPTY] ";
      } else {
        cout << slotNumber << " [" << ParkingArray[i][j] << "] ";
      }
    }
    cout << "\n";
  }
}

void DisplayQueue() {
  if (isEmpty()) {
    cout << "The waiting queue is empty.\n";
    return;
  }
  cout << "\nWaiting Queue:\n";
  Node *current = front;
  int count = 0;
  while (current != NULL) {
    cout << ++count << ". " << current->plateNum << "\n";
    current = current->next;
  }
  cout << "Total vehicles waiting: " << count << "\n";
}

void DisplaySlotStatus() {
  int available = 0, occupied = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (ParkingArray[i][j] == "EMPTY") {
        ++available;
      } else {
        ++occupied;
      }
    }
  }
  cout << "\nParking Slot Status:\n";
  cout << "Available slots: " << available << endl;
  cout << "Occupied slots: " << occupied << endl;
}

void SearchLicensePlate(string plateNum) {
  VehicleLog *current = logHead;

  while (current != NULL) {
    if (current->plateNum == plateNum) {
      cout << "License plate " << plateNum << " is parked at slot "
           << current->slotNumber << ".\n";
      return;
    }
    current = current->next;
  }
  cout << "License plate " << plateNum << " is not found in the parking lot.\n";
}

void DisplayStack() {
  if (isStackEmpty()) {
    cout << "No recently vacated spots.\n";
    return;
  }
  cout << "\nRecently Vacated Spots:\n";
  StackNode *current = stackTop;
  int count = 0;
  while (current != NULL) {
    cout << ++count << ". " << current->slotNumber << "\n";
    current = current->next;
  }
  cout << "Total recently vacated spots: " << count << "\n";
}

//------------------------------Queue---------------------------------
void enqueue(string plateNum) {
  Node *temp = new Node;
  temp->plateNum = plateNum;
  temp->next = NULL;
  if (rear == NULL) {
    front = rear = temp;
  } else {
    rear->next = temp;
    rear = temp;
  }
  cout << "Vehicle with plate number " << plateNum
       << " added to the waiting queue.\n";
}

void dequeue() {
  if (front == NULL) {
    cout << "Queue is empty.\n";
    return;
  }
  string plateNum = front->plateNum;
  Node *temp = front;
  front = front->next;
  if (front == NULL) {
    rear = NULL;
  }
  delete temp;
  cout << "Vehicle with plate number " << plateNum
       << " removed from the waiting queue and parked.\n";
  ParkVehicle(plateNum);
}

bool isFull() {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (ParkingArray[i][j] == "EMPTY") {
        return false;
      }
    }
  }
  return true;
}

bool isEmpty() { return front == NULL; }
//--------------------------------------------------------------------------

void logVehicle(string plateNum, string slotNumber) {
  VehicleLog *newLog = new VehicleLog;
  newLog->plateNum = plateNum;
  newLog->slotNumber = slotNumber;
  newLog->next = logHead;
  logHead = newLog;
}

void removeLog(string plateNum) {
  VehicleLog *current = logHead;
  VehicleLog *prev = NULL;

  while (current != NULL && current->plateNum != plateNum) {
    prev = current;
    current = current->next;
  }

  if (current == NULL) {
    return;
  }

  if (prev == NULL) {
    logHead = current->next;
  } else {
    prev->next = current->next;
  }

  delete current;
}
//--------------------------File handling--------------------------------
void writeLogToFile(const string &entry) {
  ofstream logFile("parking_log.txt", ios::app);
  if (logFile.is_open()) {
    logFile << entry << endl;
    logFile.close();
  } else {
    cout << "Unable to open log file.\n";
  }
}

void writeCurrentParkedVehiclesToFile() {
  ofstream currentParkedFile("current_parked_vehicles.txt");
  if (currentParkedFile.is_open()) {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        if (ParkingArray[i][j] != "EMPTY") {
          string slotNumber =
              custom_string_concat(string(1, 'A' + i), custom_to_string(j + 1));
          currentParkedFile << ParkingArray[i][j] << " at slot " << slotNumber
                            << "\n";
        }
      }
    }
    currentParkedFile.close();
  } else {
    cout << "Unable to open current parked vehicles file.\n";
  }
}

void loadCurrentParkedVehiclesFromFile() {
  ifstream currentParkedFile("current_parked_vehicles.txt");
  if (currentParkedFile.is_open()) {
    string line;
    while (getline(currentParkedFile, line)) {
      size_t atPos = line.find(" at slot ");
      if (atPos != string::npos) {
        string plateNum = line.substr(0, atPos);    // Extract plate number
        string slotNumber = line.substr(atPos + 9); // Extract slot number
        int row = slotNumber[0] - 'A'; // Convert row letter to index
        int col =
            stoi(slotNumber.substr(1)) - 1; // Convert column number to index
        ParkingArray[row][col] = plateNum;  // Updates the parking array
        logVehicle(plateNum, slotNumber);
      }
    }
    currentParkedFile.close();
  } else {
    cout << "No previous parking data found. Starting fresh.\n";
  }
}

//-------------------------------String--------------------------------------
string custom_to_string(int num) { // to_string replacement
  string result = "";
  bool isNegative = (num < 0);
  if (isNegative) {
    num = -num;
  }
  do {
    result = char('0' + (num % 10)) + result;
    num /= 10;
  } while (num > 0);
  if (isNegative) {
    result = '-' + result;
  }
  return result;
}

string custom_string_concat(const string &str1,
                            const string &str2) { // string concat replacement
  string result = str1;
  for (char c : str2) {
    result += c;
  }
  return result;
}

//---------------------------Stack-----------------------------
void push(string slotNumber) {
  StackNode *newNode = new StackNode;
  newNode->slotNumber = slotNumber;
  newNode->next = stackTop;
  stackTop = newNode;
}

void pop() {
  if (stackTop == NULL) {
    cout << "Stack is empty.\n";
    return;
  }
  StackNode *temp = stackTop;
  stackTop = stackTop->next;
  delete temp;
}

bool isStackEmpty() { return stackTop == NULL; }

string topStack() {
  if (!isStackEmpty()) {
    return stackTop->slotNumber;
  }
  return "";
}
//-------------------------------------------------------------

void systemClear() {
    #ifdef _WIN32
        system("cls"); // Clear screen on Windows
    #else
        system("clear"); // Clear screen on Unix/Linux/MacOS
    #endif
}

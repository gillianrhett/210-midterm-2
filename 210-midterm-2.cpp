#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

const int MIN_NR = 0, MAX_NR = 98; // I used these to pick random names - the text file had 99 names
const int MIN_LS = 5, MAX_LS = 20; // I didn't use these
const int P_ORDER = 40, P_NEW = 60, P_END_LEAVES = 20, P_ANY_LEAVES = 10, P_VIP = 10;
const int MINUTES = 20;
/*
A customer being helped at the beginning of the line and ordering their coffee is 40%
A new customer joining the end of the line is 60%
The customer at the end of the line deciding they don't want to wait and leaving is 20%
Any particular customer can decide they don't want to wait and leave the line: 10%
A VIP (very important person) customer with a Coffee House Gold Card gets to skip the line and go straight to the counter and order: 10%
*/

class DoublyLinkedList {
private:
    struct Node {
        int data;
        Node* prev;
        Node* next;
        Node(int val, Node* p = nullptr, Node* n = nullptr) {
            data = val; 
            prev = p;
            next = n;
        }
    };

    Node* head;
    Node* tail;

public:
    DoublyLinkedList() { head = nullptr; tail = nullptr; }

    void insert_after(int value, int position) {
        if (position < 0) {
            cout << "Position must be >= 0." << endl;
            return;
        }

        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
            return;
        }

        Node* temp = head;
        for (int i = 0; i < position && temp; ++i)
            temp = temp->next;

        if (!temp) {
            cout << "Position exceeds list size. Node not inserted.\n";
            delete newNode;
            return;
        }

        newNode->next = temp->next;
        newNode->prev = temp;
        if (temp->next)
            temp->next->prev = newNode;
        else
            tail = newNode;
        temp->next = newNode;
    }

    void delete_val(int value) {
        if (!head) return;

        Node* temp = head;
        
        while (temp && temp->data != value)
            temp = temp->next;

        if (!temp) return; 

        if (temp->prev)
            temp->prev->next = temp->next;
        else
            head = temp->next; 

        if (temp->next)
            temp->next->prev = temp->prev;
        else
            tail = temp->prev; 

        delete temp;
    }

    void delete_pos(int pos) {
        if (!head) {
            cout << "List is empty." << endl;
            return;
        }
    
        if (pos == 1) {
            pop_front();
            return;
        }
    
        Node* temp = head;
    
        for (int i = 1; i < pos; i++){
            if (!temp) {
                cout << "Position doesn't exist." << endl;
                return;
            }
            else
                temp = temp->next;
        }
        if (!temp) {
            cout << "Position doesn't exist." << endl;
            return;
        }
    
        if (!temp->next) {
            pop_back();
            return;
        }
    
        Node* tempPrev = temp->prev;
        tempPrev->next = temp->next;
        temp->next->prev = tempPrev;
        delete temp;
    }

    void push_back(int v) {
        Node* newNode = new Node(v);
        if (!tail)
            head = tail = newNode;
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }
    
    void push_front(int v) {
        Node* newNode = new Node(v);
        if (!head)
            head = tail = newNode;
        else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
    }
    
    void pop_front() {

        if (!head) {
            cout << "List is empty." << endl;
            return;
        }

        Node * temp = head;

        if (head->next) {
            head = head->next;
            head->prev = nullptr;
        }
        else
            head = tail = nullptr;
        delete temp;
    }

    void pop_back() {
        if (!tail) {
            cout << "List is empty." << endl;
            return;
        }
        Node * temp = tail;

        if (tail->prev) {
            tail = tail->prev;
            tail->next = nullptr;
        }
        else
            head = tail = nullptr;
        delete temp;
    }

    ~DoublyLinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void print() {
        Node* current = head;
        if (!current) {
            cout << "List is empty." << endl;
            return;
        }
        while (current) {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }

    void print_reverse() {
        Node* current = tail;
        if (!current) { 
            cout << "List is empty." << endl;
            return;
        }
        while (current) {
            cout << current->data << " ";
            current = current->prev;
        }
        cout << endl;
    }

    // I couldn't think of a way to access an individual data value without making a new member function
    // the only alternative I could think of was make the nodes store strings instead of ints, but keeping ints was easier for the random names
    int get_data(int position) { // this is a position, not an index, so 1 means the first item
        if (!head)
            return 0;
        Node* curr = head;
        int i = 1;
        while (curr && i < position) {
            curr = curr->next;
            ++i;
        }
        return curr->data;
    }
};

int main() {
    srand(time(0));
    DoublyLinkedList line;
    vector<string> names; // the line's nodes will store ints corresponding to the index values of the names
    ifstream inFile;
    string name;
    inFile.open("names.txt");
    while (!inFile.eof()){
        getline(inFile, name);
        names.push_back(name);
    }
    inFile.close();

    // When the store opens (i.e. in first time period), add 5 customers to the line right away.
    int rNum;
    cout << "Store opens:" << endl;
    int lineSize = 0;
    for (int i = 1; i < 6; ++i) {
        rNum = rand() % (MAX_NR - MIN_NR + 1) + MIN_NR;
        line.push_back(rNum);
        ++lineSize;
        cout << "\t" << names.at(line.get_data(i)) << " joins the line" << endl;
    }
    cout << "\tResulting line:" << endl;
    for (int i = 1; i <= lineSize; ++i)
        cout << "\t\t" << names.at(line.get_data(i)) << endl;

    int prob; // for probability calculations
    bool VIP = false; // keep track of VIP because they get served in the next minute

    for (int i = 2; i <= MINUTES; ++i) { // start at 2 like the example because opening was the first minute
        cout << "Time step #" << i << ":" << endl;
        
        // if a VIP arrived in the previous minute, they get served now
        if (VIP) {
            cout << "\t" << names.at(line.get_data(1)) << " is served" << endl;
            line.pop_front(); // if they ordered, they are done and can leave
            --lineSize;
            VIP = false;
        }
        
        // does the first customer order and get served?
        // I'm assuming this can happen in the same minute as a VIP gets served
        prob = rand() % 100 + 1;  // returns random number 1-100
        if (prob <= P_ORDER && lineSize > 0) {
            cout << "\t" << names.at(line.get_data(1)) << " is served" << endl;
            line.pop_front(); // if they ordered, they are done and can leave
            --lineSize;
        }
        
        // does a new customer join the queue?
        prob = rand() % 100 + 1;
        if (prob <= P_NEW) {
            // pick a new random index for a name
            rNum = rand() % (MAX_NR - MIN_NR + 1) + MIN_NR;
            line.push_back(rNum);
            ++lineSize;
            cout << "\t" << names.at(rNum) << " joins the line" << endl;
        }
        
        // does the customer at the end of the queue leave?
        prob = rand() % 100 + 1;
        if (prob <= P_END_LEAVES && lineSize > 0) {
            cout << "\t" << names.at(line.get_data(lineSize)) << " (at the rear) left the line" << endl;
            line.pop_back();
            --lineSize;
        }
        
        // does any customer leave?
        prob = rand() % 100 + 1;
        if (prob <= P_ANY_LEAVES && lineSize > 0) {
            rNum = rand() % lineSize + 1; // pick a random customer
            cout  << "\t" << names.at(line.get_data(rNum)) << " left the line" << endl;
            line.delete_pos(rNum);
            --lineSize;
        }

        // does a VIP arrive? 
        prob = rand() % 100 + 1;
        if (prob <= P_VIP) {
            rNum = rand() % (MAX_NR - MIN_NR + 1) + MIN_NR;
            line.push_front(rNum);
            ++lineSize;
            cout << "\t" << names.at(rNum) << " (VIP) joins the front of the line" << endl;
            VIP = true;
        }

        cout << "\tResulting line:" << endl;
        for (int i = 1; i <= lineSize; ++i)
            cout << "\t\t" << names.at(line.get_data(i)) << endl;        
    }

    return 0;
}
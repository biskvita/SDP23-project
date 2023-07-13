#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <queue>

using std::cout;
using std::cin;
using std::endl;

////////////////////////////////

template<typename T>
struct Node
{
    T value;
    Node* next;
    std::vector<Node*> skipTo;
};

template<typename T>
class SkipList
{
private:
    Node<T>* first{nullptr};
    Node<T>* last{nullptr};
public:
    SkipList() = default;
    SkipList(const SkipList& other) = delete;
    SkipList& operator=(const SkipList& other) = delete; 

    ~SkipList()
    {
        Node<T>* current = this->first;

        while (current != nullptr) //traverse
        {
            Node<T>* temp = current;

            if (current->skipTo.size() != 0) //delete pointers in skipTo vector
            {
                for (Node<T>* node : current->skipTo)
                {
                    delete node; 
                }

                current->skipTo.clear();
            } 

            delete current; //delete current
            current = temp->next; //go to next node
        }
        
        first = nullptr; //set first and last to nullptr
        last = nullptr;
    } 

    void push_back(const T& value)
    {
        Node<T>* temp = new Node<T>(); //create a node
        temp->value = value;

        if (last != nullptr) //if the list is not empty
        {
            this->last->next = temp;
        }
        if (this->first == nullptr) //if the list is empty
        {
            assert(this->last == nullptr);
            this->first = temp;
        }

        this->last = temp; //set last as the new node
    }

    void insertSkip(const T& node, const T& wanted) //node = where, wanted = which/what
    {
        Node<T>* current = first;
        Node<T>* current2 = first; 

        while (current->value != node) //traverse to the node that will add the skip
        {
            current = current->next; 
        }
        
        while (current2->value != wanted) //traverse to the node that will be added to the skipTo vector
        {
            current2 = current2->next;
        }
         
        assert(current2 != nullptr);
        assert(current != nullptr);

        if (current->next->value == current2->value) //if the next node of current is the wanted 
        {
            cout << current->value << " and " << current2->value << " are already connected..." << endl;
            return; 
        }

        for (Node<T>* node : current->skipTo)
        {
            if (node->value == current2->value) //if the wanted is already in the vector
            {
                cout << "Direct path already exists..." << endl;
                return; 
            }
        }

        current->skipTo.push_back(current2); //add to the vactor
    }

    void print()
    {
        Node<T>* current = this->first;

        while (current != nullptr)
        {
            cout << current->value ;

            if (current->skipTo.size() != 0)
            {
                cout << "( " ;
                for (Node<T>* node : current->skipTo)
                {
                    cout << node->value << " ";
                }
                cout << ")";
            }

            cout << endl;
            current = current->next; 
        } 
    }   
    
    int getIndex(const T& wanted) //get index of node
    {
        int num = 0;
        Node<T>* current = this->first;

        while (current->value != wanted) //traverse with counter
        {
            num++;
            current = current->next;
        }
        
        return num; 
    }
    
    bool isPossibleDirectSkip(Node<T>* current, const T& wanted) //check if the wanted node is in the current's skipTo vector
    {
        for (Node<T>* node : current->skipTo)
        {
            if (node->value == wanted)
            {
                return true;   
            }
        }

        return false; 
    }

    bool isPossibleIndirectSkip(Node<T>* current, const T& end) //check if we can skip to a node and still get to the wanted
    {
        for (Node<T>* node : current->skipTo)
        {     
            if (node->next)
            {
                if ((node->next->value == end) || (node->next->value != end && getIndex(node->value) < getIndex(end)))
                {
                    return true; 
                }
            }
        }

        return false; 
    }
    
    T getIndirectSkip(Node<T>* current, const T& end) //get the right node from skipTo vector
    {
        int num = getIndex(end);
        int max = 0;

        for (Node<T>* node : current->skipTo)
        {
            if (getIndex(node->value) < num) 
            {
                if (getIndex(node->value) > max) //get the node with the index that is closest to the wanted 
                {
                    max = getIndex(node->value);
                } 
            }
        }

        for (Node<T>* node : current->skipTo)
        {
            if (getIndex(node->value) == max) //return the node with the max index
            {
                return node->value;
            }
        }

        return nullptr; 
    }

    void getShortestPath(const T& end, Node<T>* current, std::vector<T>& res)
    {
        res.push_back(current->value); //add to result

        if (current->value == end) //end of recursion
        {
            return; 
        }

        if (current->skipTo.size() > 0 && isPossibleDirectSkip(current, end)) //if we have direct skip to the end
        {
            while (current->value != end)
            {
                current = current->next;
            }
            
            getShortestPath(end, current, res);
        }
        else if (current->skipTo.size() > 0 && isPossibleIndirectSkip(current, end)) //if there is a indirect skip to the end
        {
            T temp = getIndirectSkip(current, end);

            while (current->value != temp)
            {
                current = current->next;
            }
            
            getShortestPath(end, current, res);
        }
        else //if there are no skips, traverse 
        {
            current = current->next;
            getShortestPath(end, current, res);
        }
        
    }
    
    std::vector<T> getTravelRoute(std::queue<T> travelList)
    {
        std::vector<T> res;
        Node<T>* current = this->first;

        while (!travelList.empty())
        {
            T dest = travelList.front(); //get wanted place
            travelList.pop();
            getShortestPath(dest, current, res); //get the shortest path to the place from current node (from start)
            res.pop_back(); //pop so we do not have reocurrences 

            while (current->value != dest) //traverse the current node the place 
            {
                current = current->next;
            } 
        }

        while (current != nullptr) //add the rest of the list so we can get to the final city
        {
            res.push_back(current->value);
            current = current->next;
        }
          
        return res; 
    }
};

std::queue<std::string> getArguments(const std::string& input) //helper function for main
{
	size_t i = 0;
    size_t size = input.size(); 
    std::queue<std::string> storage; 

    while (i < size)
    {
        if (i < size && input[i] != ' ')
        {
            std::string tempStr = "";
            while (i < size && input[i] != ' ')
            {
                tempStr.push_back(input.at(i));
                ++i;
            }

            storage.push(tempStr);
        }
        else
        {
            ++i; 
        }
    } 

    return storage;
}

int main()
{
    SkipList<std::string> list; 
    std::string station;
    unsigned int n;
    cout << "N: ";
    cin >> n;

    for (size_t i = 0; i < n; i++)
    {
        cin >> station;
        list.push_back(station);
        station.clear();
    }
    
    std::string line; 
    std::string temp;
    cout << "#################################################" << endl;
    cout << "Direct lines (end with CTRL+Z):" << endl;
    cin.ignore();

    while (std::getline(std::cin, line))
    {
        int pos = line.find(" "); 
	    temp = line.substr(0, pos); 
        line = line.substr(pos+1);

        list.insertSkip(temp, line);
    }

    line.clear();
    cin.clear();
    cout << "List of cities: " ;
    std::getline(cin, line);
    std::queue<std::string> travellist = getArguments(line); 

    cout << "#################################################" << endl;
    std::vector<std::string> res = list.getTravelRoute(travellist);

    for (std::string str : res)
    {
        cout << str << " ";
    }
    
    cout << endl;

    return 0;
}

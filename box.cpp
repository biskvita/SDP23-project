#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

using std::cout;
using std::cin;
using std::endl;

////////////////////////////////

struct Box
{
    std::vector<std::string> souvenirs;
    std::vector<std::string> boxes; 
};

class BoxHandler
{
private:
    std::unordered_map<std::string, Box> umap; 
public:
    BoxHandler(std::string filename)
    {
        std::ifstream file(filename, std::ios::in); //open file

        if (!file.is_open()) 
        {
            throw std::invalid_argument("no file");
        }
        
        int num;
        int count = 0;

        file >> num; //get the rows

        while (count < num)
        {
            std::string name;
            std::string temp;
            int numS, numB;
            int count2 = 0;
            std::vector<std::string> souv;
            std::vector<std::string> box;

            file >> name >> numS; //get name of box and the number of souvenirs

            while (count2 < numS) //get the souvenirs 
            {
                file >> temp;
                souv.push_back(temp);
                temp.clear();
                ++count2;
            }

            file >> numB; //get the number of boxes 
            count2 = 0;

            while (count2 < numB) //get the boxes
            {
                file >> temp;
                box.push_back(temp);
                temp.clear();
                ++count2;
            }
            
            umap[name] = Box(); //create a new box
            umap[name].boxes = box;
            umap[name].souvenirs = souv;
            count++;
        }

        file.close(); //close file
    } 

    void print()
    {
        for (std::pair<std::string, Box> x : umap)
        {
            cout << x.first << ": "; 

            for (std::string y : x.second.souvenirs)
            {
                cout << y << " ";
            }
            cout << "| ";

            for (std::string y : x.second.boxes)
            {
                cout << y << " ";
            }
            cout << endl;
        }
    }   

    void deleteFromOthers(const std::string& key, const std::string& toDelete)
    {
        std::vector<std::string> temp;
        std::vector<std::string> tempS;

        for (std::pair<std::string, Box> x : this->umap)
        {
            if (key == x.first) //find the parent of the empty box
            {
                int size = x.second.boxes.size();

                for (int i = 0; i < size; ++i) //get its boxes without the empty one
                {
                    if (x.second.boxes[i] != toDelete)
                    {
                        temp.push_back(x.second.boxes[i]);
                    }
                }

                size = x.second.souvenirs.size();

                for (int i = 0; i < size; ++i) //get its souvenirs
                {
                    tempS.push_back(x.second.souvenirs[i]);
                }
            }
        }

        this->umap.erase(key); //delete the parent
        this->umap[key] = Box(); //create a new parent with the updated data 
        this->umap[key].boxes = temp;
        this->umap[key].souvenirs = tempS; 
    }
    
    void removeEmptyBoxes()
    {
        std::vector<std::string> toDelete;
        std::vector<std::string> bigger; 
        
        for (std::pair<std::string, Box> x : this->umap)
        {
            if (x.second.boxes.size() == 0 && x.second.souvenirs.size() == 0) //get tha empty boxes 
            {
                toDelete.push_back(x.first);
            }
        }

        for (std::string str : toDelete)
        {
            for (std::pair<std::string, Box> x : this->umap)
            {
                for (std::string y : x.second.boxes)
                {
                    if (str == y) //find the empty boxes' parent
                    {
                        bigger.push_back(x.first);
                    }
                }
            }
        }

        int size = toDelete.size();

        for (int i = 0; i < size; ++i) //delete the empty box from the parent's box vector
        {
            deleteFromOthers(bigger[i], toDelete[i]);
        }
        
        for (std::string x : toDelete) //delete the empty box
        {
            this->umap.erase(x);
        }
    }

    void changeParentBox(const std::string& parent, const std::string& child, const std::string& key)
    {
        std::string copyChild = child; //get a copy of child 
        deleteFromOthers(key, copyChild); //delete the child
        
        std::vector<std::string> temp;
        std::vector<std::string> tempS;

        for (std::pair<std::string, Box> x : this->umap)
        {
            if (parent == x.first) //find the parent 
            {
                int size = x.second.boxes.size();

                for (int i = 0; i < size; ++i) //get its boxes
                {
                    temp.push_back(x.second.boxes[i]);
                }

                size = x.second.souvenirs.size();

                for (int i = 0; i < size; ++i) //get its souvenirs
                {
                    tempS.push_back(x.second.souvenirs[i]);
                }

                temp.push_back(copyChild); //add the child to its boxes
            }
        }

        this->umap.erase(parent); //delete the parent
        this->umap[parent] = Box(); //create a new parent with the updated data 
        this->umap[parent].boxes = temp;
        this->umap[parent].souvenirs = tempS;
    }
    
    bool isThereBoxWithOneBox() //check if there is a box with a box 
    {
        for (auto x : this->umap)
        {
            if (x.second.boxes.size() == 1 && x.second.souvenirs.size() == 0)
            {
                return true;
            }
        }

        return false; 
    }
    
    void removeBoxesWithOneBox()
    {
        while (isThereBoxWithOneBox()) //do this while we still have boxes with one box
        {
            std::vector<std::string> toDelete;
            std::vector<std::string> smaller; //the boxes inside of the boxes that will be deleted
            std::vector<std::string> bigger;  //the boxes that contatin the boxes that will be deleted
            
            for (std::pair<std::string, Box> x : this->umap)
            {
                if (x.second.boxes.size() == 1 && x.second.souvenirs.size() == 0) //get the box we want to delete and the box inside of it
                {
                    smaller.push_back(x.second.boxes[0]);
                    toDelete.push_back(x.first);
                }
            }

            for (std::string str : toDelete)
            {
                for (std::pair<std::string, Box> x : this->umap)
                {
                    for (std::string y : x.second.boxes)
                    {
                        if (str == y) //get the box that contains the one that will be deleted
                        {
                            bigger.push_back(x.first);
                        }
                    }
                }
            }
            
            int size = toDelete.size();

            for (int i = 0; i < size; ++i) //change the parent box of the ones in "smaller" to the ones in "bigger"
            {
                changeParentBox(bigger[i], smaller[i], toDelete[i]);
            }

            removeEmptyBoxes(); //remove left empty boxes after the parent change
        }
    }

    void optimise()
    {
        removeEmptyBoxes();
        removeBoxesWithOneBox();
    }
};


int main()
{
    std::string filename;
    cout << "Write file: " ;
    cin >> filename;

    BoxHandler handler(filename);
    handler.optimise();
    cout << "RESULT:" << endl;
    handler.print();

    return 0; 
}
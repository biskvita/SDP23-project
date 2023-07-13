#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <climits> 
#include <unordered_map>

using std::cout;
using std::cin;
using std::endl;

////////////////////////////////

struct NeighbourInfo
{
    std::string name;
    int distance;

    bool operator>(const NeighbourInfo& other) const
    {
        return distance > other.distance;
    }
};

class Graph
{
private:
    int vertex;
    int edges;
    int minutes;
    std::unordered_map<std::string, std::vector<NeighbourInfo>> umap;
public:
    ~Graph() = default;

    Graph(std::string filename)
    {
        std::ifstream file(filename, std::ios::in); //open file

        if (!file.is_open()) 
        {
            throw std::invalid_argument("no file");
        }
        
        std::string line; 
        int count = 0; 
        int ver, ed; 

        file >> ver >> ed; //get vertices and edges
        this->vertex = ver;
        this->edges = ed;
        line.clear();

        while(count < edges) //read edges
        {
            std::string first, second;
            int x, y, dist;
            file >> first >> second >> dist; //get the two vertices and the distance between them
            
            if (this->umap.find(first) == this->umap.end()) //if we do not have the first, create it
            {
                this->umap[first] = std::vector<NeighbourInfo>();
            }

            if (this->umap.find(second) == this->umap.end()) //if we do not have the second, create it
            {
                this->umap[second] = std::vector<NeighbourInfo>();
            }

            this->umap[first].push_back({second, dist}); //add their neighbours 
            this->umap[second].push_back({first, dist});
            count++;
	    }
	
        file >> line; //read the minutes
        this->minutes = stoi(line);

        file.close(); //close file
    }

    void print() //print graph
    {
        for (auto el : this->umap)
        {
            cout << "Vertex: " << el.first << "" << endl;

            for (NeighbourInfo x : el.second)
            {
                cout << x.name << " - " << x.distance << endl;
            }
            
            cout << endl;
        }
    }

    std::pair<int, std::vector<std::string>> dijkstra(const std::string& key, const std::string& destination)
    {
        std::priority_queue<NeighbourInfo, std::vector<NeighbourInfo>, std::greater<NeighbourInfo>> unvisited;
        std::unordered_map<std::string, int> weights; //array of all distances from start 
        std::unordered_map<std::string, std::string> predecessor; //keep track of visited vertices 

        weights[key] = 0; //add the start point
        unvisited.push({key, 0});

        while (!unvisited.empty())
        {
            NeighbourInfo current = unvisited.top();
            unvisited.pop();

            for (NeighbourInfo adj : this->umap[current.name])
            {
                if (weights[adj.name] == 0) //set to infinity for the first time
                {
                    weights[adj.name] = INT_MAX;
                }
                 
                if (weights[adj.name] > weights[current.name] + adj.distance) //if there is a shorter distance through current
                {
                    weights[adj.name] = weights[current.name] + adj.distance; //update the vertex's path
                    unvisited.push({adj.name, weights[adj.name]}); 
                    predecessor[adj.name] = current.name;
                }
            }
        }

        std::vector<std::string> path;
        std::string copy = destination;
        int min = 0;

        for (std::pair<std::string, int> x : weights)
        {
            if (destination == x.first)
            {
                min = x.second; //get the minutes to the wanted destination
            }
        }

        while (copy != key)
        {
            path.emplace(path.begin(), copy); //place at beginning so we do not have to reverse it 
            copy = predecessor[copy]; //get the previous place in the path 
        }
        
        path.emplace(path.begin(), key); //add the start to the path 
        std::pair<int, std::vector<std::string>> result; //set the result 
        result.first = min;
        result.second = path; 
        
        return result; 
    }

    std::vector<std::pair<int, std::vector<std::string>>> allPaths(const std::string& key) ///generate all paths from a start point to all others
    {
        std::vector<std::pair<int, std::vector<std::string>>> result;

        for (std::pair<std::string, std::vector<NeighbourInfo>> x : this->umap)
        {
            if (x.first != key) //skip the start point
            {
                std::pair<int, std::vector<std::string>> path = this->dijkstra(key, x.first); //generate path to every othe vertex
                result.push_back({path.first, path.second});
            }
        }

        return result;
    }

    bool isInVisited(std::vector<std::string> path, std::vector<std::string> visited) //check if we have already been to vertices in this path
    {
        int size = path.size();

        for (int i = 1; i < size; ++i) //skip the start point
        {
            for (std::string y : visited)
            {
                if (path[i] == y)
                {
                    return true; 
                }
            }
        }

        return false; 
    }

    std::pair<int, std::vector<std::string>> getMaxPath(std::vector<std::pair<int, std::vector<std::string>>>  paths, std::vector<std::string> visited, int minutes)
    {
        std::vector<std::pair<int, std::vector<std::string>>>  copy;
        std::pair<int, std::vector<std::string>> result;
        std::string temp; 

        //remove the paths with the time clashing 
        for (std::pair<int, std::vector<std::string>>  x : paths)
        {
            if (minutes >= x.first)
            {
                copy.push_back({x.first, x.second});
                temp = x.second[0];
            }
        }

        //if there is no available path, return the start point
        if (copy.size() == 0)
        {
            result.first = 0;
            result.second.push_back(temp);
            return result;
        }

        int max = 0;
        int min = 0; 
        paths.clear();

        //get first element of the array in pair as max
        for (std::pair<int, std::vector<std::string>> x : copy)
        {
            max = x.second.size();
            break;
        }
        
        //find the path with maximum vertices 
        for (std::pair<int, std::vector<std::string>> x : copy)
        {
            if (x.second.size() > max)
            {
                max = x.second.size();
            }
        }

        //get the paths with the maximum vertices 
        for (std::pair<int, std::vector<std::string>> x : copy)
        {
            if (x.second.size() == max)
            {
                paths.push_back({x.first, x.second});
            }
        }

        copy.clear(); 

        //get rid of paths with already visited places
        for (std::pair<int, std::vector<std::string>> x : paths)
        {  
            if (visited.size() > 0 && !isInVisited(x.second, visited))
            {
                copy.push_back({x.first, x.second});
            }
        }
        
        //if we have filtered the paths with the visited places
        if (copy.size() > 0)
        {
            paths.clear();
            paths = copy; 
        }

        //get first element of the array in pair as min
        for (std::pair<int, std::vector<std::string>> x : paths)
        {
            min = x.first;
            break;
        }

        //find the path with the minimum distance 
        for (std::pair<int, std::vector<std::string>> x : paths)
        {
            if (x.first < min)
            {
                min = x.first;
            }
        }

        //make the result
        for (std::pair<int, std::vector<std::string>> x : paths)
        {
            if (x.first == min)
            {
                result.first = x.first;
                result.second = x.second; 
            }
        }

        return result; 
    }

    bool isInArr(std::string wanted, std::vector<std::string> arr) //if a vertex is in the array already 
    {
        for (std::string x : arr)
        {
            if (wanted == x)
            {
                return true; 
            }
        }

        return false; 
    }

    std::string getFirstHalf()
    {
        int time = this->minutes/2; //the idea is that we want to gurantee a return via the same path (in case there isnt another one)
        std::vector<std::string> vis;
        std::string start = "Railstation"; 
        std::string result = start;

        while (time > 0)
        {
            std::vector<std::pair<int, std::vector<std::string>>> paths = allPaths(start); //get all paths from start
            std::pair<int, std::vector<std::string>> path = getMaxPath(paths, vis, time); //pick the best

            if (path.first == 0) //if we cant go anywhere 
            {
                break; 
            }

            for (std::string x : path.second) //add it to the result string
            {
                if (!isInArr(x, vis))
                {
                    vis.push_back(x);
                }

                if (x != start)
                {
                    result = result + " " + x; 
                }
            }
            
            time = time - path.first; //get the new time
            start = path.second[path.second.size()-1]; //get the new starting point
        }

        return result; 
    }

    std::string getTouristRoute()
    {
        std::string firstHalf = getFirstHalf();

        int pos = firstHalf.find_last_of(" ");
        std::string last = firstHalf.substr(pos+1); //the final station we have visited

        std::pair<int, std::vector<std::string>> second = dijkstra(last, "Railstation"); //get the closest path from the final point to Railstation
        std::string secondHalf;

        for (std::string x : second.second) //add the returning path to a string
        {
            if (x != last)
            {
                secondHalf = secondHalf + " " + x; 
            }
        }

        std::string route = firstHalf + secondHalf; //combine the two strings
        return route; 
    }    
};

int main()
{
    std::string filename;
    cout << "Write file: " ;
    cin >> filename;

    Graph littleGrapgh(filename);
    std::string result = littleGrapgh.getTouristRoute();
    cout << result << endl;

    return 0; 
}
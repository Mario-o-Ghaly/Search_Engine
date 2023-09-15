#include <vector>
#include <map>
#include <iostream>
using namespace std;

struct Node {
    int index;        
    int impressions;  
    double clicks;
    double Rank;
    double CTR;
    double score;
    string link;
    vector<string> keywords; 
    vector<int> nodes;       //a vector stores the nodes going to this node (to reduce time complexity in calculating PR)

    Node() :index(0), link(""), impressions(1), Rank(0), clicks(0), CTR(0), score(0) {};
    Node(string str, int ind) :link(str), index(ind), impressions(1), Rank(0), clicks(0), CTR(0), score(0) {};
};


class Graph {
private:
    int V;
    vector<Node> vertices;   //for the data of the links
    vector<vector<int>> adjlist;
    vector<int> map;         //a sorted vector according to the score maps the indeces of the nodes

public:
    Graph() :V(0) {};
    int V_() const { return V; }

    void addEdge(string src, string dst) {
       /* if (src == dst)   //if it is not possible to add a link to itself
            return;*/
        int start = -1, end = -1;
        for (int i = 0; i < vertices.size() && (start == -1 || end == -1); ++i) {   //remove the between brackets
            if (vertices[i].link == src)
                start = i;
            if (vertices[i].link == dst)
                end = i;
        }
        if (start == -1) {
            start = vertices.size();
            vertices.push_back(Node(src, start));

            map.push_back(start);
        }
        if (end == -1) {
            end = vertices.size();
            vertices.push_back(Node(dst, end));

            map.push_back(end);
        }
        adjlist.resize(vertices.size());
        adjlist[start].push_back(end);
        vertices[end].nodes.push_back(start);  //for the page Rank
        V = vertices.size();
    }

    /* void function() {
         for (int i = 0; i < V; ++i) {
             cout << vertices[i].link << endl;
         }
     }*/

    void add_keywords(string str) {
        int i = 0;
        for (; i < str.size(); ++i) {
            if (str[i] == ',')
                break;
        }
        string link = str.substr(0, i);

        int j = Find_index(link);

        int start = ++i;
        for (; i < str.size(); ++i) {
            if (str[i] == ',') {
                vertices[j].keywords.push_back(str.substr(start, i - start));
                start = i + 1;
            }
        }
        vertices[j].keywords.push_back(str.substr(start, str.size() - start));
    }

    /* void keywords() {
         for (int j = 0; j < V; ++j) {
             for (int i = 0; i < vertices[j].keywords.size(); ++i) {
                 cout << vertices[j].keywords[i] << "  ";
             }
             cout << endl;
         }
     }*/

    int Find_index(string link) {
        for (int j = 0; j < V; ++j)
            if (vertices[j].link == link)
                return j;
        return 0;                         //This is just for fixing an error
    }

    Node* vertix(string link) {
        return(&vertices[Find_index(link)]);    //to return a specific node using the link
    }

    Node* vertix(int ind) {
        return(&vertices[ind]);    //to return a specific node using the index
    }

    vector<int> mapping()const {     //to loop over the nodes in an ordered way according to the score
        return map;
    }

    /*void impressions() {
         for (int i = 0; i < V; ++i) {
             cout << vertices[i].impressions << endl;
         }
     }*/

    void calculate_PR() {
        double DF = 0.85;
        double* arr = new double[V];
        for (int i = 0; i < V; ++i) {
            arr[i] = 1.0 / V;
            vertices[i].Rank = 1.0 / V;
        }
        double add;
        for (int j = 0; j < 7; ++j) {
            for (int i = 0; i < V; ++i) {
                double sum = 0;
                for (int r = 0; r < vertices[i].nodes.size(); ++r) 
                    sum += (vertices[vertices[i].nodes[r]].Rank / adjlist[vertices[i].nodes[r]].size());
                arr[i] = sum;
            }
            add = 0;
            for (int i = 0; i < V; ++i) {
                vertices[i].Rank = arr[i] * 0.85 + (0.15 / V);
                add += vertices[i].Rank;
            }
            
        }

        for (int i = 0; i < V; ++i)
            vertices[i].Rank *= (1.0 / add);

        calculate_score();
    }

    void calculate_score() {
        double percent;
        for (int i = 0; i < V; ++i) {
            percent = (0.1 * vertices[i].impressions) / (1 + 0.1 * vertices[i].impressions);
            vertices[i].score = 0.4 * vertices[i].Rank + 0.6 * (vertices[i].Rank * (1 - percent) + vertices[i].CTR * percent);  
        }

        mergeSort(0, V - 1);
    }

    void update_CTR(int v) {
        vertices[v].CTR = vertices[v].clicks / vertices[v].impressions;
    }

    void update_score(int v) {
        double percent = (0.1 * vertices[v].impressions) / (1 + 0.1 * vertices[v].impressions);
        vertices[v].score = 0.4 * vertices[v].Rank + 0.6 * (vertices[v].Rank * (1 - percent) + vertices[v].CTR * percent);
    }

    void clicked(int v) {
        vertices[v].CTR = (++vertices[v].clicks) / vertices[v].impressions;
        update_score(v);
    }

    void displayed(int v) {
        vertices[v].CTR = (vertices[v].clicks) / ++vertices[v].impressions;
        update_score(v);
    }

    void merge( int p, int q, int r) {
        int n1 = q - p + 1;
        int n2 = r - q;

        int* L = new int[n1], * M = new int[n2];

        for (int i = 0; i < n1; i++)
            L[i] = map[p + i];
        for (int j = 0; j < n2; j++)
            M[j] = map[q + 1 + j];

        int i, j, k;
        i = 0;
        j = 0;
        k = p;

        while (i < n1 && j < n2) {
            if (vertices[L[i]].score >= vertices[M[j]].score) {
                map[k] = L[i];
                i++;
            }
            else {
                map[k] = M[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            map[k] = L[i];
            i++;
            k++;
        }

        while (j < n2) {
            map[k] = M[j];
            j++;
            k++;
        }
    }

    void mergeSort(int l, int r) {
        if (l < r) {
            int m = l + (r - l) / 2;

            mergeSort(l, m);
            mergeSort(m + 1, r);
            merge(l, m, r);
        }
    }
};
//increase impressions by 1 when shown in the search results
//increase number of click-throughs when chosen
//The page Rank will not change

//You should update the files before ending the program (impressions)
//You should create new file for the CTR
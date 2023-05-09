#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

class Node {
public:
    // represent node index
    int index;

    // represent power demand
    int node_value;

    // represent node priority
    int priority_value;

    // represent all adjacent nodes and its respective weight
    vector<pair<double, Node*>> adj;

    Node (int index, int node_value, int priority_value) {
        this->index = index;
        this->node_value = node_value;
        this->priority_value = priority_value;
    }

    // function to find adjacent node by index
    pair<double, Node*>* findAdjByIndex(int index) {
        auto it = find_if(this->adj.begin(), this->adj.end(), [index](const pair<double, Node*> obj) {return obj.second->index == index;});
        int idx = it - this->adj.begin();
        pair<double, Node*>* node = NULL;
        if (it != this->adj.end()) {
            node = &this->adj[idx];
        }
        return node;
    }

    // function to remove adjacent node by index
    void removeAdjByIndex(int index) {
        auto it = find_if(this->adj.begin(), this->adj.end(), [index](const pair<double, Node*> obj) {return obj.second->index == index;});
        if (it != this->adj.end()) {
            this->adj.erase(it);
        }
    }

    // Define which node can be the greater node
    bool operator>(const Node& other) const {
        // if nodes have same priority, compare their value
        if (this->priority_value == other.priority_value) {
            return this->node_value > other.node_value;
        }
        // default, compare the priority value
        return this->priority_value > other.priority_value;
    }

    // override << operator to display node with cout
    friend ostream& operator<<(ostream& os, const Node& dt)
    {
        os << "Node " << dt.index << " : value " << dt.node_value << " - priority " << dt.priority_value;
        return os;
    }
};


// used mainly to represent source-dest node pair
typedef pair<Node*, Node*> sd;

// edge is a pair of weight and source-dest node pair
typedef pair<double, sd> edge;

class Graph {
public:
    // represent all nodes
    vector<Node*> nodes;

    // find node by index in vector
    Node* findByIndex(int index) {
        auto it = find_if(this->nodes.begin(), this->nodes.end(), [index](const Node* obj) {return obj->index == index;});
        Node* node = nullptr;
        if (it != this->nodes.end()) {
            node = *it;
        }
        return node;
    }

    // calculate weight of the edge
    static double calculateEdgeWeight(int impedance, int dest_power, int dest_priority) {
        return (double) impedance / (dest_power * dest_priority);
    }

    // Generate mst from the graph with given start index
    Graph* generateMSTWoEval(int start_index, int dg_capacity) {
        Graph* mst = new Graph();

        // define visited array
        vector<bool> visited(this->nodes.size() + 1);
        fill(visited.begin(), visited.end(), false);

        // initiate total power to 0
        int total_power = 0;

        // get node by start index
        Node* start_node = this->findByIndex(start_index);

        // create priority queue for edge
        priority_queue<edge, vector<edge>, greater<edge> > pq;
        pq.push(make_pair(0, make_pair(start_node, nullptr)));

        // repeat until priority queue empty
        while (!pq.empty()) {
            // get weight
            double weight = pq.top().first;

            // get current node
            Node* node = pq.top().second.first;

            // get parent node (if any)
            Node* parent = pq.top().second.second;

            // pop pq
            pq.pop();

            // if node already visited, then skip
            if (visited[node->index]) continue;

            // check of total power exceed dg capacity after insert new node
            total_power = total_power + node->node_value;
            if (total_power > dg_capacity) break;

            // add node to mst
            Node* new_node = new Node(node->index, node->node_value, node->priority_value);
            mst->nodes.emplace_back(new_node);
            if (parent != nullptr) {
                Node* new_parent = mst->findByIndex(parent->index);
                new_parent->adj.emplace_back(make_pair(weight, new_node));
                new_node->adj.emplace_back(make_pair(node->findAdjByIndex(parent->index)->first, new_parent));
            }

            // mark the node as visited
            visited[node->index] = true;


            // add all adjacent node to pq
            for (auto &[weight, adj_node]: node->adj) {
                if (visited[adj_node->index]) continue;

                pq.push(make_pair(weight, make_pair(adj_node, node)));
            }

        }

        // return the mst
        return mst;
    }

    // Generate mst from the graph with given start index
    Graph* generateMST(int start_index) {
        Graph* mst = new Graph();

        // define visited array
        vector<bool> visited(this->nodes.size() + 1);
        fill(visited.begin(), visited.end(), false);

        // get node by start index
        Node* start_node = this->findByIndex(start_index);

        // create priority queue for edge
        priority_queue<edge, vector<edge>, greater<edge> > pq;
        pq.push(make_pair(0, make_pair(start_node, nullptr)));

        // repeat until priority queue empty
        while (!pq.empty()) {
            // get weight
            double weight = pq.top().first;

            // get current node
            Node* node = pq.top().second.first;

            // get parent node (if any)
            Node* parent = pq.top().second.second;

            // pop pq
            pq.pop();

            // if node already visited, then skip
            if (visited[node->index]) continue;

            // add node to mst
            Node* new_node = new Node(node->index, node->node_value, node->priority_value);
            mst->nodes.emplace_back(new_node);
            if (parent != nullptr) {
                Node* new_parent = mst->findByIndex(parent->index);
                new_parent->adj.emplace_back(make_pair(weight, new_node));
                new_node->adj.emplace_back(make_pair(node->findAdjByIndex(parent->index)->first, new_parent));
            }

            // mark the node as visited
            visited[node->index] = true;

            // add all adjacent node to pq
            for (auto &[weight, adj_node]: node->adj) {
                if (visited[adj_node->index]) continue;

                pq.push(make_pair(weight, make_pair(adj_node, node)));
            }

        }

        // return the mst
        return mst;
    }


    // function to calculate total power
    int getTotalPower() {
        int total = 0;
        for (Node* node: this->nodes) {
            total += node->node_value;
        }
        return total;
    }

    // function to remove node by index
    void removeNodeByIndex(int index) {
        auto it = find_if(this->nodes.begin(), this->nodes.end(), [index](const Node* obj) {return obj->index == index;});
        Node* node = *it;
        for (auto &[weight, adj_node]: node->adj) {
            adj_node->removeAdjByIndex(index);
        }

        if (it != this->nodes.end()) {
            this->nodes.erase(it);
        }
    }

    // override << operator to display graph with cout
    friend ostream& operator<<(ostream& os, const Graph& dt) {
        for (int i = 0; i < dt.nodes.size(); ++i) {
            os << *dt.nodes[i] << endl;
            os << "Adjacent : " << endl;
            for (int j = 0; j < dt.nodes[i]->adj.size(); ++j) {
                os << "\t" << *dt.nodes[i]->adj[j].second << " - weight " << dt.nodes[i]->adj[j].first << endl;
            }
            os << endl;
        }
        return os;
    };
};

// Used to compare node to remove
class Compare {
public:
    bool operator()(Node below, Node above) {
        // add node with the least adjacent first
        if (above.adj.size() < below.adj.size()) {
            return true;
        }

        // add node with the least priority first
        if (above.adj.size() == below.adj.size()
                    && above.priority_value < below.priority_value) {
            return true;
        }

        // add node with the least value first
        if (above.adj.size() == below.adj.size()
                   && above.priority_value == below.priority_value
                   && above.node_value <= below.node_value) {
            return true;
        }

        return false;
    }
};

// function to optimize
void evaluateMST(Graph* mst, int start_index, int dg_capacity) {
    // create priority queue to eliminate node if needed
    priority_queue<Node, vector<Node>, Compare> pq;

    // calculate current power
    int current_power = mst->getTotalPower();

    // add all nodes to pq, except starting node
    for (Node* node: mst->nodes) {
        if (node->index != start_index) pq.push(*node);
    }

    // repeat until current power is not greater than dg capacity
    while (!pq.empty() && current_power > dg_capacity) {
        // Get top node
        Node node = pq.top();
        pq.pop();

        // if current node is not leaf node, then get out of the loop
        if (node.adj.size() > 1) break;

        // remove node from mst
        mst->removeNodeByIndex(node.index);

        // recalculate the power
        current_power = mst->getTotalPower();
    }
}

int main() {
    Graph *graph = new Graph();
    int priority[3] = {100, 10, 1};

//  CREATE ALL NODES
    Node* node1 = new Node(1, 10, priority[2]);
    Node* node2 = new Node(2, 18, priority[0]);
    Node* node3 = new Node(3, 9, priority[2]);
    Node* node4 = new Node(4, 12, priority[1]);
    Node* node5 = new Node(5, 1, priority[2]);

//  ADD ALL NODES ADJACENT
    node1->adj.emplace_back(Graph::calculateEdgeWeight(3, node2->node_value, node2->priority_value), node2);
    node1->adj.emplace_back(Graph::calculateEdgeWeight(5, node3->node_value, node3->priority_value), node3);

    node2->adj.emplace_back(Graph::calculateEdgeWeight(3, node1->node_value, node1->priority_value), node1);
    node2->adj.emplace_back(Graph::calculateEdgeWeight(4, node3->node_value, node3->priority_value), node3);
    node2->adj.emplace_back(Graph::calculateEdgeWeight(3, node5->node_value, node5->priority_value), node5);

    node3->adj.emplace_back(Graph::calculateEdgeWeight(5, node1->node_value, node1->priority_value), node1);
    node3->adj.emplace_back(Graph::calculateEdgeWeight(4, node2->node_value, node2->priority_value), node2);
    node3->adj.emplace_back(Graph::calculateEdgeWeight(3, node5->node_value, node5->priority_value), node5);

    node4->adj.emplace_back(Graph::calculateEdgeWeight(3, node5->node_value, node5->priority_value), node5);

    node5->adj.emplace_back(Graph::calculateEdgeWeight(3, node2->node_value, node2->priority_value), node2);
    node5->adj.emplace_back(Graph::calculateEdgeWeight(3, node3->node_value, node3->priority_value), node3);
    node5->adj.emplace_back(Graph::calculateEdgeWeight(3, node4->node_value, node4->priority_value), node4);

//  ADD ALL NODES TO GRAPH
    graph->nodes.push_back(node1);
    graph->nodes.push_back(node2);
    graph->nodes.push_back(node3);
    graph->nodes.push_back(node4);
    graph->nodes.push_back(node5);

//  PRINT GRAPH
    cout << "ORIGINAL GRAPH" << endl;
    cout << *graph << endl;

//  MST WITHOUT EVALUATION
    Graph* mst2 = graph->generateMSTWoEval(3, 40);
    cout << "MINIMUM SPANNING TREE WITHOUT EVALUATION" << endl;
    cout << *mst2;
    cout << "total power : " << mst2->getTotalPower() << endl;

//  PRINT MST
    Graph* mst = graph->generateMST(3);
    cout << "MINIMUM SPANNING TREE" << endl;
    cout << *mst << endl;

//  EVALUATE MST BY CONSTRAIN
    int dg_capacity = 40;
    int start_node = 3;
    evaluateMST(mst, start_node, dg_capacity);

//  PRINT NEW MST
    cout << "EVALUATED MST" << endl;
    cout << *mst;

    cout << "dg capacity : " << dg_capacity << endl;
    cout << "total power : " << mst->getTotalPower() << endl;
}

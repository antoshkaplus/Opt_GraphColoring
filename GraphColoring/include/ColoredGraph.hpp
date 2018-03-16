#pragma once

#include "util.hpp"


// if graph is dence it's much better to keep adjacent colors in array and not map
// also it's much better to make colors as a sequence 

// mutable class
// here we expect that colors could be any number and not necessarily ordered

// instead of inheritance use shared_ptr to reuse graph
// can also use template to pick reuse or not
class ColoredGraph : public Graph {
private:

    // node => color
    vector<Color>               _coloring;
    // support data structure
    // node, color => number of adjacent nodes with that color 
    vector<ant::CountMap<Color>> _adjacentNodesOfColorCount;
    // how many nodes with the particular color
    ant::CountMap<Color>         _nodeCountOfColor;
    // nodes that are uncolored
    set<Node>                   _uncoloredNodes;

public:
    ColoredGraph() {}
    ColoredGraph(const Graph& gr) : Graph(gr) {
        _coloring.resize(nodeCount(), COLOR_NONE);
        _adjacentNodesOfColorCount.resize(nodeCount());
        // _nodeCountOfColor nothing i can initialize
        for (int i = 0; i < nodeCount(); i++) {
            _uncoloredNodes.insert(i);
        }
    }

    ColoredGraph(const Graph& gr, const vector<Color>& coloring)
        : Graph(gr),
          _coloring(coloring) {
        
        _adjacentNodesOfColorCount.resize(nodeCount());
        for (Node i = 0; i < nodeCount(); i++) {
            if (color(i) == COLOR_NONE) {
                _uncoloredNodes.insert(i);
                continue;
            }
            
            for (Node j : nextNodes(i)) {
                _adjacentNodesOfColorCount[j].increase(color(i));
            }
            _nodeCountOfColor.increase(color(i));
        }
    }
    
    ColoredGraph& operator=(const ColoredGraph& c_gr) {
        _coloring                   = c_gr._coloring;
        _adjacentNodesOfColorCount  = c_gr._adjacentNodesOfColorCount;
        _nodeCountOfColor           = c_gr._nodeCountOfColor;
        _uncoloredNodes             = c_gr._uncoloredNodes;
        return *this;
    }
    
    size_t adjacentNodesOfColorCount(Node i, Color c) const {
        auto it = _adjacentNodesOfColorCount[i].find(c);
        return it == _adjacentNodesOfColorCount[i].end() ? 0 : it->second;
    } 

    set<Color> adjacentColors(Node i) const {
        return _adjacentNodesOfColorCount[i].keys();
    }
    
    const map<Color, Count>& adjacentNodesOfColorCount(Node i) {
        return _adjacentNodesOfColorCount[i];
    }
    
    Count adjacentNodesWithSameColorCount(Node i) {
        return adjacentNodesOfColorCount(i, color(i));
    }
    
    
    set<Color> colors() const {
        return _nodeCountOfColor.keys();
    }
    
    size_t nodeCountOfColor(Color c) const {
        return _nodeCountOfColor.get(c);
    }
    
    const ant::CountMap<Color>& nodeCountOfColor() const {
        return _nodeCountOfColor;
    }
    
    size_t colorCount() const {
        return _nodeCountOfColor.size();
    }
    
    size_t adjacentColorCount(Node i) const {
        return _adjacentNodesOfColorCount[i].size();
    }  
    
    size_t adjacentUncoloredNodeCount(Node i) const {
        size_t count = 0;
        for (Node j : nextNodes(i)) {
            if (_uncoloredNodes.count(j)) count++;
        }
        return count;
    }
    
    Color color(Node i) const {
        return _coloring[i];
    }
    
    const vector<Color>& coloring() const {
        return _coloring;
    } 
    
    size_t uncoloredNodeCount() const {
        return _uncoloredNodes.size();
    }
    
    const set<int>& uncoloredNodes() const {
        return _uncoloredNodes;
    }
  
    bool isColored(Node i) const {
        return _coloring[i] != COLOR_NONE;
    }
  
    // i: index of node, which should be without color
    void unsetColor(int i) {
        int c = color(i);
        assert(c != -1); // unsetting uncolored node!
        for (int j : nextNodes(i)) {
            _adjacentNodesOfColorCount[j].decrease(c);
        }
        _nodeCountOfColor.decrease(c);
        _coloring[i] = -1;
        _uncoloredNodes.insert(i);
    }
  
    void setColor_2(int i, int c) {
        if (isColored(i)) unsetColor(i);
        setColor(i, c);
    }
  
    // call this method if you want to 
    // set a color of an !!!uncolored node!!!
    void setColor(int i, int c) {
        assert(color(i) == -1); // setting color of colored node!;
        for (int j : nextNodes(i)) {
            _adjacentNodesOfColorCount[j].increase(c);
        }
        _nodeCountOfColor.increase(c);
        _coloring[i] = c;
        _uncoloredNodes.erase(i);
    }
    
    void setColor(Node i) {
        assert(adjacentColorCount(i) < colorCount());
        for (Color c : colors()) {
            if (adjacentNodesOfColorCount(i, c) == 0) {
                setColor(i, c);
                break;
            }
        }
    }

    // sets another first possible color for a node
    void resetColor(Node i) {
        int cMain = color(i);
        if (cMain == -1) cout << "resetting color of uncolored node!\n";
        unsetColor(i);
        for (auto& p : _nodeCountOfColor) {
            Color c = p.first;
            if (_adjacentNodesOfColorCount[i].count(c) && c != cMain) {
                setColor(i, c);
                break;
            }
        }
    }
  
    // if some color is already set
    void resetColor(int i, int c) {
        unsetColor(i);
        setColor(i, c);
    }
    
    // don't use with uncolored nodes
    // colored graph should be feasible for color of node i0 and color 
    void kempeChainSwap(Node i0, Color c1) {
        assert(color(i0) != COLOR_NONE && c1 != -1);
        unordered_set<Node> recoloredNodes;
        stack<Node> nodesToHandle;
        Color c0 = color(i0);
        nodesToHandle.push(i0);
        while (!nodesToHandle.empty()) {
            Node i = nodesToHandle.top();
            nodesToHandle.pop();
            // future color for this node
            Color c = color(i) == c0 ? c1 : c0;
            resetColor(i, c);
            recoloredNodes.insert(i);
            for (Node j : nextNodes(i)) {
                if (color(j) == c && recoloredNodes.count(j) == 0) {
                    nodesToHandle.push(j);
                } 
            }
        }
    }
};


bool isFeasibleColoring(const ColoredGraph& c_gr) {
    if (c_gr.uncoloredNodeCount() > 0) return false;
    for (Node i = 0; i < c_gr.nodeCount(); i++) {
        if (c_gr.adjacentNodesOfColorCount(i, c_gr.color(i)) > 0) return false;
    }
    return true;
}

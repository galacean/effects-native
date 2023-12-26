//
//  link_chain.hpp
//
//  Created by Zongming Liu on 2022/1/20.
//

#ifndef link_chain_hpp
#define link_chain_hpp

#include <stdio.h>
#include <vector>

namespace mn {


class LinkNode {
    
public:
    
    LinkNode();
    
    LinkNode(std::vector<float> content);
    
    ~LinkNode();
    
    std::vector<float>& GetContext() {
        return content_;
    }

    LinkNode* pre_;
    
    LinkNode* next_;
    
private:
    
    std::vector<float> content_;
    
};

class LinkChain {
    
public:
  
    LinkChain();
    
    ~LinkChain();
    
    void PushNode(std::vector<float> content);
    
    void RemoveNode(LinkNode* node);
    
    size_t Length() {
        return length_;
    }

    LinkNode* first_;
    
    LinkNode* last_;
    
private:
    
    void InsertNode(LinkNode* current_node, LinkNode* node);
    
    bool SortNode(LinkNode* node, LinkNode* current_node);
    
    size_t length_;

};

}

#endif /* link_chain_hpp */

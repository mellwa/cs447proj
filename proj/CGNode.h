//
//  CGNode.h
//  proj
//
//  Created by Chao Chen on 2014-03-28.
//  Copyright (c) 2014 Chao Chen. All rights reserved.
//

#ifndef proj_CGNode_h
#define proj_CGNode_h

#include <vector>
#include <string>
#include <map>

class CGNode{
    std::string name;
    CGNode* parent;
    std::vector<CGNode*> children;
    std::vector<CGNode*> friends;
    std::map<CGNode*, unsigned long> friends_pair;
    std::map<CGNode*, unsigned long> children_pair;
    int t_support;
public:
    CGNode(std::string name);
    void setChild(CGNode *child);
    void setFriend(CGNode *node);
    void setParent(CGNode *parent);
    int T_Support();
    void T_Support_Increment();
    std::string getname();
    std::vector<CGNode*> getChildren();
    std::vector<CGNode*> getFriends();
    int friends_index(CGNode*);
};



#endif

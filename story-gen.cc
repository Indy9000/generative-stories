#include <iostream>
#include <string>
#include <map>
#include <vector>

/*
Rule:
    name
    [phrase]

name: string
phrase: string* Rule string*
string: [a-zA-Z\-]
[]: array
*/

struct Phrase;

struct Rule{
    std::string name_{""};
    std::vector<Phrase> phrases_{};
};

struct Node{
    std::string words_{""};
    Rule * p_rule_{nullptr};
};

struct Phrase{
    std::vector<Node> nodes_;
    
    std::string parse_rule_name(const std::string& t, int & j){
        int i=j;
        bool found{false};
        while(j < static_cast<int>(t.size())){
            if('#' == t[j]) { found = true; break; }
            ++j;            
        }
        if(!found){
            throw std::runtime_error(std::string("illformed rulename, no delimiting #"));
        }
        return t.substr(i, (j-1)-i-1);
    }

    void parse(const std::string& text, std::map<std::string, Rule*>& rules){
        int i=0,j=0;
        while(j < static_cast<int>(text.size())){
            if(text[j]=='#'){ //found a rule, save text so far to a node, save rule on a new node
                Node n;
                n.words_ = text.substr(i, (j-1)-i-1);
                nodes_.push_back(n);

                Node n1;
                auto rule_name = parse_rule_name(text,j);
                if(rules.find(rule_name)==rules.end()){ //create this rule if not found yet
                    n1.p_rule_ = new Rule();
                    n1.p_rule_->name_ = rule_name;
                    rules[rule_name] = n1.p_rule_;
                }else{
                    n1.p_rule_ = rules[rule_name];
                }
                nodes_.push_back(n1);
                i = j;
            }
        }
        Node n;
        n.words_ = text.substr(i, (j-1)-i-1);
        nodes_.push_back(n);
    }
};

int main(){
/*
Parse:
Load Json
for each key
    for each element
    parse Phrase

Production:
find starting rule,
randomly select a phrase
unfold phrase recursively
*/    
}
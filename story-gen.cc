#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <random>

#include "json11.hpp"

// Seed with a real random value, if available
std::random_device rrr;

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
        //parse the rule name from 1st char after # to the next #
        int i=j;
        while('#'!= t[j] && j < t.size())
            ++j;
        bool found = (t[j]=='#');
        if(!found){
            throw std::runtime_error(std::string("illformed rulename, no delimiting #"));
        }

        ++j; //skip '#'
        return t.substr(i, j-i-1);
    }

    void parse(const std::string& text, std::map<std::string, Rule*>& rules){
        int i=0,j=0;
        while(j < text.size()){
          while('#'!= text[j] && j< text.size()){
            ++j;
          }
          
          {//insert the text node
              Node text_node;
              text_node.words_ = text.substr(i,j-i);
              std::cout << "normal-text=["<< text_node.words_ <<"]\n";
              nodes_.push_back(text_node);
          }

          bool found = (text[j]=='#');
          if(found){//insert the rule node
            auto rule_name = parse_rule_name(text,++j);
            std::cout << "rule_name=[" <<rule_name <<"]\n"; 
            Node rule_node;
            if(rules.find(rule_name)==rules.end()){ //not found in the rules dictionary
                rule_node.p_rule_ = new Rule();
                rule_node.p_rule_->name_ = rule_name;
                rules[rule_name] = rule_node.p_rule_;
            }else{//found in the rules dictionary
                rule_node.p_rule_ = rules[rule_name];
            }
            nodes_.push_back(rule_node);
          }
          
          i = j;
        }
    }
};

void unfold_random_phrase(Rule* p_rule){
    if (p_rule->phrases_.size()==0){
        std::cout << "no phrases. returning;";
        return;
    }
    //-- select random phrase
    std::default_random_engine e1(rrr());
    std::uniform_int_distribution<int> uniform_dist(0, p_rule->phrases_.size()-1);
    int phi = uniform_dist(e1);
    // std::cout << "\nUnfolding phrase = "<<phi<< "\n";
    //-- unfold that phrase
    for(const auto& n : p_rule->phrases_[phi].nodes_){
        if(n.p_rule_==nullptr){
            std::cout << n.words_;
        }else{
            // std::cout << "\nUnfolding rule = "<<n.p_rule_->name_ << "\n";
            unfold_random_phrase(n.p_rule_);
        }
    }
}

int main(int argc, char* argv[]){
/*
Parse:
Load Json
for each key
    for each element
    parse Phrase
*/
// if(argc<2)
//     std::cout << "cmd line params: "<< argv[0] << " rule.json" << std::endl;

// auto rules_file = std::string(argv[1]);
auto rules_file = std::string("./tracery-like-template.json");
std::stringstream ss;
{
    std::ifstream t(rules_file);
    ss << t.rdbuf();
    t.close();
}
std::cout << ss.str() << std::endl;

//parsed content would be stored here
std::map<std::string,Rule*> rule_set;

std::string err;
auto rules_set_json = json11::Json::parse(ss.str(), err);
if(!err.empty()){
    std::cout << "error parsing json" << std::endl;
}else{
    for(auto const& k : rules_set_json.object_items()){
        std::cout << "rule-name-json="<<k.first << "\n";
        Rule * r=nullptr;
        if(rule_set.find(k.first)==rule_set.end()){
            r = new Rule();
            r->name_ = k.first;
            rule_set[k.first] = r;
        }else{
            r = rule_set[k.first];            
        }
        //-- set the phrasess
        for(auto const& v : k.second.array_items()){
            Phrase ph;
            std::cout << v.string_value() << std::endl;
            ph.parse(v.string_value(),rule_set);
            r->phrases_.push_back(ph);
        }
        std::cout << "****** setting new rule:["<<k.first <<"]\n";
        std::cout << "****** phrase_ count="<< r->phrases_.size()<<"\n";
    }
    std::cout << "\n";
}
/*
Production:
find starting rule,
randomly select a phrase
unfold phrase recursively
*/    
std::cout << "-----------------------------------------------------------\n";
std::cout << "rule-set key count="<< rule_set.size()<<"\n";

if(rule_set.find("origin")!=rule_set.end()){
    std::cout << "found starting rule\n";
    const auto& rule = rule_set["origin"];
    std::cout << "ptr=" << rule << "\n";

    unfold_random_phrase(rule);
}


}
/*
 * Author: vpetrigo
 * Task:
 * Задача простая - найти производную многочлена. Многочлен может быть большой, но только с положительными целыми степенями и целыми коэффициентами. 
 * Подумайте, каким образом его лучше представить в памяти? Вывод должен быть без пробелов и в порядке убывания степеней, хотя ввод этому правилу может не удовлетворять. 
 * Также на ввод может быть подан многочлен с неприведенными слагаемыми.
 * Для этой задачи существует 10 тестов, созданных вручную (3 из них вы можете увидеть ниже) и 90 тестов, созданных автоматически случайным образом.
 *
 * Sample Input 1:
 * x^2+x
 * Sample Output 1:
 * 2*x+1
 * Sample Input 2:
 * 2*x^100+100*x^2
 * Sample Output 2:
 * 200*x^99+200*x
 * Sample Input 3:
 * x^10000+x+1
 * Sample Output 3:
 * 10000*x^9999+1
 * Sample Input 4:
 * -x^2-x^3
 * Sample Output 4:
 * -3*x^2-2*x
 * Sample Input 5:
 * x+x+x+x+x+x+x+x+x+x
 * Sample Output 5:
 * 10
 */

#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <cctype>
#include <functional>
#include <sstream>

using coef_type = int;

struct Token {    
    enum class Kind {
        UNDEF,
        OPERATOR,
        COEF,
        VARIABLE,
        EXP
    };
    
    Token() : kind {Kind::UNDEF} {}
    Token(char ch, Kind k) : op{ch}, kind{k} {}
    Token(coef_type v, Kind k) : val{v}, kind{k} {}
    
    Kind kind;
    char op;
    coef_type val;
};

// Construct the answer after parsing an input string
std::string answer(const std::map<std::string, coef_type, std::greater<std::string>>& tokens) {
    std::string ans;
    
    for (auto cit = tokens.cbegin(); cit != tokens.cend(); ++cit) {
        if (cit != tokens.cbegin() && cit->second > 0) {
            ans += "+";
        }
        
        if (cit->first != "1") {
            if (cit ->second > 1 || cit ->second < 1) {
                ans += std::to_string(cit->second);
                ans += "*";
            }
            ans += cit->first;
        }
        else {
            ans += std::to_string(cit->second);
        }
    }
    
    return ans;
}

template <typename CIt>
void skip_whitespace(CIt& it) {
    while (isspace(*it)) {
        ++it;
    }
}

template <typename CIt>
coef_type get_int(CIt& it) {
    coef_type number = 0;
    coef_type sign;
    
    sign = (*it == '-') ? -1 : 1;
    
    if (*it == '-') {
        ++it;
    }
    
    while (isdigit(*it)) {
        number *= 10;
        number += *it - '0';
        ++it;
    }
    
    return number;
}

void push_operator(std::vector<Token>& v, char ch) {
    v.push_back(Token{ch, Token::Kind::OPERATOR});
}

void push_number(std::vector<Token>& v, coef_type num, Token::Kind type) {  
    v.push_back(Token{num, type});
}

void push_variable(std::vector<Token>& v, char ch) {
    v.push_back(Token{ch, Token::Kind::VARIABLE});
}

void add_to_ans(std::map<std::string, coef_type, std::greater<std::string>>& ans, const std::pair<std::string, coef_type>& func) {
    ans[func.first] += func.second;
}

std::pair<std::string, coef_type> pop_function(std::vector<Token>& st) {
    std::pair<std::string, coef_type> deriv_fun;
    Token t = st.back();
    auto stack_lenght = st.size();
    coef_type coef = 1;

    if (t.kind == Token::Kind::EXP) {
        std::string fun;

        coef = t.val;
        // remove the exponential coefficient
        st.pop_back();
        // remove the variable character
        st.pop_back();
        
        if (st.size() > 0 && st.back().kind != Token::Kind::OPERATOR) {
            coef *= st.back().val;
        }
        
        if (st.front().kind == Token::Kind::OPERATOR && st.front().op == '-') {
            coef = -coef;
        }
        
        if (t.val > 2) {
            fun = "x^" + std::to_string(t.val - 1);
        }
        else if (t.val == 2) {
            fun = "x";
        }
        else {
            fun = "1";
        }
        
        deriv_fun = std::make_pair(fun, coef);
    }
    else if (t.kind == Token::Kind::VARIABLE) {    
        // if we have '-' or '+' at the beginning of a function statement
        if (st.front().kind == Token::Kind::OPERATOR) {
            if (st.front().op == '-') {
                coef = -coef;
            }
            // remove the variable token
            st.pop_back();
            // if there is a coefficient before variable;
            if (st.back().kind == Token::Kind::COEF) {
                coef *= st.back().val;
            }
        }
        else if (st.front().kind == Token::Kind::COEF) {
            coef = st.front().val;
        }
        
        deriv_fun = std::make_pair("1", coef);
    }
    else if (t.kind == Token::Kind::COEF) {
        deriv_fun = std::make_pair("0", 0);
    }
    else {
        throw std::runtime_error{"Bad input"};
    }
    
    st.clear();
    
    return deriv_fun;
}

std::string derivative(const std::string& polynomial) {
    std::map<std::string, coef_type, std::greater<std::string>> ans;
    std::vector<Token> stack;
    bool is_negative = false;
    
    for (auto cit = polynomial.cbegin(); cit != polynomial.cend(); ++cit) {
        if (isspace(*cit)) {
            skip_whitespace(cit);
            
            if (cit == polynomial.cend()) {
                break;
            }
        }
        
        switch (*cit) {
            case '+':
            case '-':
            {
                if (stack.size() > 0) {
                    auto func = pop_function(stack);
                    
                    if (func.first != "0") {
                        add_to_ans(ans, func);
                    }
                }
                
                push_operator(stack, *cit);
                break;
            }
            case '1': case '2': case '3': 
            case '4': case '5': case '6': 
            case '7': case '8': case '9':
            {
                push_number(stack, get_int(cit), Token::Kind::COEF);
                // get back because we get over the last digit character
                --cit;
                break;
            }
            case '*':
                break;
            case 'x':
                push_variable(stack, *cit);
                break;
            case '^':
                // TODO: Now I do not want to deal with the separate power token, now it is skipped
                // But might be changed somehow
                ++cit;
                skip_whitespace(cit);
                push_number(stack, get_int(cit), Token::Kind::EXP);
                --cit;
                break;
            default:
                std::cerr << "Unknown character" << std::endl;
                break;
        }
    }
    
    if (stack.size()) {
        auto func = pop_function(stack);
        if (func.first != "0") {
            add_to_ans(ans, func);
        }
    }
    
    return answer(ans);
}

int main() {
    std::string inp_function;
    
    while(getline(std::cin, inp_function)) {
        std::cout << derivative(inp_function) << std::endl;
    }
    
    return 0;
}
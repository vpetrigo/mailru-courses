/*
 *  Author: vpetrigo
 *  Task:
 *  Реализовать абстрактный базовый класс «Фигура», содержащий методы вычисления площади и метод show() для отображения информации об объекте. 
 *  Построить производные классы Square (квадрат), Circle (окружность), Rectangle (прямоугольник), содержащие методы для вычисления периметров фигур. 
 *  Создать массив указателей на объекты базового класса и заполнить этот массив объектами производных классов. 
 *  Продемонстрировать работу виртуальных методов базового класса при проходе по этому массиву.
 *
 *  Sample Input:
 *  s(3), c(2), r(2,4)
 *  Sample Output:
 *  9, 12.56, 8
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <cctype>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class Shape {
public:
    virtual double square() const = 0;
    virtual void show() const = 0;
    virtual ~Shape() {}
};

class Square : public Shape {
public:
    explicit Square(double side) : _side{side} {}
    
    double square() const override {
        return _side * _side;
    }
    
    void show() const override {
        std::cout << "Square" << std::endl;
    }
private:
    double _side;
};

class Rectangle : public Shape {
public:
    explicit Rectangle(double height, double width) : _height{height}, _width{width} {}
    
    double square() const override {
        return _height * _width;
    }
    
    void show() const override {
        std::cout << "Rectangle" << std::endl;
    }
private:
    double _height;
    double _width;
};

class Circle : public Shape {
public:
    static constexpr float pi = 3.14;
    
    explicit Circle(double radius) : _radius{radius} {}
    
    double square() const override {
        return pi * _radius * _radius;
    }
    
    void show() const override {
        std::cout << "Circle" << std::endl;
    }
private:
    double _radius;
};

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    char ch;
    
    while (std::cin.get(ch)) {
        switch (ch) {
            char l_parenthesis, r_parenthesis;
            char comma;
            case 'r':
                double w, h;
                
                std::cin >> l_parenthesis >> w >> comma >> h >> r_parenthesis >> comma;
                shapes.push_back(make_unique<Rectangle>(w, h));
                break;
            case 's':
            case 'c':
                double num;
                
                std::cin >> l_parenthesis >> num >> r_parenthesis >> comma;
                
                if (ch == 's') {
                    shapes.push_back(make_unique<Square>(num));
                }
                else {
                    shapes.push_back(make_unique<Circle>(num));
                }
                
                break;
            default:
                std::cout << "Unknown Shape" << std::endl;
                std::cin.setstate(std::ios_base::failbit);
                break;
        }
        
        std::cin >> std::ws;
    }
    
    for (std::size_t i = 0; i < shapes.size(); ++i) {
        std::cout << std::setprecision(4) << shapes[i].get()->square();
        
        if (i + 1 != shapes.size()) {
             std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    return 0;
}
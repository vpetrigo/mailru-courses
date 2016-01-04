#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <regex>

using namespace std;

class Student {
public:
  Student() {}

  Student(const Student& s) : Name{s.Name}, Surname{s.Surname}, Midname{s.Midname}, Birth_date{s.Birth_date},
                              Address{s.Address}, Phone{s.Phone}, Department{s.Department}, Course{s.Course} {}

  Student& operator=(const Student& s) {
    if (this != &s) {
      Student(s).swap(*this);
    }

    return *this;
  }

  ~Student() {}

  void swap(Student& s) {
    ::swap(Name, s.Name);
    ::swap(Surname, s.Surname);
    ::swap(Midname, s.Midname);
    ::swap(Birth_date, s.Birth_date);
    ::swap(Address, s.Address);
    ::swap(Phone, s.Phone);
    ::swap(Department, s.Department);
    ::swap(Course, s.Course);
  }

  void set_name(const string& n) {
    Name = n;
  }

  const string& get_name() const {
    return Name;
  }

  void set_surname(const string& s) {
    Surname = s;
  }

  const string& get_surname() const {
    return Surname;
  }

  const string& get_midname() const {
    return Midname;
  }

  void set_midname(const string& m) {
    Midname = m;
  }

  const string& get_birthdate() const {
    return Birth_date;
  }

  void set_birthdate(const string& b) {
    Birth_date = b;
  }

  const string& get_address() const {
    return Address;
  }

  void set_addres(const string& a) {
    Address = a;
  }

  const string& get_phone() const {
    return Phone;
  }

  void set_phone(const string& p) {
    Phone = p;
  }

  const string& get_department() const {
    return Department;
  }

  void set_department(const string& d) {
    Department = d;
  }

  const int& get_course() const {
    return Course;
  }

  void set_course(const int c) {
    Course = c;
  }

private:
  string Name;
  string Surname;
  string Midname;
  string Birth_date;
  string Address;
  string Phone;
  string Department;
  int Course;
};

// read comma separated data
string read_data(istream& is) {
  string d;
  char ch = '\0';
  constexpr char delim = ',';

  while (is.get(ch) && isspace(ch)) {
    // skip whitespaces
  }

  is.unget();

  while (is.get(ch) && ch != delim) {
    d += ch;
  }

  is.unget();

  return d;
}

void check_delimiter(istream& is, const char delim) {
  if (is.get() != delim) {
    throw runtime_error{"Bad input"};
  }
}

istream& operator>>(istream& is, Student& s) {
  vector<function<void(const string& c)>> v {
                                             bind(&Student::set_surname, &s, placeholders::_1),
                                             bind(&Student::set_name, &s, placeholders::_1),
                                             bind(&Student::set_midname, &s, placeholders::_1),
                                             bind(&Student::set_birthdate, &s, placeholders::_1),
                                             bind(&Student::set_addres, &s, placeholders::_1),
                                             bind(&Student::set_phone, &s, placeholders::_1),
                                             bind(&Student::set_department, &s, placeholders::_1)
                                           };
  for (auto &elem : v) {
    elem(read_data(is));
    check_delimiter(is, ',');
  }

  int course;

  is >> course;
  s.set_course(course);
  check_delimiter(is, '.');

  return is;
}

ostream& operator<<(ostream& os, const Student& s) {
  const string delim = ", ";

  cout << s.get_surname() << delim << s.get_name() << delim << s.get_midname()
    << delim << s.get_birthdate() << delim << s.get_address() << delim
    << s.get_phone() << delim << s.get_department() << delim << s.get_course();

  return os;
}

void print_res(const vector<Student>& s) {
  for (auto i = 0; i < s.size(); ++i) {
    cout << s[i];
    if (i + 1 != s.size()) {
      cout << "; ";
    }
  }
  cout << ". ";
}

// accept birthdate
int get_year(const string& birthdate) {
  regex pattern{R"(\d{4})"};
  smatch sm;
  if (!regex_search(birthdate, sm, pattern)) {
    throw runtime_error{"Birth date in the wrong format"};
  }

  return stoi(sm[0]);
}

void check_students(vector<string>& to_find, const vector<Student>& s) {
  vector<Student> tmp;
  int year = stoi(to_find.back());
  to_find.pop_back();
  int course = stoi(to_find.back());
  to_find.pop_back();
  string depart = to_find.back();
  to_find.pop_back();

  for (const auto& elem : s) {
    if (elem.get_department() == depart) {
      tmp.push_back(elem);
    }
  }
  print_res(tmp);
  tmp.clear();

  for (const auto& elem : s) {
    if (elem.get_course() == course) {
      tmp.push_back(elem);
    }
  }
  print_res(tmp);
  tmp.clear();

  for (const auto& elem : s) {
    if (get_year(elem.get_birthdate()) >= year) {
      tmp.push_back(elem);
    }
  }
  print_res(tmp);
}

vector<string> get_search() {
  vector<string> to_find;

  for (int i = 0; i < 3; ++i) {
    to_find.push_back(read_data(cin));

    if (i + 1 != 3) {
      check_delimiter(cin, ',');
    }
  }

  return to_find;
}

int main() {
  constexpr int inp_size = 2;
  vector<Student> v;

  for (int i = 0; i < inp_size; ++i) {
    Student s;

    cin >> s;
    v.push_back(s);
  }

  string dep, birth_date;
  int course;

  auto to_find = get_search();
  check_students(to_find, v);

  return 0;
}

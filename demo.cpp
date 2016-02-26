#include <iostream>
#include <string>
#include <iosfwd>

using namespace std;
struct Attributes{
	string name;
	int age;
};

class Person{
public:
	Person(string& n, int a);
	~Person();
	string& getName()const { return attrs->name; }
	int& getAge() const { return attrs->age; }
	void setName(string& name);
	void setAge(int age);
	friend ostream& operator<<(ostream&, Person&);
private:
	Attributes* attrs;
};

ostream& operator<<(ostream& os, Person& p){
	os<<'{'<<p.attrs->name<<','<<p.attrs->age<<'}';
	return os;
}

Person::Person(string& n, int a){
	attrs = new Attributes;
	attrs->name = n;
	attrs->age = a;
	cout<<"Person()"<<endl;
}

Person::~Person(){
	delete attrs;
	cout<<"~Person()"<<endl;
}

void Person::setName(string& name){
	attrs->name = name;
}

void Person::setAge(int age){
	attrs->age = age;
}


int main(int argc, char* argv[]){
	string name = "fengyin";
	Person p(name, 24);
	cout<<p<<endl;
	p.setAge(18);
	cout<<p<<endl;
	
	return 0;
}

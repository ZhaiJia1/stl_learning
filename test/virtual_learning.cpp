#include<iostream>
#include<functional>
using namespace std;


//考虑virtual之外的其他选择
//使用函数指针


class GameCharacter;
short defaultHealthCalc(const GameCharacter&) {
	cout << "defaultFunc" << endl;
	return 11;
}

struct HealthCalctor {
	int operator()(const GameCharacter& gc)const {
		cout << "struct HealthFunc" << endl;
		return 1;
	}
};


class GameCharacter {

public:
	//函数指针
	//typedef int (*HealthCalcFunc)(const GameCharacter&);

	typedef  function<int(const GameCharacter&)> HealthCalcFunc;

	explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc,const string& info = "GameCharacter is created!") :health(0), healthFunc(hcf) {
		logInfo(info);
	}

	//拷贝赋值运算符重载
	GameCharacter& operator = (const GameCharacter& gc_){
		health = gc_.health;
		return *this;
	}


	void logInfo(const string& info)const {
		cout << info << endl;
	}

	int healthValue()const {
		return healthFunc(*this);
	}
	void setHealthCalcFunc(HealthCalcFunc newHealthCalcFunc) {
		healthFunc = newHealthCalcFunc;
	}
	enum AttackType { quick, heavy, urtal };
	
	void attack(AttackType at_ = quick) const {
		doattack(at_);
	}
	int health = 0;
private:
	virtual void doattack(AttackType at_) const = 0;
	HealthCalcFunc healthFunc;
};


class EvilBodGuy :public GameCharacter {
public:
	explicit EvilBodGuy(HealthCalcFunc hcf = defaultHealthCalc):GameCharacter(hcf,createLogInfo()){

	}

private:
	static string createLogInfo() {
		return "EvilBodGuy is created!";
	}
	virtual void doattack(AttackType at_ ) const {
		cout << "do " << at_ << " attack" << endl;
	}
};

class FinalBoss :public GameCharacter {
public:
	explicit FinalBoss(HealthCalcFunc hcf = defaultHealthCalc) :GameCharacter(hcf, createLogInfo()) {

	}

private:
	static string createLogInfo() {
		return "FinalBoss is created!";
	}
	virtual void doattack(AttackType at_) const {
		cout << "do " << at_ << " attack" << endl;
	}
};


class GameLevel {
public:
	float health(const GameCharacter& gc) const {
		cout << "GameLevel health func" << endl;
		return 0.1;
	}
};


int loseslowly(const GameCharacter& gc) {
	cout << "loseslowlyFunc" << endl;
	return gc.health;
}
int losequickly(const GameCharacter& gc) {
	cout << "losequicklyFunc" << endl;
	return gc.health;
}





class Person {
public:
	Person():age(-1) {
		cout << "Base is constructed with -1!" << endl;
	}
	Person(const Person& per_ ) :age(per_.age) {
		cout << "Base is constructed with copy!" << endl;
	}
	explicit Person(int age_) : age(age_) {
		cout << "Base is constructed with int !" << endl;
	}
	//explicit被用来阻止隐式类型转换
	virtual ~Person()
	{
		cout << "Base is destroyed!" << endl;
	}
	int getAge() {
		return age;
	}

	virtual void printAge() {
		cout << "Age is:" << age << endl;
	}
	
	void setAge(int newAge) {
		age = newAge;
	}

private:
	int age;
};

class Student : public Person{
public:
	Student() :schoolAge(-1) {
		cout << "Student is constructed with -1" << endl;
	}
	explicit Student(int age_,int schage_) :schoolAge(schage_) {
		cout << "Student is constructed with int" << endl;
	}
	Student(const Student& stu_ ) :schoolAge(stu_.schoolAge) {
		cout << "Student is constructed with copy" << endl;
	}
	~Student()
	{
		cout << "Student is destroyed!" << endl;
	}
	int getschoolAge() {
		if (schoolAge == -1) {
			cout << "schoolAge is not initiallized!<" << endl;
		}
		return schoolAge;
	}
	virtual void printAge() {
		Person::printAge();
		cout << "schoolAge is :" << schoolAge << endl;
	}

private:
	int schoolAge;
};

void doSomething(Student s) {
	cout << "print process in type of pass-by-value" << endl;
	cout << endl;
}

void doSomething2(const Student& s) {
	cout << "print process in type of pass-by-reference-to-const" << endl;
	cout << endl;
}


Person* returnPerson(Student* stu) {
	return stu;
}

//int main() {
//	
//	GameCharacter* gc1 = new EvilBodGuy();
//	GameCharacter* gc2 = gc1;
//	GameCharacter* gc3 = new FinalBoss();
//	gc1->attack();
//	
//	gc2->attack(GameCharacter::heavy);
//	gc3->attack(GameCharacter::urtal);
//	
//	return 0;
//}
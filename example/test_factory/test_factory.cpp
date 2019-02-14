#define SF_DEBUG
#include "sf_object_factory.hpp"
#include "sf_stdc++.h"
using namespace skyfire;

struct Student
{
	std::string name;
	int age;
	bool male;
};

struct StudentExt : public Student
{
	std::string address;
};

SF_JSONIFY(Student, name, age, male);
SF_JSONIFY(StudentExt, name, age, male, address);

void print_student(Student st)
{
	std::cout << "name:" << st.name << std::endl;
	std::cout << "age:" << st.age << std::endl;
	std::cout << "male:" << st.male << std::endl;
}

void print_student_ext(StudentExt st)
{
	print_student(st);
	std::cout << "address:" << st.address << std::endl;
}


int main()
{
	sf_object_factory factory;
	factory.load_config_file(R"(C:\code\sflib\example\test_factory\config.json)");
	auto student1 = factory.get_object<Student>("student1");
	auto student2 = factory.get_object<StudentExt>("student2");
	auto student3 = factory.get_object<StudentExt>("student3");
	print_student(*student1);
	print_student_ext(*student2);
	print_student_ext(*student3);

	getchar();
}
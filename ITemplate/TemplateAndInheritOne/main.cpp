#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

class DefaultPolicy1{};
class DefaultPolicy2{};
class DefaultPolicy3{};
class DefaultPolicy4{};


class DefaultPolicies
{
public:
	typedef DefaultPolicy1 P1;
	typedef DefaultPolicy2 P2;
	typedef DefaultPolicy3 P3;
	typedef DefaultPolicy4 P4;
};

class DefaultPolicyArgs : virtual public DefaultPolicies
{
	;
};


template <typename Base, int D>
class Discriminator : public Base
{
	;
};

template <typename Setter1,
typename Setter2,
typename Setter3,
typename Setter4>
class PolicySelector : public Discriminator<Setter1, 1>, 
					   public Discriminator<Setter2, 2>,
					   public Discriminator<Setter3, 3>,
					   public Discriminator<Setter4, 4>
{
	
};

template <typename PolicySetter1 = DefaultPolicyArgs,
typename PolicySetter2 = DefaultPolicyArgs,
typename PolicySetter3 = DefaultPolicyArgs,
typename PolicySetter4 = DefaultPolicyArgs>
class BreadSlicer
{
public:
	typedef PolicySelector<PolicySetter1, PolicySetter2, PolicySetter3, PolicySetter4> Policies;
	void DoTest()
	{
		printf("Policies::P1 is %s\n", typeid(Policies::P1).name());
		printf("Policies::P2 is %s\n", typeid(Policies::P2).name());
		printf("Policies::P3 is %s\n", typeid(Policies::P3).name());
		printf("Policies::P4 is %s\n", typeid(Policies::P4).name());
	}
};

template <typename Policy>
class Policy1_is : virtual public DefaultPolicies
{
public:
	typedef Policy P1;
};

template <typename Policy>
class Policy2_is : virtual public DefaultPolicies
{
public:
	typedef Policy P2;
};


template <typename Policy>
class Policy3_is : virtual public DefaultPolicies
{
public:
	typedef Policy P3;
};

template <typename Policy>
class Policy4_is : virtual public DefaultPolicies
{
public:
	typedef Policy P4;
};

class CustomPolicy
{

};



int main(void)
{
	BreadSlicer<Policy3_is<CustomPolicy> > bc;
	bc.DoTest();


	system("pause");
	return 0;
}
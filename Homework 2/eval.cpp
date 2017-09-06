#include "Map.h"
#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <cassert>
using namespace std;

const bool isInfix(string infix);
string postfixConvert(string infix);
const bool allLettersInMap(string infix, const Map& values);
bool divideByZero(string postfix, const Map& values);
int getResult(string postfix, const Map& values);
const bool precedence(char lhs, char rhs);

int evaluate(string infix, const Map& values, string& postfix, int& result)
// Evaluates an integer arithmetic expression
// Precondition: infix is an infix integer arithmetic
//   expression consisting of single lower case letter operands,
//   parentheses, and the operators +, -, *, /, with embedded blanks
//   allowed for readability. 
// Postcondition: If infix is a syntactically valid infix integer
//   expression whose only operands are single lower case letters
//   (whether or not they appear in the values map), then postfix is
//   set to the postfix form of the expression; otherwise postfix may
//   or may not be changed, result is unchanged, and the function
//   returns 1.  If infix is syntactically valid but contains at
//   least one lower case letter operand that does not appear in the
//   values map, then result is unchanged and the function returns 2.
//   If infix is syntactically valid and all its lower case operand
//   letters appear in the values map, then if evaluating the
//   expression (using for each letter in the expression the value in
//   the map that corresponds to it) attempts to divide by zero, then
//   result is unchanged and the function returns 3; otherwise,
//   result is set to the value of the expression and the function
//   returns 0.
{
	if (!isInfix(infix)) return 1; //Invalid infix

	postfix = postfixConvert(infix);

	if (!allLettersInMap(infix, values)) return 2;

	if (divideByZero(postfix, values)) return 3;

	result = getResult(postfix, values);

	return 0;
}

const bool isInfix(string infix) //Check if it is an infix expression
{
	//Check if empty
	if (infix == "") return false;

	//Check lowercase
	//Check operands
	char operandList[7] = { '+', '-', '*', '/', '(', ')', ' ' };
	for (int i = 0; i < infix.size(); i++)
	{
		if (isalpha(infix[i]) && isupper(infix[i])) return false; //lowercase
		if (!isalpha(infix[i])) //operands
		{
			bool match = false; //to check operands
			for (int k = 0; k < 7; k++) //check against list
				if (infix[i] == operandList[k]) match = true;

			if (match == false) return false;
		}
	}

	//Check letter:operand ratio
	int alphas = 0;
	int otherOps = 0;
	int leftPara = 0;
	int rightPara = 0;

	for (int i = 0; i < infix.size(); i++)
	{
		if (isalpha(infix[i])) alphas++;
		if (infix[i] == '+' || infix[i] == '-' || infix[i] == '*' || infix[i] == '/') otherOps++;
		if (infix[i] == '(') leftPara++;
		if (infix[i] == ')') rightPara++;
	}
	if ((alphas - otherOps) != 1) return false;

	//Check bracket ratio
	if (leftPara != rightPara) return false;

	return true; //Passed all checks
}

string postfixConvert(string infix) //Conversion to postfix form
{
	string postfix = ""; //clear postfix string just in case

	stack<char> cStack;

	for (int i = 0; i<infix.size(); i++) //for each character in infix string
	{
		if (isalpha(infix[i]) && islower(infix[i]))
			postfix += infix[i];
		else if (infix[i] == '(')
			cStack.push(infix[i]); //push it into stack
		else if (infix[i] == ')')
		{
			while (!cStack.empty() && cStack.top() != '(') //as long as stack isn't empty and isn't '('
			{
				postfix += cStack.top();
				cStack.pop(); //pop all operators between parenthesis
			}
			cStack.pop(); //remove the '('
		}
		else if (infix[i] == '*' || infix[i] == '+' || infix[i] == '-' || infix[i] == '/')
		{
			while (!cStack.empty() && cStack.top() != '(' && precedence(infix[i], cStack.top()))
			{
				postfix += cStack.top(); //pop off operators when conditions are met
				cStack.pop();
			}
			cStack.push(infix[i]); //push in new operator
		}
	}

	while (!cStack.empty()) //pop the remaining operators into the postfix string
	{
		postfix += cStack.top();
		cStack.pop();
	}

	return postfix;
}

const bool allLettersInMap(string infix, const Map& values) //Returns true if all letters in infix correspond to a key in Map
{
	for (int i = 0; i<infix.size(); i++)
	{
		char temp = infix[i];
		if (isalpha(temp) && islower(temp) && !values.contains(temp)) //lower case letter without matching key in map, return false
			return false;
	}
	return true;
}

bool divideByZero(string postfix, const Map& values) //Returns true if postfix string attempts to divide something by zero, use getresult function as a model
{
	if (postfix.size() == 1) return false; //no operators

	stack<int> leStack;
	for (int i = 0; i < postfix.size(); i++)
	{
		char token = postfix[i];
		if (isalpha(token)) //add int value mapped to by token to stack
		{
			int temp = 0;
			values.get(token, temp);
			leStack.push(temp);
		}
		else //operator
		{
			int operand2 = leStack.top();
			leStack.pop();
			int operand1 = leStack.top();
			leStack.pop();

			int answer = 0;

			switch (token)
			{
			case '+':
				answer = operand1 + operand2;
				break;
			case '-':
				answer = operand1 - operand2;
				break;
			case '*':
				answer = operand1 * operand2;
				break;
			case '/':
				if (operand2 == 0)	return true;
				else
					answer = operand1 / operand2;
				break;
			}
			leStack.push(answer);
		}
	}
	return false;
}

int getResult(string postfix, const Map& values) //Calculates final value of postfix 
{
	if (postfix.size() == 1) //no operators
	{
		int temp = 0;
		values.get(postfix[0], temp);
		return temp;
	}

	stack<int> leStack;
	for (int i = 0; i < postfix.size(); i++) //traverse postfix
	{
		char token = postfix[i];
		if (isalpha(token)) //add int value mapped to by token to stack
		{
			int temp = 0;
			values.get(token, temp);
			leStack.push(temp);
		}
		else //operator
		{
			int operand2 = leStack.top();
			leStack.pop();
			int operand1 = leStack.top();
			leStack.pop();

			int answer = 0;

			switch (token)
			{
			case '+':
				answer = operand1 + operand2;
				break;
			case '-':
				answer = operand1 - operand2;
				break;
			case '*':
				answer = operand1 * operand2;
				break;
			case '/':
				answer = operand1 / operand2;
				break;
			}
			leStack.push(answer);
		}
	}
	return int(leStack.top());
}

const bool precedence(char lhs, char rhs) //Compares precedence of operands, returns true if rhs is >= precedence of lhs
{
	if (lhs == '*' || lhs == '/')
		if (rhs == '+' || rhs == '-')
			return false;
	return true;
}


int main()
{
	char vars[] = { 'a', 'e', 'i', 'o', 'u', 'y', '#' };
	int  vals[] = { 3,  -9,   6,   2,   4,   1 };
	Map m;
	for (int k = 0; vars[k] != '#'; k++)
		m.insert(vars[k], vals[k]);
	string pf;
	int answer;
	assert(evaluate("a+ e", m, pf, answer) == 0 &&
		pf == "ae+"  &&  answer == -6);
	answer = 999;
	assert(evaluate("", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a+", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a i", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("ai", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("()", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("y(o+u)", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a+E", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("(a+(i-o)", m, pf, answer) == 1 && answer == 999);
	// unary operators not allowed:
	assert(evaluate("-a", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a*b", m, pf, answer) == 2 &&
		pf == "ab*"  &&  answer == 999);
	assert(evaluate("y +o *(   a-u)  ", m, pf, answer) == 0 &&
		pf == "yoau-*+"  &&  answer == -1);
	answer = 999;
	assert(evaluate("o/(y-y)", m, pf, answer) == 3 &&
		pf == "oyy-/"  &&  answer == 999);
	assert(evaluate(" a  ", m, pf, answer) == 0 &&
		pf == "a"  &&  answer == 3);
	assert(evaluate("((a))", m, pf, answer) == 0 &&
		pf == "a"  &&  answer == 3);
	cout << "Passed all tests:)" << endl;
}
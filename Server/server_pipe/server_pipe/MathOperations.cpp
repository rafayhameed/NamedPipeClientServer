#include "pch.h"
#include "MathOperations.h"

int MathOperations::Add(int x, int y)
{
	return (x + y);
}

int MathOperations::Sub(int x, int y)
{
	return x - y;
}


int MathOperations::perform(MessageServiceReq* obj) {
	switch (obj->functionID) {
	case AddFun:
		return Add(obj->param1, obj->param2);
		break;
	case SubFun:
		return Sub(obj->param1, obj->param2);
		break;
	}

}

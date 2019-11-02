#pragma once
#include "CustomClass.h"
#include "utils.h"


class MathOperations : public CustomClass
{
public:

	int perform(MessageServiceReq* param);
	int Add(int x, int y);
	int Sub(int x, int y);
};


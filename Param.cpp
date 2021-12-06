#include "Bela.h"
#include "param.h"

Param::Param(float *sr, float *s)
{
	setup(s, sr);
}

void Param::setup(float *sr, float *s)
{
	steps = (int)(*s * *sr);
}

void Param::process()
{
	if(count) {
		current = target - delta * count;
		count--;
	}
}

void Param::setCurrent(float v)
{
	count = 0;
	target = v;
	current = v;
	delta = 0;
}

float Param::getCurrent() const
{
	return current;
}

void Param::setTarget(float v)
{
	count = steps;
	target = v;
	delta = (target - current) / count;
}

float Param::getTarget() const
{
	return target;
}

bool Param::getStatus() const {
	return false;
}
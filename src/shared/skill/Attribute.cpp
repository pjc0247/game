﻿#include "Attribute.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

Attribute::Attribute() :
	value(0),
	bonusRate(0), bonusValue(0){
}
Attribute::Attribute(
	float _bonusValue, float _bonusRate) :
	value(0), maxValue(0),
	bonusRate(_bonusRate), bonusValue(_bonusValue){
}
Attribute::~Attribute(){
}

void Attribute::set(
	float _value){

	value = _value;
	maxValue = _value;
}

float Attribute::increase(
	float plus){
	
	value = MAX(MIN(value + plus, maxValue), 0);
	return value;
}

float Attribute::upgrade(
	float plus){
	
	maxValue = MAX(maxValue + plus, 0);
	value = maxValue;
	return maxValue;
}

float Attribute::get(){
	return (value + bonusValue)
		* (1.0f + bonusRate);
}

float &Attribute::getMaxValue(){
	return maxValue;
}
float &Attribute::getValue(){
	return value;
}
float &Attribute::getBonusRate(){
	return bonusRate;
}
float &Attribute::getBonusValue(){
	return bonusValue;
}
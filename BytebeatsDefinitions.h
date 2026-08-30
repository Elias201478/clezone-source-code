#pragma once
#include "Audio.h"
#include <math.h>
#include <stdint.h>


unsigned char Bytebeat_One(unsigned int t) {
	return t * ((t << 3 | t) * t >> t ^ t);
}
unsigned char Bytebeat_Two(unsigned int t) {
	return ((t >> 4) * (t >> 6) ^ t * 9) & 255;
}
unsigned char Bytebeat_Three(unsigned int t) {
	return 21 * (t >> 8 & t) - (t >> 5) + t * (t >> 141 & 14);
}
unsigned char Bytebeat_Four(unsigned int t) {
	return ((t >> 4) * (t >> 8) ^ t * 13) & 255;

}
unsigned char Bytebeat_Five(uint32_t t) {
	return ((t >> 2) * (t >> 6) ^ t * 7);
}
unsigned char Bytebeat_Six(uint32_t t) {
	return (t ^ (t >> 3) ^ (t * 7) ^ (t >> 5)) & (t * (12 + ((t >> 13) & 3)));
}
unsigned char Bytebeat_Seven(uint32_t t) {
	return (((t * (1 + (((t >> 12) ^ (t >> 15)) & 3))) << 2) * (t >> 8) ^ t * 13) & 255;
}
#include "utils.h"

bool compareStr(c*str1, c*str2) {
	c a=str1[0];
	c b=str2[0];
	size_t count = 0;
	while (a==b) {
		if (a==0) return true; // but they still equal, so if a==b==0 then that means both strings are same length and just ended.
		a=str1[count];
		b=str2[count++];
	}
	return false;
}

c*concatStr(c*into, c*what) {
	size_t count1 = 0;
	while (into[count1]!=0) count1++;
	size_t count2 = 0;
	while (what[count2]!=0) {
		into[count1+count2] = what[count2];
		count2++;
	}
	into[count1+count2] = 0; // if the |while (what[count2]!=0)| loop ended, then that means |what[count2]==0| therefore i could've saved bytes if it wasn't for this explanation for future twinks
	return into;
}

void*memcpy(void*dest, void*src, size_t n) {
	size_t count = 0;
	while (count<n) {
		*((c*)dest+count) = *((c*)src+count);
		count++;
	}
	return(dest);
}

void*memset(void*dest, int byte, size_t n) {
	size_t count = 0;
	while (count<n) {
		*((c*)dest+count) = byte;
		count++;
	}
	return(dest);
}

bool sStarts(c*longer, c*shorter) {
	while (*shorter != 0) {
		if (*longer!=*shorter) return false;
		longer++;
		shorter++;
	}
	return true;
}

int parseInt(c*str, size_t limit) { // behaves similar to js's |parseInt| but the base is now a limit.
	if (*str == 45) return 0-parseInt(str+1,limit-1);
	size_t count = 0;
	int res = 0;
	while ((limit==0)||(count<limit)) {
		if ((str[count]<48)||(str[count]>57)) return res; // |parseInt("123a") == 123|, we don't have |NaN| |int|s in c, |0 < 48| (unless 1st grader math breaks somehow)
		res = (res*10)+(str[count]-48);
		count++;
	}
	return res; // backup return in case we got limited
}

c*i2s(int in, c*s) {
	if (in<0) {
		s[0] = 45;
		return i2s(0-in, s+1)-1; // negative ints would be a separate pain to separately handle a separate edge case with
	}
	if (in==0) {
		s[0] = 48;
		s[1] = 0;
		return s;
	}
	int backup = in;
	size_t len = 0;
	while (in>0) {
		len++;
		in /= 10;
	}
	s[len]=0;
	in = backup;
	while (in>0) {
		s[--len] = (in%10)+48;
		in /= 10;
	}
	return s;
}

c*d2s(double in, c*s, size_t limit) {
	if (in<0) {
		s[0] = 45;
		return d2s(0-in, s+1, limit-1)-1;
	}
	i2s((int)in, s); // truncate number and stringify the truncated part
	size_t len = 0;
	double left = in-(int)in; // the truncated off part.
	while (s[len]!=0) len++;
	s[len++] = 46;
	if (left==0) {
		s[len++] = 48;
		s[len] = 0;
		return s; // append .0 and null term :3
	}
	while ((left!=0)&&((limit==0)||(len<limit))) {
		left *= 10; // extract the latest digit
		s[len++] = (((int)left)%10)+48;
		left -= (double)((int)left); // truncate an reconvert
	}
	s[len] = 0;
	return s;
}

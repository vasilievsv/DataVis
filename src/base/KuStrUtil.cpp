﻿#include "KuStrUtil.h"
#include <sstream>
#include <assert.h>
#include <regex>


void KuStrUtil::toUpper(char* str)
{
	while (*str != 0) {
		*str = std::toupper(*str);
		++str;
	}
}


void KuStrUtil::toUpper(std::string& str) 
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) -> unsigned char { return std::toupper(c); });
}


void KuStrUtil::toLower(char* str) 
{
	while (*str != 0) {
		*str = std::tolower(*str);
		++str;
	}
}


void KuStrUtil::toLower(std::string& str) 
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) -> unsigned char { return std::tolower(c); });
}


int KuStrUtil::count(const std::string_view& str, char ch)
{
	int c(0);
	for (unsigned i = 0; i < str.size(); i++)
		if (str[i] == ch)
			++c;
	return c;
}


bool KuStrUtil::beginWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase)
{
	if (nstr < nwith)
		return false;

	return nocase ? _strnicmp(str, with, nwith) == 0 : strncmp(str, with, nwith) == 0;
}


bool KuStrUtil::beginWith(const char* str, const char* with, bool nocase)
{
	return beginWith(str, length(str), with, length(with), nocase);
}


bool KuStrUtil::beginWith(const std::string& str, const std::string& with, bool nocase)
{
	return beginWith(str.c_str(), str.length(), with.c_str(), with.length(), nocase);
}


bool KuStrUtil::endWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase)
{
	if (nstr < nwith)
		return false;

	str += nstr - nwith;
	return nocase ? _strnicmp(str, with, nwith) == 0 : strncmp(str, with, nwith) == 0;
}


bool KuStrUtil::endWith(const char* str, const char* with, bool nocase)
{
	return endWith(str, length(str), with, length(with), nocase);
}


bool KuStrUtil::endWith(const std::string& str, const std::string& with, bool nocase)
{
	return endWith(str.c_str(), str.length(), with.c_str(), with.length(), nocase);
}


const char* KuStrUtil::findFirstOf(const char* str, char ch)
{
	assert(str != nullptr);
	return std::strchr(str, ch);
}


const char* KuStrUtil::findFirstNotOf(const char* str, char ch)
{
	assert(str != nullptr);

	if (*str == '\0' && ch == '\0')
		return nullptr;

	while (*str == ch) ++str;

	return *str == '\0' ? nullptr : str;
}


const char* KuStrUtil::findFirstOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	auto pos = std::strcspn(str, which);
	return str[pos] == '\0' ? nullptr : str + pos;
}


const char* KuStrUtil::findFirstNotOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	for (; *str != '\0'; str++)
		if (std::strchr(which, *str) == nullptr)
			return str;

	return nullptr;
}


const char* KuStrUtil::findLastOf(const char* str, char ch)
{
	assert(str != nullptr);
	return std::strrchr(str, ch);
}


const char* KuStrUtil::findLastNotOf(const char* str, char ch)
{
	assert(str != nullptr);

	if (*str == '\0' && ch == '\0')
		return nullptr;

	for (auto buf = str + length(str) - 1; buf >= str; buf--)
		if (*buf != ch)
			return buf;

	return nullptr;
}


const char* KuStrUtil::findLastOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	for(auto buf = str + length(str) - 1; buf >= str; buf--) 
		if (std::strchr(which, *buf) != nullptr)
			return buf;

	return nullptr;
}


const char* KuStrUtil::findLastNotOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	for (auto buf = str + length(str) - 1; buf >= str; buf--)
		if (std::strchr(which, *buf) == nullptr)
			return buf;

	return nullptr;
}


bool KuStrUtil::replaceSubstr(char* str, const char* substr, const char* with)
{
    assert(length(substr) == length(with));
	bool replaced = false;
	while (str = std::strstr(str, substr)) {
		replaced = true;
		const char* ch = with;
		while (*ch != '\0')
			*str++ = *ch++;
	}

	return replaced;
}


bool KuStrUtil::replaceSubstr(std::string& str, const std::string& substr, const std::string& with)
{
	std::string::size_type pos = 0;
	bool replaced = false;
	while ((pos = str.find(substr, pos)) != std::string::npos) {
		replaced = true;
		str.replace(pos, substr.length(), with);
		pos += with.length();
	}

	return replaced;
}


void KuStrUtil::replaceChar(char* str, char ch, char with)
{
	while (str = const_cast<char*>(findFirstOf(str, ch)))
			*str++ = with;
}


void KuStrUtil::replaceChar(std::string& str, char ch, char with)
{
	std::replace(str.begin(), str.end(), ch, with);
}


void KuStrUtil::eraseFirstOf(char* str, char ch)
{
	auto p = const_cast<char*>(findFirstOf(str, ch));
	if (p) while (*p != '\0') *p = *++p;
}


void KuStrUtil::eraseAll(char* str, char ch)
{
	auto p = str;
	while (*str != '\0') {
		while (*str++ == ch); // skip ch
		*p++ = *str++; // copy non-ch
	}
}


void KuStrUtil::eraseAll(std::string& str, const std::string& substr)
{
	replaceSubstr(str, substr, "");
}


const char* KuStrUtil::trimLeftInPlace(const char* str, const char* spaces)
{
	auto pos = findFirstNotOneOf(str, spaces);
	return pos ? pos : str + length(str);
}


void KuStrUtil::trimRightInPlace(char* str, const char* spaces)
{
	auto pos = const_cast<char*>(findLastNotOneOf(str, spaces));
	if (pos != nullptr)
		*++pos = '\0';
}


char* KuStrUtil::trimInPlace(char* str, const char* spaces)
{
	trimRightInPlace(str, spaces);
	return const_cast<char*>(trimLeftInPlace(str, spaces));
}


void KuStrUtil::trim(std::string& str, const char* spaces)
{
	auto pos = str.find_last_not_of(spaces);
	if (pos != std::string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(spaces);
		if (pos != std::string::npos) str.erase(0, pos);
	}
	else {
		str.erase(str.begin(), str.end());
	}
}


std::string_view KuStrUtil::trim(const std::string_view& sv, const char* spaces)
{
	std::string_view v = sv;
	v.remove_prefix(std::min(v.find_first_not_of(spaces), v.size()));

	auto trim_pos = v.find_last_not_of(spaces);
	if (trim_pos != v.npos)
		v.remove_suffix(v.size() - trim_pos - 1);

	return v;
}


std::vector<std::string_view> KuStrUtil::split(const std::string_view& full, const std::string& delims, bool skipEmpty)
{
    std::vector<std::string_view> tokens;
	size_t start = 0, found = 0, end = full.size();
	while ((found = full.find_first_of(delims, start)) != std::string::npos) {
		if (found != start)
			tokens.push_back(full.substr(start, found - start));
		else if (!skipEmpty)
			tokens.push_back("");
		start = found + 1;
	}

	if (start != end)
		tokens.push_back(full.substr(start));
	else if (!skipEmpty)
		tokens.push_back("");

    return tokens;
}


std::vector<std::string_view> KuStrUtil::splitWithQuote(const std::string_view& full, const std::string& delims, bool skipEmpty)
{
	auto pos = full.find_first_of("'\"");
	if (pos == std::string_view::npos)
		return split(full, delims, skipEmpty);

	auto last = std::string_view(full.data() + pos + 1, full.size() - pos - 1).find(full[pos]);
	if (last == std::string_view::npos)
		return split(full, delims, skipEmpty); // 不匹配的引号，当作正常字符解析

	std::vector<std::string_view> res;
	if (pos != 0)
		res = split(std::string_view(full.data(), pos), delims, skipEmpty);

	auto qtok = std::string_view{ full.data() + pos, last + 2 };
	res.push_back(qtok);

	auto x = splitWithQuote({qtok.data() + qtok.size(), full.size() - qtok.size() - pos}, delims, skipEmpty);

	res.insert(res.end(), x.begin(), x.end());
	return res;
}


std::vector<std::string> KuStrUtil::splitRegex(const std::string& full, const std::string& regex, bool skipEmpty)
{
	std::vector<std::string> tokens;
	const std::regex re(regex);
	using sti_ = std::sregex_token_iterator;
	tokens.assign(sti_(full.cbegin(), full.cend(), re, -1), sti_());
	return tokens;

/*
	std::smatch sm;
	auto begin = full.cbegin();
	auto end = full.cend();

	while (std::regex_search(begin, end, sm, re)) {
		if(!skipEmpty || begin != sm[0].first)
		    tokens.push_back({ begin, sm[0].first });
		begin = sm.suffix().first;
	}

	if(!skipEmpty || begin != end)
	    tokens.push_back({ begin, end });
*/
}


std::string KuStrUtil::join(const std::vector<std::string> &input, char c)
{
	std::string out;
	if (!input.empty()) {
		out = input[0];
		for (size_t i = 1; i < input.size(); i++)
			out += c + input[i];
	}

	return out;
}

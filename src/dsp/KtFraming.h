﻿#pragma once
#include <vector>
#include <assert.h>


// 对信道数据的分帧处理器，支持多通道处理

template<typename T>
class KtFraming
{
public:

	constexpr static bool isInterleaving() { return true; }

	// @size: 每帧的长度
	// @chann: 数据通道数
	// @shift: =0时，取size值
	// 每个通道延迟size个数据输出
	KtFraming(unsigned size, unsigned chann, unsigned shift)
		: size_(size), chann_(chann), buf_(), shift_(shift ? shift : size) {}

	// 初始化内部状态为initVal，无延时输出
	KtFraming(unsigned size, unsigned chann, unsigned shift, T initVal)
	    : size_(size), chann_(chann), buf_((size - 1) * chann, initVal), shift_(shift ? shift : size) {}

	unsigned size() const { return size_; }
	unsigned shift() const { return shift_; }
	unsigned channels() const { return chann_; }
	unsigned buffered() const { return buf_.size() / channels(); }

	// framing in place
	template<typename OP> void apply(const T* first, const T* last, OP op);

	template<typename OP> void flush(OP op);

	// 将数据压入缓存，延迟执行op操作
	void push(const T* first, const T* last) {
		buf_.insert(buf_.end(), first, last);
	}

	// 清空缓存
	void reset() {
		buf_.clear();
	}

	// 重置
	void reset(unsigned size, unsigned chann, unsigned shift) {
		size_ = size, chann_ = chann, shift_ = shift;
	    buf_.clear();
	}

	// 计算samples组数据可分为多少帧
	// @addBuffered: 若为true，则在samples基础上加入缓存数据进行帧数计算
	unsigned outFrames(unsigned samples, bool addBuffered) const;

	// 执行flush操作，将产生多少帧输出
	unsigned flushFrames() const;

private:

	// 计算需要向缓存拷贝多少数据，才能分帧耗尽缓存
	unsigned needAppended_() const;

	// 在连续数据上执行分帧，不考虑缓存数据，返回残留数据迭代指针
	// NOTE: 此处op使用引用类型，以便apply多次调用时共享op的内部状态
	template<typename OP>
	const T* execute_(const T* first, const T* last, OP& op);


private:
	std::vector<T> buf_;
	unsigned chann_; // 信道数
	unsigned size_; // 帧长
	unsigned shift_; // 帧移
};


template<typename T> template<typename OP>
void KtFraming<T>::apply(const T* first, const T* last, OP op)
{
	auto isize = (last - first) / channels(); // 新增输入的数据数量
	if (outFrames(isize, true) == 0) { // 若数据量不足，仅执行缓存操作
		push(first, last);
		return;
	}

	// 分两段执行分帧：
	//   一是拷贝部分输入到缓存，先耗尽缓存数据，
	//   二是对剩余的输入执行在线处理
	auto copysize = needAppended_();
	auto of = outFrames(copysize, true);
	assert(outFrames(copysize, true) * shift_ >= buffered());
	if (copysize > isize)
		copysize = isize;
	auto pushed = first + copysize * channels(); // 第一阶段压入堆栈的最后位置
	push(first, pushed);
	auto residue = execute_(buf_.data(), buf_.data() + buf_.size(), op); // 执行第一阶段分帧

	if (pushed == last) { // 无剩余数据
		buf_.erase(buf_.begin(), buf_.begin() + (residue - buf_.data()));
		assert(outFrames(0, true) == 0);
	}
	else { // 处理剩余数据
		first = pushed - (buf_.data() + buf_.size() - residue);
		assert(first <= last);

		residue = execute_(first, last, op); // 执行第二阶段分帧
		assert(residue <= last);

		buf_.clear();
		push(residue, last); // 残留数据压入缓存
	}
}


template<typename T> template<typename OP>
void KtFraming<T>::flush(OP op)
{
	auto frames = flushFrames();
	if (frames > 0) {
		buf_.resize((frames - 1) * shift() + size() * channels(), 0);
		auto p = buf_.data();
		for (unsigned i = 0; i < frames; i++, p += shift())
			op(p);
	}
}


template<typename T>
unsigned KtFraming<T>::outFrames(unsigned samples, bool addBuffered) const
{
	if (addBuffered)
		samples += buffered();

	// 考虑shift > size的情况，保证最少能执行一次shift操作时再分帧
	if (samples < std::max(size(), shift()))
		return 0;

	assert(shift() != 0);
	return (samples - std::max(size(), shift())) / shift() + 1;
}


template<typename T>
unsigned KtFraming<T>::flushFrames() const
{
	return unsigned(std::floor(double(buf_.size()) / double(shift())));
}


template<typename T> template<typename OP>
const T* KtFraming<T>::execute_(const T* first, const T* last, OP& op) 
{
	auto isize = (last - first) / channels();
	auto frames = outFrames(isize, false);
	auto shiftRaw = shift_ * channels();
	for (unsigned i = 0; i < frames; i++) {
		op(first);
		first += shiftRaw;
	}

	assert(first <= last);
	return first;
}


template<typename T>
unsigned KtFraming<T>::needAppended_() const 
{
	if (buf_.empty())
		return 0;

	auto c = ((buffered() - 1) / shift()) * shift() + size();
	if (c < shift())
		c = shift();
	
	return c - buffered();
}


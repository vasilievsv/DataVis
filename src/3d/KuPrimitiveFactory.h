#pragma once
#include <array>
#include "KtAABB.h"


// 生成基本的3d构型

class KuPrimitiveFactory
{
public:

	using point3 = std::array<double, 3>;

	enum KeType
	{
		k_position,
		k_normal,
		k_texcoord,

		k_edge_index,
		k_mesh_index
	};


	template<int TYPE, typename T = float, bool CCW = false>
	static int makeBox(const point3& lower, const point3& upper, void* obuf, unsigned stride = 0);


	// 生成grid的quads索引
	// 
	// grid由nx*ny个顶点构成，顶点排列存储顺序如下：
	// 0,  1,      ..., ny-1,
	// ny, ny + 1, ..., 2*ny-1,
	// ...
	// (nx-1)*ny, ..., nx*ny-1
	//
	template<typename IDX_TYPE = unsigned, bool CCW = false>
	static int makeIndexGrid(unsigned nx, unsigned ny, IDX_TYPE* obuf);

private:
	KuPrimitiveFactory() = delete;
};


template<int TYPE, typename T, bool CCW>
int KuPrimitiveFactory::makeBox(const point3& lower, const point3& upper, void* obuf, unsigned stride)
{
	/*
		1-----2
	   /|    /|
	  / |   / |
	 5-----4  |
	 |  0--|--3
	 | /   | /
	 |/    |/
	 6-----7
	*/

	const T* MESH_IDX;

	if constexpr (CCW) {
		static constexpr T MESH_IDX_[] = {
			3, 0, 1, 1, 2, 3,
			7, 6, 0, 0, 3, 7,
			4, 5, 6, 6, 7, 4,
			2, 1, 5, 5, 4, 2,
			7, 3, 2, 2, 4, 7,
			0, 6, 5, 5, 1, 0
		};
		MESH_IDX = MESH_IDX_;
	}
	else {
		static constexpr T MESH_IDX_[] = {
			1, 0, 3, 3, 2, 1,
			0, 6, 7, 7, 3, 0,
			6, 5, 4, 4, 7, 6,
			5, 1, 2, 2, 4, 5,
			2, 3, 7, 7, 4, 2,
			5, 6, 0, 0, 1, 5
		};
		MESH_IDX = MESH_IDX_;
	}

	static constexpr T EDGE_IDX[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		1, 5, 2, 4, 0, 6, 3, 7
	};

	if constexpr (TYPE == k_mesh_index) {
		assert(stride == 0 || stride == sizeof(T));

		if (obuf)
			std::copy(MESH_IDX, MESH_IDX + 36, (T*)obuf);

		return 36;
	}
	else if constexpr (TYPE == k_edge_index) {
		assert(stride == 0 || stride == sizeof(T));

		if (obuf)
			std::copy(std::cbegin(EDGE_IDX), std::cend(EDGE_IDX), (T*)obuf);

		return std::size(EDGE_IDX);
	}
	else {
		if (obuf == nullptr)
			return 8;

		char* buf = (char*)obuf;

		if constexpr (TYPE == k_position) {
			auto aabb = KtAABB<T>(KtPoint<T, 3>(lower[0], lower[1], lower[2]), 
				KtPoint<T, 3>(upper[0], upper[1], upper[2]));
			auto corns = aabb.allCorners();
			constexpr int point_size = sizeof(T) * 3;
			assert(sizeof(corns[0]) == point_size);

			if (stride == 0)
				stride = point_size;

			for (unsigned i = 0; i < corns.size(); i++) {
				auto pt = (std::array<T, 3>*)buf;
				*pt = corns[i];
				buf += stride;
			}
		}
		else if constexpr (TYPE == k_normal) {
			auto aabb = KtAABB<T, 3>(KtPoint<T, 3>(lower[0], lower[1], lower[2]),
				KtPoint<T, 3>(upper[0], upper[1], upper[2]));
			auto corns = aabb.allCorners();
			constexpr int point_size = sizeof(T) * 3;
			assert(sizeof(corns[0]) == point_size);

			if (stride == 0)
				stride = point_size;

			static constexpr char diag[] = {
				4, 7, 6, 5, 0, 3, 2, 1
			};

			for (unsigned i = 0; i < corns.size(); i++) {
				auto pt = (std::array<T, 3>*)buf;
				*pt = corns[i] - corns[diag[i]];
				buf += stride;
			}
		}
		else if constexpr (TYPE == k_texcoord) {
			static_assert(false, "TODO");
		}
		else {
			static_assert(false, "unknown type");
		}

		return 8;
	}
}


template<typename IDX_TYPE, bool CCW>
int KuPrimitiveFactory::makeIndexGrid(unsigned nx, unsigned ny, IDX_TYPE* obuf)
{
	int count = (nx - 1) * (ny - 1) * 4; // 构成grid的quads数量

	if (obuf) {
		for(unsigned i = 1; i < nx; i++)
			for (unsigned j = 1; j < ny; j++) {

				if constexpr (!CCW) {
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + (j - 1));
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + j);
					*obuf++ = static_cast<IDX_TYPE>(i * ny + j);
					*obuf++ = static_cast<IDX_TYPE>(i * ny + (j - 1));
				}
				else {
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + (j - 1));
					*obuf++ = static_cast<IDX_TYPE>(i * ny + (j - 1));
					*obuf++ = static_cast<IDX_TYPE>(i * ny + j);
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + j);	
				}

			}
	}

	return count;
}

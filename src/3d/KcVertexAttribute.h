#pragma once

/// 初始版本参考Nebula3

/** This class declares the usage of a single vertex buffer as a component
    of a complete VertexDeclaration.
    @remarks
    Several vertex buffers can be used to supply the input geometry for a
    rendering operation, and in each case a vertex buffer can be used in
    different ways for different operations; the buffer itself does not
    define the semantics (position, normal etc), the VertexAttribute
    class does.
*/
class KcVertexAttribute
{
public:

	/// Vertex attribute semantics, used to identify the meaning of vertex buffer contents
	enum KeSemantic
	{
		k_position, /// Position, 3 reals per vertex	
		k_normal, /// Normal, 3 reals per vertex	
		k_tangent, /// Tangent (X axis if normal is Z)	
		k_binormal, /// Binormal (Y axis if normal is Z)	
		k_texcoord, /// Texture coordinates
		k_diffuse, /// Diffuse colours	
		k_specular, /// Specular colours
		k_blend_weights, /// Blending weights	
		k_blend_indices, /// Blending indices
		k_instance, // 实例化数组
		k_semantic_count /// The  number of VertexElementSemantic elements
	};

	enum KeFormat
	{
		k_float,			//> one-component float, expanded to (float, 0, 0, 1)
		k_float1 = k_float,
		k_float2,			//> two-component float, expanded to (float, float, 0, 1)
		k_float3,			//> three-component float, expanded to (float, float, float, 1)
		k_float4,			//> four-component float
		k_ubyte4,			//> four-component unsigned byte
		k_short,
		k_short1 = k_short,
		k_short2,			//> two-component signed short, expanded to (value, value, 0, 1)
		k_short3,
		k_short4,			//> four-component signed short
		k_ubyte4_norm,      //> four-component normalized unsigned byte (value / 255.0f)
		k_short1_norm,
		k_short2_norm,	    //> two-component normalized signed short (value / 32767.0f)
		k_short3_norm,
		k_short4_norm,	    //> four-component normalized signed short (value / 32767.0f)
	};

	KcVertexAttribute() = default;

	KcVertexAttribute(unsigned loc, KeFormat fmt, unsigned offset, KeSemantic semantic, unsigned semanticIndex = 0);

	/// Gets the vertex buffer index from where this attribute draws it's values
	unsigned location() const { return location_; }

	/// Gets the offset into the buffer where this attribute starts
	unsigned offset() const { return bytesOffset_; }

	/// Gets the data format of this attribute
	KeFormat format() const { return format_; }

	/// Gets the meaning of this attribute
	KeSemantic semantic() const { return semantic_; }

	/// Gets the index of this attribute, only applicable for repeating elements
	unsigned semanticIndex() const { return semanticIndex_; }

	unsigned divisor() const { return divisor_; }

	// caculate the size in byte of attribute with this format
	unsigned byteSize() const;

	KeFormat baseType() const;

	// get # of component per attribute
	unsigned componentCount() const;

	// get the size in byte of per-component
	unsigned componentByteSize() const;

	bool normalized() const;

	bool enabled() const { return enabled_; }
	void enable(bool b) { enabled_ = b; }

private:

	/// The meaning of the attribute
	KeSemantic semantic_{ k_position };

	union {
		/// The semantic index for the attribute. A semantic index modifies a semantic, 
		/// with an integer index number. A semantic index is only needed in a case 
		/// where there is more than one attribute with the same semantic. 
		/// only applicable for some attributes like texture coords.
		unsigned semanticIndex_{ 0 };

		/// 仅对k_instance语义有效
		/// 表示顶点属性的更新频率，每隔多少个实例将又一次设置实例的该属性，比如设置为1。那么每一个实例的属性都不一样，设置为2则每两个实例同样，3则每三个实例改变属性。
		/// 而该属性的属性数组大小将为(instance / divisor), instance为渲染实例数
		/// 设divisor为2, instance为100, 则实例化数组尺寸至少为100 / 2 = 50，才能保证每一个实例都有自己的属性。
		/// 若divisor设置为0, 代表非实例化
		unsigned divisor_;
	};

	/// The data type of the attribute data. 
	KeFormat format_;

	/// An integer value that identifies the input-assembler. 
	unsigned location_{ 0 };

    /// The offset in the buffer that this attribute starts at.
	/// Use 0 for convenience to define the current attribute directly after the previous one, 
	/// including any packing if necessary. 
	unsigned bytesOffset_{ 0 };

	bool enabled_{ true };
};
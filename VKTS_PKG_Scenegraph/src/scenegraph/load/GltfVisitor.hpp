/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef VKTS_GLTFVISITOR_HPP_
#define VKTS_GLTFVISITOR_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

enum GltfState {
	GltfState_Start,
	GltfState_End,

	GltfState_Error,

	GltfState_Asset,
	GltfState_Buffers,
	GltfState_BufferViews,
	GltfState_Accessors,
	GltfState_Images,
	GltfState_Samplers,
	GltfState_Textures,
	GltfState_Materials,
	GltfState_Meshes,
	GltfState_Skins,
	GltfState_Nodes,
	GltfState_Animations,
	GltfState_Scenes,

	GltfState_Buffer,
	GltfState_BufferView,
	GltfState_Accessor,
	GltfState_Image,
	GltfState_Sampler,
	GltfState_Texture,
	GltfState_Material,
	GltfState_Mesh,
	GltfState_Skin,
	GltfState_Node,
	GltfState_Animation,
	GltfState_Scene,

	GltfState_Material_Extras,
	GltfState_Material_PbrMetallicRoughness,
	GltfState_Material_TextureInfo,
	GltfState_Mesh_Primitive,
	GltfState_Mesh_Primitive_Attributes,
	GltfState_Skin_InverseBindMatrices,
	GltfState_Skin_Joints,
	GltfState_Node_Children,
	GltfState_Animation_Sampler,
	GltfState_Animation_Sampler_Properties,
	GltfState_Animation_Channel,
	GltfState_Animation_Channel_Target,
	GltfState_Scene_Node
};

typedef struct _GltfBuffer {
	IBinaryBufferSP binaryBuffer;
	uint32_t byteLength;
	std::string name;
} GltfBuffer;

typedef struct _GltfBufferView {
	GltfBuffer* buffer;
    uint32_t byteOffset;
    uint32_t byteLength;
    uint32_t byteStride;
	std::string name;
} GltfBufferView;

typedef struct _GltfAccessor {
	GltfBufferView* bufferView;
    uint32_t byteOffset;
    int32_t componentType;
    VkBool32 normalized;
    uint32_t count;
    std::string type;
    Vector<float> max;
    Vector<float> min;
    // TODO: Add 'sparse'.
	std::string name;
} GltfAccessor;

typedef struct _GltfImage {
	IImageDataSP imageData;
	std::string name;
} GltfImage;

typedef struct _GltfSampler {
	int32_t magFilter;
	int32_t minFilter;
	int32_t wrapS;
	int32_t wrapT;
	std::string name;
} GltfSampler;

typedef struct _GltfTexture {
	int32_t format;
	int32_t internalFormat;
	GltfSampler* sampler;
	GltfImage* source;
	int32_t target;
	int32_t type;
	std::string name;
} GltfTexture;

typedef struct _GltfPbrMetallicRoughness {
	float baseColorFactor[4];
	GltfTexture* baseColorTexture;
	float metallicFactor;
	float roughnessFactor;
	GltfTexture* metallicRoughnessTexture;
} GltfPbrMetallicRoughness;

typedef struct _GltfMaterialExtras {
	std::string alphaMode;
	float alphaCutoff;
} GltfMaterialExtras;

typedef struct _GltfMaterial {
	GltfMaterialExtras extras;

	GltfPbrMetallicRoughness pbrMetallicRoughness;

	GltfTexture* normalTexture;
	GltfTexture* occlusionTexture;
	float emissiveFactor[3];
	GltfTexture* emissiveTexture;

	std::string name;
} GltfMaterial;

typedef struct _GltfPrimitive {
	GltfAccessor* position;
	GltfAccessor* normal;
	GltfAccessor* binormal;
	GltfAccessor* tangent;
	GltfAccessor* texCoord;
	GltfAccessor* joint;
	GltfAccessor* weight;
	GltfAccessor* indices;
	int32_t mode;

	GltfMaterial* material;
	std::string name;
} GltfPrimitive;

typedef struct _GltfMesh {
	Vector<GltfPrimitive> primitives;
	std::string name;
} GltfMesh;

struct _GltfNode;

typedef struct _GltfSkin {
	Vector<GltfAccessor*> inverseBindMatrices;
	uint32_t skeleton;
	Vector<uint32_t> joints;
	std::string name;
} GltfSkin;

typedef struct _GltfNode {
	// TODO: Add 'camera',
	Vector<uint32_t> children;
	GltfSkin* skin;
	float matrix[16];
	GltfMesh* mesh;
	float rotation[4];
	float scale[3];
	float translation[3];
	std::string name;
} GltfNode;

typedef struct _GltfAnimation_Sampler {
	GltfAccessor* input;
    std::string interpolation;
    GltfAccessor* output;
	std::string name;
} GltfAnimation_Sampler;

typedef struct _GltfChannel {
	GltfAnimation_Sampler* sampler;
	GltfNode* targetNode;
	std::string targetPath;
	std::string name;
} GltfChannel;

typedef struct _GltfAnimation {
	Vector<GltfAnimation_Sampler> samplers;
	Vector<GltfChannel> channels;
	std::string name;
} GltfAnimation;

typedef struct _GltfScene {
	Vector<GltfNode*> nodes;
	std::string name;
} GltfScene;

class GltfVisitor : public JsonVisitor
{

private:

	const std::string directory;

	std::stack<enum GltfState> state;

	VkBool32 gltfBool;
	std::string gltfString;
	int32_t gltfInteger;
	float gltfFloat;
	int32_t gltfIntegerArray[16];
	float gltfFloatArray[16];

	uint32_t arrayIndex;
	uint32_t arraySize;
	VkBool32 numberArray;

	VkBool32 objectArray;

	GltfBuffer gltfBuffer;
	GltfBufferView gltfBufferView;
	GltfAccessor gltfAccessor;
	GltfPrimitive gltfPrimitive;
	GltfImage gltfImage;
	GltfSampler gltfSampler;
	GltfTexture gltfTexture;
	GltfMaterial gltfMaterial;
	GltfMesh gltfMesh;
	GltfSkin gltfSkin;
	GltfNode gltfNode;
	GltfAnimation_Sampler gltfAnimation_Sampler;
	GltfChannel gltfChannel;
	GltfAnimation gltfAnimation;
	GltfScene gltfScene;

	Vector<GltfBuffer> allGltfBuffers;
	Vector<GltfBufferView> allGltfBufferViews;
	Vector<GltfAccessor> allGltfAccessors;
	Vector<GltfImage> allGltfImages;
	Vector<GltfSampler> allGltfSamplers;
	Vector<GltfTexture> allGltfTextures;
	Vector<GltfMaterial> allGltfMaterials;
	Vector<GltfMesh> allGltfMeshes;
	Vector<GltfSkin> allGltfSkins;
	Vector<GltfNode> allGltfNodes;
	Vector<GltfAnimation> allGltfAnimations;
	Vector<GltfScene> allGltfScenes;
	GltfScene* defaultScene;

	void visitBuffer(JSONobject& jsonObject);
	void visitBufferView(JSONobject& jsonObject);
	void visitAccessor(JSONobject& jsonObject);
	void visitImage(JSONobject& jsonObject);
	void visitSampler(JSONobject& jsonObject);
	void visitTexture(JSONobject& jsonObject);
	void visitMaterial(JSONobject& jsonObject);
	void visitMesh(JSONobject& jsonObject);
	void visitSkin(JSONobject& jsonObject);
	void visitNode(JSONobject& jsonObject);
	void visitAnimation(JSONobject& jsonObject);
	void visitScene(JSONobject& jsonObject);

	void visitMaterial_Extras(JSONobject& jsonObject);
	void visitMaterial_PbrMetallicRoughness(JSONobject& jsonObject);
	void visitMaterial_TextureInfo(JSONobject& jsonObject);
	void visitMesh_Primitive(JSONobject& jsonObject);
	void visitMesh_Primitive_Attributes(JSONobject& jsonObject);
	void visitAnimation_Sampler(JSONobject& jsonObject);
	void visitAnimation_Sampler_Properties(JSONobject& jsonObject);
	void visitAnimation_Channel(JSONobject& jsonObject);
	void visitAnimation_Channel_Target(JSONobject& jsonObject);

public:

	GltfVisitor() = delete;

	explicit GltfVisitor(const std::string& directory);

	virtual ~GltfVisitor();

	//

	virtual void visit(JSONnull& jsonNull) override;

	virtual void visit(JSONfalse& jsonFalse) override;

	virtual void visit(JSONtrue& jsonTrue) override;

	virtual void visit(JSONfloat& jsonFloat) override;

	virtual void visit(JSONinteger& jsonInteger) override;

	virtual void visit(JSONstring& jsonString) override;

	virtual void visit(JSONarray& jsonArray) override;

	virtual void visit(JSONobject& jsonObject) override;

	//

	const std::string& getDirectory() const;

	enum GltfState getState() const;

	//

	const Vector<GltfBuffer>& getAllGltfBuffers() const;

	const Vector<GltfBufferView>& getAllGltfBufferViews() const;

	const Vector<GltfAccessor>& getAllGltfAccessors() const;

	const Vector<GltfImage>& getAllGltfImages() const;

	const Vector<GltfSampler>& getAllGltfSamplers() const;

	const Vector<GltfTexture>& getAllGltfTextures() const;

	const Vector<GltfMaterial>& getAllGltfMaterials() const;

	const Vector<GltfMesh>& getAllGltfMeshes() const;

	const Vector<GltfSkin>& getAllGltfSkins() const;

	const Vector<GltfNode>& getAllGltfNodes() const;

	const Vector<GltfAnimation>& getAllGltfAnimations() const;

	const Vector<GltfScene>& getAllGltfScenes() const;

	//

	const GltfScene* getDefaultScene() const;

	//

	VkBool32 isByte(const int32_t componentType) const;

	VkBool32 isUnsignedByte(const int32_t componentType) const;

	VkBool32 isShort(const int32_t componentType) const;

	VkBool32 isUnsignedShort(const int32_t componentType) const;

	VkBool32 isUnsignedInt(const int32_t componentType) const;

	VkBool32 isFloat(const int32_t componentType) const;

	uint32_t getBytesPerComponent(const int32_t componentType) const;

	uint32_t getComponentsPerType(const std::string& type) const;

	const void* getBufferPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const int8_t* getBytePointer(const GltfAccessor& accessor, const uint32_t element) const;

	const uint8_t* getUnsignedBytePointer(const GltfAccessor& accessor, const uint32_t element) const;

	const int16_t* getShortPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const uint16_t* getUnsignedShortPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const uint32_t* getUnsignedIntPointer(const GltfAccessor& accessor, const uint32_t element) const;

	const float* getFloatPointer(const GltfAccessor& accessor, const uint32_t element) const;

};

}

#endif /* VKTS_GLTFVISITOR_HPP_ */

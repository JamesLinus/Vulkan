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

#ifndef VKTS_GLTFPARAMETER_HPP_
#define VKTS_GLTFPARAMETER_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class GltfParameter : public Parameter
{

private:

	JSONobjectSP glTF;

	JSONarraySP scenes_nodes;
	int32_t rootNodeCounter;

	JSONarraySP nodes;
	SmartPointerMap<std::string, JSONarraySP> nodeNameToChildren;
	int32_t nodeCounter;

	JSONarraySP meshes;

	JSONarraySP primitives;

	JSONarraySP materials;
	SmartPointerMap<std::string, JSONobjectSP> materialNameToMaterial;

public:

	GltfParameter() :
		Parameter(), glTF(), scenes_nodes(), rootNodeCounter(0), nodes(), nodeCounter(0), meshes(), primitives(), materials(),  materialNameToMaterial()
    {
    }

    virtual ~GltfParameter()
    {
    }

    //

	const JSONobjectSP& getGlTf() const
	{
		return glTF;
	}

    //

    virtual void visit(IScene& scene)
    {
    	glTF = JSONobjectSP(new JSONobject());

    	if (!glTF.get())
    	{
    		return;
    	}

    	//
    	// asset
    	//

    	auto assetValue = JSONobjectSP(new JSONobject());

    	auto generatorValue = JSONstringSP(new JSONstring("VKTS glTF 2.0 exporter"));

    	auto versionValue = JSONstringSP(new JSONstring("2.0"));

    	if (!assetValue.get() || !generatorValue.get() || !versionValue.get())
    	{
    		return;
    	}

    	assetValue->addKeyValue("generator", generatorValue);
    	assetValue->addKeyValue("version", versionValue);

    	//

    	glTF->addKeyValue("asset", assetValue);

    	//
    	// scene
    	//

    	auto sceneValue = JSONintegerSP(new JSONinteger(0));

    	if (!sceneValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("scene", sceneValue);

    	//
    	// scenes
    	//

    	auto scenesValue = JSONarraySP(new JSONarray());

    	auto defaultScene = JSONobjectSP(new JSONobject());

    	auto scene_nodes = JSONarraySP(new JSONarray());

    	if (!scenesValue.get() || !defaultScene.get() || !scene_nodes.get())
    	{
    		return;
    	}

    	scenesValue->addValue(defaultScene);

    	defaultScene->addKeyValue("nodes", scene_nodes);

    	//

    	glTF->addKeyValue("scenes", scenesValue);

    	//

    	this->scenes_nodes = scene_nodes;

    	//
    	// nodes
    	//

    	auto nodesValue = JSONarraySP(new JSONarray());

    	if (!nodesValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("nodes", nodesValue);

    	//

    	this->nodes = nodesValue;

    	//
    	// meshes
    	//

    	auto meshesValue = JSONarraySP(new JSONarray());

    	if (!meshesValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("meshes", meshesValue);

    	//

    	this->meshes = meshesValue;

    	//
    	// materials
    	//

    	auto materialsValue = JSONarraySP(new JSONarray());

    	if (!materialsValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("materials", materialsValue);

    	//

    	this->materials = materialsValue;
    }

    virtual void visit(IObject& object)
    {
    	if (!scenes_nodes.get())
    	{
    		return;
    	}

    	//

    	auto rootNodeIndex = JSONintegerSP(new JSONinteger(rootNodeCounter));

    	if (!rootNodeIndex.get())
    	{
    		return;
    	}

    	//

    	scenes_nodes->addValue(rootNodeIndex);

    	//

    	rootNodeCounter++;
    }

    virtual void visit(INode& node)
    {
    	if (!nodes.get())
    	{
    		return;
    	}

    	//

    	auto currentNode = JSONobjectSP(new JSONobject());

    	if (!currentNode.get())
    	{
    		return;
    	}

    	nodes->addValue(currentNode);

    	//
    	//

    	auto childrenValue = JSONarraySP(new JSONarray());

    	if (!childrenValue.get())
    	{
    		return;
    	}

    	currentNode->addKeyValue("children", childrenValue);

    	nodeNameToChildren[node.getName()] = childrenValue;

    	//

		if (node.getMeshes().size() > 1)
		{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Node has %u meshes!", node.getMeshes().size());

			return;
		}

    	if (node.getMeshes().size() == 1)
    	{
        	auto meshValue = JSONintegerSP(new JSONinteger((int32_t)meshes->size()));

        	if (!meshValue.get())
        	{
        		return;
        	}

        	currentNode->addKeyValue("mesh", meshValue);
    	}

    	//

    	if (node.getRotate().x != 0.0f || node.getRotate().y != 0.0f || node.getRotate().z != 0.0f)
    	{
    		Quat rotation;

    		VkTsRotationMode rotationMode = node.getNodeRotationMode();

    		if (!(node.isNode() || node.isArmature()))
    		{
    			rotationMode = node.getBindRotationMode();
    		}

        	switch (rotationMode)
        	{
        		case VKTS_EULER_YXZ:
        			rotation = rotateRzRxRy(node.getRotate().z, node.getRotate().x, node.getRotate().y);
        			break;
        		case VKTS_EULER_XYZ:
        			rotation = rotateRzRyRx(node.getRotate().z, node.getRotate().y, node.getRotate().x);
        			break;
        		case VKTS_EULER_XZY:
        			rotation = rotateRyRzRx(node.getRotate().y, node.getRotate().z, node.getRotate().x);
        			break;
        	}

    		//

        	auto rotationValue = JSONarraySP(new JSONarray());

        	auto rotationX = JSONfloatSP(new JSONfloat(rotation.x));
        	auto rotationY = JSONfloatSP(new JSONfloat(rotation.y));
        	auto rotationZ = JSONfloatSP(new JSONfloat(rotation.z));
        	auto rotationW = JSONfloatSP(new JSONfloat(rotation.w));

        	if (!rotationValue.get() || !rotationX.get() || !rotationY.get() || !rotationZ.get() || !rotationW.get())
        	{
        		return;
        	}

        	rotationValue->addValue(rotationX);
        	rotationValue->addValue(rotationY);
        	rotationValue->addValue(rotationZ);
        	rotationValue->addValue(rotationW);

        	//

        	currentNode->addKeyValue("rotation", rotationValue);
    	}

    	if (node.getScale().x != 1.0f || node.getScale().y != 1.0f || node.getScale().z != 1.0f)
    	{
        	auto scaleValue = JSONarraySP(new JSONarray());

        	auto scaleX = JSONfloatSP(new JSONfloat(node.getScale().x));
        	auto scaleY = JSONfloatSP(new JSONfloat(node.getScale().y));
        	auto scaleZ = JSONfloatSP(new JSONfloat(node.getScale().z));

        	if (!scaleValue.get() || !scaleX.get() || !scaleY.get() || !scaleZ.get())
        	{
        		return;
        	}

        	scaleValue->addValue(scaleX);
        	scaleValue->addValue(scaleY);
        	scaleValue->addValue(scaleZ);

        	//

        	currentNode->addKeyValue("scale", scaleValue);
    	}

    	if (node.getTranslate().x != 0.0f || node.getTranslate().y != 0.0f || node.getTranslate().z != 0.0f)
    	{
        	auto translationValue = JSONarraySP(new JSONarray());

        	auto translationX = JSONfloatSP(new JSONfloat(node.getTranslate().x));
        	auto translationY = JSONfloatSP(new JSONfloat(node.getTranslate().y));
        	auto translationZ = JSONfloatSP(new JSONfloat(node.getTranslate().z));

        	if (!translationValue.get() || !translationX.get() || !translationY.get() || !translationZ.get())
        	{
        		return;
        	}

        	translationValue->addValue(translationX);
        	translationValue->addValue(translationY);
        	translationValue->addValue(translationZ);

        	//

        	currentNode->addKeyValue("translation", translationValue);
    	}

    	//

    	auto nameValue = JSONstringSP(new JSONstring(node.getName()));

    	if (!nameValue.get())
    	{
    		return;
    	}

    	currentNode->addKeyValue("name", nameValue);

    	//
    	// Process children index.
    	//

    	if (node.getParentNode().get())
    	{
    		if (!nodeNameToChildren.contains(node.getParentNode()->getName()))
    		{
    			return;
    		}

    		auto parentChildren = nodeNameToChildren[node.getParentNode()->getName()];

    		//

        	auto nodeIndex = JSONintegerSP(new JSONinteger(nodeCounter));

        	if (!nodeIndex.get())
        	{
        		return;
        	}

        	parentChildren->addValue(nodeIndex);
    	}

    	//

    	nodeCounter++;
    }

    virtual void visit(IMesh& mesh)
    {
    	if (!meshes.get() || mesh.getNumberSubMeshes() == 0)
    	{
    		return;
    	}

    	auto currentMesh = JSONobjectSP(new JSONobject());

    	if (!currentMesh.get())
    	{
    		return;
    	}

    	meshes->addValue(currentMesh);

    	//

    	auto primitivesValue = JSONarraySP(new JSONarray());

    	if (!primitivesValue.get())
    	{
    		return;
    	}

    	currentMesh->addKeyValue("primitives", primitivesValue);

    	this->primitives = primitivesValue;

    	//

    	auto nameValue = JSONstringSP(new JSONstring(mesh.getName()));

    	if (!nameValue.get())
    	{
    		return;
    	}

    	currentMesh->addKeyValue("name", nameValue);
    }

    virtual void visit(ISubMesh& subMesh)
    {
    	if (!primitives.get())
    	{
    		return;
    	}

    	auto currentPrimitive = JSONobjectSP(new JSONobject());

    	if (!currentPrimitive.get())
    	{
    		return;
    	}

    	primitives->addValue(currentPrimitive);

    	//
    	// Mode
		//

    	int32_t mode = 0;

    	switch (subMesh.getPrimitiveTopology())
    	{
			case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
				mode = 0;
				break;
			case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
				mode = 1;
				break;
			case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
				mode = 3;
				break;
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
				mode = 4;
				break;
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
				mode = 5;
				break;
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
				mode = 6;
				break;
			default:
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "SubMesh has unsupported topology 0x%x!", (uint32_t)subMesh.getPrimitiveTopology());
				return;
    	}

    	auto modeValue = JSONintegerSP(new JSONinteger(mode));

    	if (!modeValue.get())
    	{
    		return;
    	}

    	currentPrimitive->addKeyValue("mode", modeValue);

    	//
    	// material
    	//

    	if (!subMesh.getBSDFMaterial().get() || !subMesh.getBSDFMaterial()->isSorted())
    	{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "SubMesh has unsupported material!");
			return;
    	}

    	//

    	if (!materialNameToMaterial.contains(subMesh.getBSDFMaterial()->getName()))
    	{
        	auto currentMaterial = JSONobjectSP(new JSONobject());

        	if (!currentMaterial.get())
        	{
        		return;
        	}

        	materials->addValue(currentMaterial);

        	materialNameToMaterial[subMesh.getBSDFMaterial()->getName()] = currentMaterial;
    	}

    	//

    	auto materialValue = JSONintegerSP(new JSONinteger((int32_t)materials->size() - 1));

    	if (!materialValue.get())
    	{
    		return;
    	}

    	currentPrimitive->addKeyValue("material", materialValue);

    	//

    	// TODO: Gather primitives values.
    }

    virtual void visit(IBSDFMaterial& material)
    {
    	if (!materials.get() || !materialNameToMaterial.contains(material.getName()))
    	{
    		return;
    	}

    	//

    	// Check, if values already gathered.
    	auto currentMaterial = materialNameToMaterial[material.getName()];

    	if (currentMaterial->size() != 0)
    	{
    		return;
    	}

    	//

    	auto nameValue = JSONstringSP(new JSONstring(material.getName()));

    	if (!nameValue.get())
    	{
    		return;
    	}

    	currentMaterial->addKeyValue("name", nameValue);

    	//

    	// TODO: Gather material parameters.
    }

};

} /* namespace vkts */

#endif /* VKTS_GLTFPARAMETER_HPP_ */
